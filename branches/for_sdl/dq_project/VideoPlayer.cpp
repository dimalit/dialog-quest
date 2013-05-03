#include <stdlib.h>
#include <theora/theoradec.h>
#include <theora/theora.h>
#include "VideoPlayer.h"
 
#ifdef WIN32
#ifdef _MSC_VER
#ifdef _DEBUG
#pragma comment(lib, "libogg_static_d.lib")
#pragma comment(lib, "libtheora_static_d.lib")
#else
#pragma comment(lib, "libogg_static.lib")
#pragma comment(lib, "libtheora_static.lib")
#endif
#endif
#endif
 
#include <vector>
#include <fstream>
 
template<class T> T clamp(const T& _v, const T& _a, const T& _b)
{
	return _v < _a ? _a : _v > _b ? _b : _v;
}
 
 
class VideoPlayer::Impl
{
public:
	Impl(const char* d, std::size_t size) :
		mHge(hgeCreate(HGE_VERSION)), data(&d[0], &d[size]), status(true), current(0), t_count(0), t_ctx(0), t_setup(0), t_frame_time(0), t_time(0), pp_inc(0),
			frames(0), dropped(0)
	{
		if(d){
			ogg_sync_init(&o_state);
			th_info_init(&t_info);
			th_comment_init(&t_comment);
		}// only for non-NULLs!
	}
 
	~Impl()
	{
		if(data.size()){
			th_info_clear(&t_info);
			th_comment_clear(&t_comment);
			th_setup_free(t_setup);
			th_decode_free(t_ctx);
			ogg_stream_clear(&t_state);
			ogg_sync_clear(&o_state);
		}
		mHge->Release();
	}
 
	bool FeedPage()
	{
		while (true)
		{
			if (ogg_sync_pageout(&o_state, &o_page) > 0)
				return true;
			if (current >= data.size())
				return false;
			std::size_t size(data.size() - current);
			if (size > 4096)
			{
				size = 4096;
			}
			char* buffer(ogg_sync_buffer(&o_state, size));
			memcpy(buffer, &data[current], size);
			ogg_sync_wrote(&o_state, size);
			current += size;
			if (size == 0)
				return false;
		}
	}
 
	bool FeedPacket()
	{
		while (true)
		{
			int ret(ogg_stream_packetout(&t_state, &o_packet));
			if (ret == 1)
				return true;
			if (ret < 0)
				break;
			if (!FeedPage())
				break;
			int serialno(ogg_page_serialno(&o_page));
			if (serialno == t_state.serialno)
			{
				ogg_stream_pagein(&t_state, &o_page);
			}
		}
		return status = false;
	}
 
	bool FeedHeaders()
	{
		while (true)
		{
			if (!FeedPage())
				return status = false;
			if (!ogg_page_bos(&o_page))
			{
				if (t_count)
				{
					ogg_stream_pagein(&t_state, &o_page);
				}
				break;
			}
			ogg_stream_state test;
			ogg_stream_init(&test, ogg_page_serialno(&o_page));
			ogg_stream_pagein(&test, &o_page);
			if (ogg_stream_packetout(&test, &o_packet) == 1 && t_count == 0 && th_decode_headerin(&t_info, &t_comment, &t_setup, &o_packet) >= 0)
			{
				memcpy(&t_state, &test, sizeof(test));
				t_count = 1;
			}
			else
			{
				ogg_stream_clear(&test);
			}
		}
		if (!t_count)
			return status = false;
		while (t_count < 3)
		{
			int ret(ogg_stream_packetout(&t_state, &o_packet));
			if (ret < 0)
				return status = false;
			if (ret == 0)
			{
				if (!FeedPage())
					return status = false;
				if (ogg_page_serialno(&o_page) == t_state.serialno)
				{
					ogg_stream_pagein(&t_state, &o_page);
				}
				continue;
			}
			if (!th_decode_headerin(&t_info, &t_comment, &t_setup, &o_packet))
				return status = false;
			++t_count;
		}
		t_ctx = th_decode_alloc(&t_info, t_setup);
		th_setup_free(t_setup);
		t_setup = 0;
		t_frame_time = 1000.0f * t_info.fps_denominator / t_info.fps_numerator;
		th_decode_ctl(t_ctx, TH_DECCTL_GET_PPLEVEL_MAX, &pp_level_max, sizeof(pp_level_max));
		pp_level = pp_level_max;
		th_decode_ctl(t_ctx, TH_DECCTL_SET_PPLEVEL, &pp_level, sizeof(pp_level));
		return status;
	}
 
	HTEXTURE CreateTexture(const std::string& name) const
	{
		return status ? mHge->Texture_Create(t_info.pic_width, t_info.pic_height) : 0;
	}
 
	bool Update(float time, HTEXTURE texture)
	{
		if (!status)
			return false;
		if (t_time > time)
			return true;
		while (FeedPacket())
		{
			if (pp_inc)
			{
				pp_level += pp_inc;
				th_decode_ctl(t_ctx, TH_DECCTL_SET_PPLEVEL, &pp_level, sizeof(pp_level));
				pp_inc = 0;
			}
			if (o_packet.granulepos >= 0)
			{
				th_decode_ctl(t_ctx, TH_DECCTL_SET_GRANPOS, &o_packet.granulepos, sizeof(o_packet.granulepos));
			}
			ogg_int64_t videobuf_granulepos;
			if (th_decode_packetin(t_ctx, &o_packet, &videobuf_granulepos) == 0)
			{
				t_time = (float)th_granule_time(t_ctx, videobuf_granulepos);
				++frames;
 
				if (t_time >= time)
					break;
				pp_inc = pp_level > 0 ? -1 : 0;
				++dropped;
			}
		}
		float tdiff(t_time - time);
		if (tdiff > t_frame_time)
		{
			pp_inc = pp_level < pp_level_max ? 1 : 0;
		}
		else if (tdiff < t_frame_time)
		{
			pp_inc = pp_level > 0 ? -1 : 0;
		}
		Output(texture);
		return true;
	}
 
	void Output(HTEXTURE texture)
	{
		th_ycbcr_buffer yuv;
		th_decode_ycbcr_out(t_ctx, yuv);
		int dstStride(0);
		if (BYTE* buffer = (BYTE*)LockTexture(texture, dstStride))
		{
			dstStride -= mHge->Texture_GetWidth(texture);
			dstStride *= -4; // !!!
			const unsigned char* y_data(yuv[0].data);
			const unsigned char* u_data(yuv[1].data);
			const unsigned char* v_data(yuv[2].data);
			int yStride(yuv[0].stride);
			int uvStride(yuv[1].stride);
			switch (t_info.pixel_fmt)
			{
			case TH_PF_420:
				for (unsigned i = 0; i < t_info.pic_height; ++i, buffer += dstStride, y_data += yStride)
				{
					const unsigned char* y_p(y_data);
					const unsigned char* u_p(u_data);
					const unsigned char* v_p(v_data);
					for (unsigned j = 0; j < t_info.pic_width; ++j, buffer += 4, ++y_p)
					{
						int y(9535 * (*y_p - 15));
						int u(*u_p - 128);
						int v(*v_p - 128);
						buffer[2] = clamp<int>((y + 13074 * v) >> 13, 0, 255); // red
						buffer[1] = clamp<int>((y - 6660 * v - 3203 * u) >> 13, 0, 255); // green
						buffer[0] = clamp((y + 16531 * u) >> 13, 0, 255); // blue
						buffer[3] = 255;
						if (j & 1)
						{
							++u_p;
							++v_p;
						}
					}
					if (i & 1)
					{
						u_data += uvStride;
						v_data += uvStride;
					}
				}
				break;
 
			case TH_PF_422:
				break;
 
			case TH_PF_444:
				break;
 
			default:
				break;
			}
			UnlockTexture(texture);
		}
	}
 
	  bool GetStatus() const
	  {
		  return status;
	  }
 
	  DWORD* LockTexture(HTEXTURE texture, int& stride) const
 	  {
 		  stride = t_info.pic_width;
 		  return mHge->Texture_Lock(texture, false, 0, 0, t_info.pic_width, t_info.pic_height);
 	  }
 
 	  void UnlockTexture(HTEXTURE texture)
 	  {
 		  mHge->Texture_Unlock(texture);
 	  }
 
	HGE* mHge;
	std::vector<char> data;
	bool status;
	std::size_t current;
	ogg_sync_state o_state;
	ogg_page o_page;
	ogg_packet o_packet;
	ogg_stream_state t_state;
	int t_count;
	th_info t_info;
	th_comment t_comment;
	th_dec_ctx* t_ctx;
	th_setup_info* t_setup;
	float t_frame_time;
	float t_time;
	int pp_level;
	int pp_level_max;
	int pp_inc;
	int frames;
	int dropped;
};
 
VideoPlayer::VideoPlayer() :
	mTexture(0), mTime(0)
{
	mHge = hgeCreate(HGE_VERSION);
}
 
 
VideoPlayer::~VideoPlayer()
{
	Close();
}
 
const char* VideoPlayer::LoadFile(const std::string& fileName, DWORD& size)
{
	return (const char*)mHge->Resource_Load(fileName.c_str(), &size);
}
 
bool VideoPlayer::Open(const std::string& fileName, const hgeVector& posPlayer, const hgeVector& sizePlayer)
{
	DWORD size;
	const char* data = LoadFile(fileName, size);
	if(data == 0)return false;
	return Open((void*)data, size, posPlayer, sizePlayer);
}

bool VideoPlayer::Open(void* data, DWORD size, const hgeVector& posPlayer, const hgeVector& sizePlayer)
{
	if (mTexture)
	{
		Close();
	}
 
	mImpl.reset(new Impl((const char*)data, (size_t)size));
	if (mImpl->FeedHeaders())
	{
		char buf[12];
		sprintf(buf,"%x",(int)data);
		mTexture = mImpl->CreateTexture(buf);
		float w = (float)mHge->Texture_GetWidth(mTexture);
		float h = (float)mHge->Texture_GetHeight(mTexture);
		mSprite = new hgeSprite(mTexture, 0, 0, w, h);
		mSprite->SetHotSpot(0.0f, 0.0f);
	
		mPos = posPlayer;

		if(sizePlayer.x != 0.0f && sizePlayer.y != 0.0f){
			mScale.x = sizePlayer.x / mImpl->t_info.pic_width;
			mScale.y = sizePlayer.y / mImpl->t_info.pic_height;
		}
		else{
			mScale = hgeVector(1.0f, 1.0f);
		}

		return true;
	}
	mImpl.reset();
	return false;
}
 
void VideoPlayer::Close()
{
	if (mTexture)
	{
		mHge->Texture_Free(mTexture);
		mTexture = 0;
	}
	mTime = 0;
	mImpl.reset();
	mHge->Release();
}
 
void VideoPlayer::Update(float time)
{
	mTime += time;
	mImpl->Update(mTime, mTexture);
}
 
bool VideoPlayer::IsPlaying() const
{
	return mImpl.get() && mImpl->GetStatus();
}
 
void VideoPlayer::Render()
{
	mSprite->RenderEx(mPos.x, mPos.y, 0, mScale.x, mScale.y);
}