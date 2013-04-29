#include <string>
#include <cstring>
#include <fstream>
#include <time.h>
#include <stdio.h>

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <shlobj.h>

using boost::asio::ip::tcp;
using namespace boost::filesystem;

#include "main.h"

std::string get_localdata_folder();

void switch_to_english(){
	HKL buf[20];
	// get list
	unsigned n = GetKeyboardLayoutList(20, buf);
	if(n <= 0) return; // :(

	// fin first 0x409
	char str[20];
	for(int i=0; i<n; i++){
		// english?
		if((((unsigned)buf[i]) & 0xfff) == 0x409){
			ActivateKeyboardLayout(buf[i],0);
			break;
		}
	}// for
}


// returns true if need restart
bool goto_localdata_folder(){
	path dst_path(get_localdata_folder());
	path src_path(hge->Resource_MakePath());
	if(equivalent(dst_path, src_path))return false;// it's OK

	// create dir
	boost::system::error_code ec;
	copy_directory(src_path, dst_path, ec);
	directory_iterator it(src_path);
	// copy files
	while(it != directory_iterator()){
		path src_file(it->path());
		path dst_file(dst_path); dst_file /= src_file.filename();
		copy_file(src_file, dst_file, copy_option::overwrite_if_exists);
		++it;
	}
	
	// execute!
    PROCESS_INFORMATION processInformation;
    STARTUPINFO startupInfo;
    memset(&processInformation, 0, sizeof(processInformation));
    memset(&startupInfo, 0, sizeof(startupInfo));
    startupInfo.cb = sizeof(startupInfo);

	char str_module[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), str_module, sizeof(str_module));
	path dst_exe(dst_path); dst_exe /= path(str_module).filename();
    bool result = ::CreateProcess(dst_exe.string().c_str(), NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, dst_path.string().c_str() , &startupInfo, &processInformation);
	return result;
}

bool file_present(const char* file_path){
	path p(file_path);
	try{
		unsigned size = file_size(p);
		if(size > 1)
			return true;
		else
			return false;
	}catch(...){
		return false;
	}
}

bool download_resources(){
	std::string folder = get_localdata_folder();
	std::string files[] = {"I-15bis.ogg"};//, "font1.fnt", "bg.png", "circle.png", "cursor.png", "font1.png", "menu.wav"};
	for(int i=0; i<1; i++){
		std::string path = folder + "\\" + files[i];
		bool ok = true;
		if(!file_present(path.c_str()))
			ok = http2file("http://upload.wikimedia.org/wikipedia/commons/b/b5/"+files[i], folder);
		hge->System_Log("Downloading %s", files[i].c_str());
		if(!ok)
			return false;
	}// for i
	return true;
}

std::string get_localdata_folder(){
	char buf[MAX_PATH];

	if(SUCCEEDED(SHGetFolderPath(NULL,
								 CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, 
								 NULL, 
								 0, 
								 buf))) 
	{
		strcat(buf, "\\");
		strcat(buf, APP_NAME);
		try{
			create_directory(path(buf));
		}catch(...){
			return "";
		}
		return std::string(buf);
	}
	else
		return "";
}

bool http2file(std::string from_path, std::string to_path){
  boost::asio::io_service io_service;

	if(to_path.empty())
		to_path = get_localdata_folder();

  // get file name and append it to to_path
  size_t pos2 = from_path.find_last_not_of("/");
  size_t pos1 = from_path.find_last_of("/", pos2);
  to_path += "\\" + from_path.substr(pos1+1, pos2-pos1);
  

  std::ofstream out_file(to_path.c_str(), std::ios::binary);
  if(!out_file.is_open())
	return 0;
  try{
    // Get a list of endpoints corresponding to the server name.
    tcp::resolver resolver(io_service);
	std::string host;
	if(strlen(hge->Ini_GetString("inet", "PROXY", "")))
	    host = hge->Ini_GetString("inet", "PROXY", "");
	else
		host = hge->Ini_GetString("inet", "HTTP_HOST", "");
	tcp::resolver::query query(host, "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::resolver::iterator end;

    // Try each endpoint until we successfully establish a connection.
    tcp::socket socket(io_service);
    boost::system::error_code error = boost::asio::error::host_not_found;
    while (error && endpoint_iterator != end)
    {
      socket.close();
	  tcp::endpoint ep(*endpoint_iterator++);
	  if(strlen(hge->Ini_GetString("inet", "PROXY", ""))){
		ep.port( hge->Ini_GetInt("inet", "PROXY_PORT", 80) );
	  }
      socket.connect(ep, error);
    }
	if (error){
	  out_file.close();
      return 0;
	}

    // Form the request. We specify the "Connection: close" header so that the
    // server will close the socket after transmitting the response. This will
    // allow us to treat all data up until the EOF as the content.
    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "GET " << from_path << " HTTP/1.0\r\n";
    request_stream << "Host: " << hge->Ini_GetString(0, "HTTP_HOST", "") << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n\r\n";

    // Send the request.
    boost::asio::write(socket, request);

    // Read the response status line.
    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n");

    // Check that response is OK.
    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    unsigned int status_code;
    response_stream >> status_code;
    std::string status_message;
    std::getline(response_stream, status_message);
    if (!response_stream || http_version.substr(0, 5) != "HTTP/" || status_code != 200)
    {
	  out_file.close();
      return 0;
    }

    // Read the response headers, which are terminated by a blank line.
    boost::asio::read_until(socket, response, "\r\n\r\n");

    // Process the response headers.
    std::string header;
    while (std::getline(response_stream, header) && header != "\r");

	// Write whatever content we already have to output.
    if (response.size() > 0)
      out_file << &response;

	out_file.flush();

    // Read until EOF, writing data to output as we go.
    while (boost::asio::read(socket, response,
          boost::asio::transfer_at_least(1), error))
      out_file << &response;
	
	out_file.close();
    
	if (error != boost::asio::error::eof)
      throw boost::system::system_error(error);
  }
  catch (std::exception& e)
  {
    out_file.close();
    return 0;
  }
  return 1;
}