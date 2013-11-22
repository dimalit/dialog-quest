/*
 *  App.cpp
 *  Created by Seth Robinson on 3/6/09.
 *  For license info, check the license.txt file that should have come with this.
 *
 */

#include "PlatformPrecomp.h"
#include "App.h"
#include "GUI/MainMenu.h"
#include "GUI/ImageTestMenu.h"
#include "Renderer/LinearParticle.h"
#include "Entity/EntityUtils.h"//create the classes that our globally library expects to exist somewhere.
#include "Renderer/SoftSurface.h"
#include "GUI/AboutMenu.h"

#include "Image.h"
#include "Animation.h"
#include "Text.h"
#include "LuaScreenItem.h"
#include "LuaTimer.h"
#include "LuaSoundEffect.h"
#include "Texture.h"
#include "TextInput.h"

#include "LuaCassowary.h"
#include "lua_lib.h"



#include <luabind/class_info.hpp>
#include <stdio.h>

SurfaceAnim g_surf;
 
MessageManager g_messageManager;
MessageManager * GetMessageManager() {return &g_messageManager;}

FileManager g_fileManager;
FileManager * GetFileManager() {return &g_fileManager;}

lua_State* L;

#ifdef __APPLE__

#if TARGET_OS_IPHONE == 1
	//it's an iPhone or iPad
	//#include "Audio/AudioManagerOS.h"
	//AudioManagerOS g_audioManager;
	#include "Audio/AudioManagerDenshion.h"
	
	AudioManagerDenshion g_audioManager;
#else
	//it's being compiled as a native OSX app
   #include "Audio/AudioManagerFMOD.h"
  AudioManagerFMOD g_audioManager; //dummy with no sound

//in theory, CocosDenshion should work for the Mac builds, but right now it seems to want a big chunk of
//Cocos2d included so I'm not fiddling with it for now

//#include "Audio/AudioManagerDenshion.h"
//AudioManagerDenshion g_audioManager;
#endif
	
#else

#if defined RT_WEBOS || defined RTLINUX
#include "Audio/AudioManagerSDL.h"
AudioManagerSDL g_audioManager; //sound in windows and WebOS
//AudioManager g_audioManager; //to disable sound
#elif defined ANDROID_NDK
#include "Audio/AudioManagerAndroid.h"
AudioManagerAndroid g_audioManager; //sound for android
#elif defined PLATFORM_BBX
#include "Audio/AudioManagerBBX.h"
//AudioManager g_audioManager; //to disable sound
AudioManagerBBX g_audioManager;
#elif defined PLATFORM_FLASH
//AudioManager g_audioManager; //to disable sound
#include "Audio/AudioManagerFlash.h"
AudioManagerFlash *g_audioManager = new AudioManagerFlash;
#else


//in windows
//AudioManager g_audioManager; //to disable sound

#ifdef RT_FLASH_TEST
#include "Audio/AudioManagerFlash.h"
AudioManagerFlash g_audioManager;
#else

#include "Audio/AudioManagerAudiere.h"
AudioManagerAudiere g_audioManager;  //Use Audiere for audio
#endif
//#include "Audio/AudioManagerFMOD.h"
//AudioManagerFMOD g_audioManager; //if we wanted FMOD sound in windows

#endif
#endif

#if defined PLATFORM_FLASH
	AudioManager * GetAudioManager(){return g_audioManager;}
#else
	AudioManager * GetAudioManager(){return &g_audioManager;}
#endif

App *g_pApp = NULL;
BaseApp * GetBaseApp() 
{
	if (!g_pApp)
	{
		#ifndef NDEBUG
		LogMsg("Creating app object");
		#endif
		g_pApp = new App;
	}

	return g_pApp;
}

App * GetApp() 
{
	return g_pApp;
}

App::App()
{
	m_bDidPostInit = false;
}

App::~App()
{
	L_ParticleSystem::deinit();
#ifdef PLATFORM_FLASH
	SAFE_DELETE(g_audioManager);
#endif
}

void App::OnExitApp(VariantList *pVarList)
{
	LogMsg("Exiting the app");

	OSMessage o;
	o.m_type = OSMessage::MESSAGE_FINISH_APP;
	GetBaseApp()->AddOSMessage(o);
}

int lua_error_handler(lua_State* L){
	const char* msg = lua_tostring(L, -1);
	assert(msg);
	std::cout << msg << std::endl;
	lua_pop(L, 1);

	luaL_dostring(L, "print(debug.traceback(\"\", 4))");
	exit(1);

//	lua_Debug d;
//	lua_getinfo(L, "Sln", &d);
//	std::cout << d.short_src << ":" << d.currentline;
	lua_pushstring(L, "error");
	return 1;
}

void gc_on_update(){
	lua_gc(L, LUA_GCCOLLECT, 0);
}

bool App::Init()
{
	//SetDefaultAudioClickSound("audio/enter.wav");
	SetDefaultButtonStyle(Button2DComponent::BUTTON_STYLE_CLICK_ON_TOUCH_RELEASE);
	//SetManualRotationMode(true); //commented out, so iOS will handle rotations, plays better with 3rd party libs and looks cool

    bool bScaleScreenActive = false; //!!! if true, we'll stretch every screen to the coords below
    int scaleToX = 480;
	int scaleToY = 320;
    
	switch (GetEmulatedPlatformID())
	{
		//special handling for certain platforms to tweak the video settings

	case PLATFORM_ID_WEBOS:
		//if we do this, everything will be stretched/zoomed to fit the screen
		if (IsIPADSize)
		{
			//doesn't need rotation
			SetLockedLandscape(false);  //because it's set in the app manifest, we don't have to rotate ourselves
			SetupScreenInfo(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT);
            if (bScaleScreenActive)
                SetupFakePrimaryScreenSize(scaleToX,scaleToY); //game will think it's this size, and will be scaled up
		} 
else
		{
			//but the phones do
			SetLockedLandscape(true); //we don't allow portrait mode for this game
            if (bScaleScreenActive)
                SetupFakePrimaryScreenSize(scaleToX,scaleToY); //game will think it's this size, and will be scaled up
		}
		
		break;

		case PLATFORM_ID_IOS:
			SetLockedLandscape(true); //we stay in portrait but manually rotate, gives better fps on older devices
            if (bScaleScreenActive)
                SetupFakePrimaryScreenSize(scaleToX,scaleToY); //game will think it's this size, and will be scaled up
			break;
			
	default:
		
		//Default settings for other platforms

		SetLockedLandscape(false); //we don't allow portrait mode for this game
		SetupScreenInfo(GetPrimaryGLX(), GetPrimaryGLY(), ORIENTATION_PORTRAIT);
            if (bScaleScreenActive)
                SetupFakePrimaryScreenSize(scaleToX,scaleToY); //game will think it's this size, and will be scaled up
	}

	L_ParticleSystem::init(2000);

	if (m_bInitted)	
	{
		return true;
	}
	
	if (!BaseApp::Init()) return false;


	LogMsg("Save path is %s", GetSavePath().c_str());

	if (!GetFont(FONT_SMALL)->Load("interface/font_times.rtfont")) 
	{
		LogMsg("Can't load font 1");
		return false;
	}
	if (!GetFont(FONT_LARGE)->Load("interface/font_trajan.rtfont"))
	{
		LogMsg("Can't load font 2");
		return false;
	}
	if (!GetFont(FONT_PHONETIC)->Load("interface/font_phonetic.rtfont"))
	{
		LogMsg("Can't load font 3");
		return false;
	}
	if (!GetFont(FONT_TIMES_14)->Load("interface/font_times_14.rtfont"))
	{
		LogMsg("Can't load font 3");
		return false;
	}
	//GetFont(FONT_SMALL)->SetSmoothing(false); //if we wanted to disable bilinear filtering on the font

	GetBaseApp()->SetFPSLimit(25);
	GetBaseApp()->SetFPSVisible(true);
	
	bool bFileExisted;
	m_varDB.Load("save.dat", &bFileExisted);
 
	//preload audio
	GetAudioManager()->Preload("audio/click.wav");
	//GetAudioManager()->Preload("audio/techno.mp3");

//	freopen("xtree.txt", "wb", stdout);

	L = lua_open();
	luaL_openlibs(L);
	luabind::open(L);
	luabind::set_pcall_callback(&lua_error_handler);
	luabind::bind_class_info(L);

	lua_gc(L, LUA_GCSTOP, 0);			// !!!

	Lualib::luabind(L);

	LuaScreenItem::luabind(L);
	LuaCompositeItem::luabind(L);

	LuaImageItem::luabind(L);
	LuaAnimatedItem::luabind(L);
	LuaTextItem::luabind(L);
	LuaTextBoxItem::luabind(L);
	LuaTextureItem::luabind(L);
	LuaTextInputItem::luabind(L);

	LuaCassowary::luabind(L);

	LuaTimer::luabind(L, "Timer");	// TODO: Why name?
	LuaSoundEffect::luabind(L);

	lua_pushcfunction(L, lua_error_handler);		// don't move it below: order matters
    if(luaL_loadfile(L, "lib.lua")!=0){
		std::cout << lua_tostring(L,-1) << "\n";
		return false;
	}
	lua_pcall(L, 0, LUA_MULTRET, -2);

	return true;
}

void App::SaveOurStuff()
{
	LogMsg("Saving our stuff");
	m_varDB.Save("save.dat");
}

void App::Kill()
{
	SaveOurStuff();
	BaseApp::Kill();
	g_pApp = NULL;
}

void App::PostInitIfNeeded(){
	if (!m_bDidPostInit)
	{
		m_bDidPostInit = true;
		m_special = GetSystemData() != C_PIRATED_NO;

		//build a dummy entity called "GUI" to put our GUI menu entities under
		Entity *pGUIEnt = GetEntityRoot()->AddEntity(new Entity("GUI"));
		// TODO: Where should I put initialization: here or App:Init?
		//MainMenuCreate(pGUIEnt);
		ImageTestMenuCreate(pGUIEnt);
	}
}

void App::Update()
{
	PROFILE_BEGIN(App_Update);
	BaseApp::Update();
	PostInitIfNeeded();
//temporarily disable	gc_on_update();
//	PROFILE_END();
}

void App::Draw()
{
	PROFILE_FUNC();

//	std::cout << "begin render" << std::endl;
	PrepareForGL();
//	glClearColor(0.6,0.6,0.6,1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BaseApp::Draw();
	//root_item()->print_need_lay_out();
	//std::cout << "end render" << std::endl;
}

void App::OnEnterBackground()
{
	BaseApp::OnEnterBackground();
	SaveOurStuff();

}
void App::OnScreenSizeChange()
{
	BaseApp::OnScreenSizeChange();
}

void App::GetServerInfo( string &server, uint32 &port )
{
#if defined (_DEBUG) && defined(WIN32)
	server = "localhost";
	port = 8080;

	//server = "www.rtsoft.com";
	//port = 80;
#else

	server = "rtsoft.com";
	port = 80;
#endif
}

int App::GetSpecial()
{
	return m_special; //1 means pirated copy
}

Variant * App::GetVar( const string &keyName )
{
	return GetShared()->GetVar(keyName);
}

std::string App::GetVersionString()
{
	return "V0.7";
}

float App::GetVersion()
{
	return 0.7f;
}

int App::GetBuild()
{
	return 1;
}

const char * GetAppName() {return "SimpleApp";}

//for palm webos and android
const char * GetBundlePrefix()
{
	const char * bundlePrefix = "com.rtsoft.";
	return bundlePrefix;
}

const char * GetBundleName()
{
	const char * bundleName = "rtsimpleapp";
	return bundleName;
}
