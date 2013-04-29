/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** hge_tut06 - Creating menus
*/


// Copy the files "menu.wav", "font1.fnt", "font1.png",
// "bg.png" and "cursor.png" from the folder "precompiled"
// to the folder with executable file. Also copy hge.dll
// and bass.dll to the same folder.


#include <math.h>
#include <cassert>
#include <iostream>

#include <hge.h>
#include <hgefont.h>
#include <hgeanim.h>

#include <lua.hpp>
#include <cpptcl.h>

#include "main.h"
#include "menuitem.h"
#include "FrameTimer.h"
#include "SingleSelectionMenu.h"
#include "VideoPlayer.h"
#include "MovableItem.h"

#include "LuaImageWidget.h"
#include "LuaTextWidget.h"
#include "Image.h"
#include "Text.h"
#include "Animation.h"
#include "Frame.h"
#include "LuaScreenItem.h"
#include "LuaSoundEffect.h"
#include "LuaTimer.h"
#include "lua_layers.h"
#include "lua_lib.h"

int lua_error_handler(lua_State* L){
	std::cerr << lua_tostring(L, -1) << std::endl;
	lua_pop(L, 1);

	luaL_dostring(L, "print(debug.traceback(\"\", 4))");

	lua_Debug d;
	lua_getinfo(L, "Sln", &d);
	std::cerr << d.short_src << ":" << d.currentline;

	//lua_Debug d;
	//int lev = 0;
	//while(lua_getstack(L, lev--, &d)){
	//	lua_getinfo(L, ">n", &d);
	//	if(d.name)
	//		std::cerr << d.name << std::endl;
	//}
	return -1;
}

FrameTimer* frame_timer = FrameTimer::getInstance();
bool need_exit = false;

class MainMenu: public Visual, private SingleSelectionMenu{
public:
	class Listener{
	public:
		virtual void onSelect(std::string) = 0;
		virtual void onExit() = 0;
	};
	MainMenu(Listener* l = 0)
	{
		widget = new MenuWidget();

		char* f = hge->Resource_EnumFiles("scripts\\*.lua");
		while(f){
			widget->insertItem(f);
			scripts.push_back(
				hge->Resource_MakePath(
					(std::string("scripts\\") + f).c_str()
				)
			);
			f = hge->Resource_EnumFiles(0);
		}// insert
		widget->insertItem("Exit");
	
		// move to center
		hgeRect r = widget->getRect();
		float w = r.x2 - r.x1;
		float h = r.y2 - r.y1;
		widget->move(50, 500);

		this->setWidget(widget);

		listener = l;
	}
	~MainMenu(){
		delete widget;
	}
	void onChangeSelection(){
		int s = getSelection();

		// if changed:
		if(!listener)return;
		if(s == scripts.size())
			listener->onExit();
		else if(s >= 0 && s < scripts.size())
			listener->onSelect(scripts[s]);
	}
	void Render(){
		widget->Render();
	}
	void show(){widget->show();}
	void hide(){widget->hide();}
	void setListener(Listener* l){
		listener = l;
	}

private:
	MenuWidget* widget;
	Listener* listener;
	std::vector<std::string> scripts;
};

lua_State* L;
int screen_width;
int screen_height;

Tcl::interpreter interp;

class MenuListener: public MainMenu::Listener{
	virtual void onSelect(std::string script){
		if(luaL_dofile(L, script.c_str()) != 0)
			lua_error_handler(L);
			//std::cerr << lua_tostring(L, -1);
	}
	virtual void onExit(){
		need_exit = true;
	}
};

// Pointer to the HGE interface.
// Helper classes require this to work.
HGE *hge=0;
hgeResourceManager* res_manager;

CompositeVisual scene;

MainMenu* mm;
MenuListener menu_listener;
VideoPlayer* player;
MovableItem  *mover;
void* video; DWORD video_size;

// Some resource handles
HEFFECT				snd;
HTEXTURE			tex;
hgeQuad				quad;

// Pointers to the HGE objects we will use
hgeFont				*fnt;
hgeSprite			*spr;

bool FrameFunc()
{
	lua_gc(L, LUA_GCCOLLECT, 1);
//	lua_gc(L, LUA_GCSTEP, 100);
	lua_gc(L, LUA_GCSTOP, 1);

	float dt=hge->Timer_GetDelta();
	static float t=0.0f;
	float tx,ty;
	static int lastid=0;

	frame_timer->Update(dt);

	// Here we update our background animation
	//t+=dt;
	t = 0;
	tx=50*cosf(t/60);
	ty=50*sinf(t/60);

//	quad.v[0].tx=tx;        quad.v[0].ty=ty;
//	quad.v[1].tx=tx+800/64; quad.v[1].ty=ty;
//	quad.v[2].tx=tx+800/64; quad.v[2].ty=ty+600/64;
//	quad.v[3].tx=tx;        quad.v[3].ty=ty+600/64;
	
	lua_gc(L, LUA_GCRESTART, 1);
	return need_exit;
}


bool RenderFunc()
{
	lua_gc(L, LUA_GCSTOP, 1);
	// Render graphics
	hge->Gfx_BeginScene();
	hge->Gfx_RenderQuad(&quad);

	//render all layers
	int n = Layers::num_layers();
	for(int i=0; i<n; i++){
		Layers::get_layer(i)->Render();
	}

	scene.Render();

//	fnt->SetColor(0xFFFFFFFF);
//	fnt->printf(5, 5, HGETEXT_LEFT, "FPS:%d", hge->Timer_GetFPS());

	float mx, my;
	hge->Input_GetMousePos(&mx, &my);
	spr->Render(mx, my);

	hge->Gfx_EndScene();
	
	lua_gc(L, LUA_GCRESTART, 1);
	return false;
}

#ifndef _DEBUG
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#else
int main()
#endif
{

	switch_to_english();

	hge = hgeCreate(HGE_VERSION);
	hge->System_SetState(HGE_LOGFILE, "demo.log");
	hge->System_SetState(HGE_INIFILE, "demo.ini");
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_TITLE, "Dialog Quest Demo");
	hge->System_SetState(HGE_SCREENBPP, 32);

	//hge->System_SetState(HGE_ZBUFFER, true);
	hge->System_SetState(HGE_SHOWSPLASH, false);
//	hge->System_SetState(HGE_FPS, HGEFPS_VSYNC);
	hge->System_SetState(HGE_FPS, 60);

	screen_width = hge->Ini_GetInt("system", "SCREEN_WIDTH", 1680);
	screen_height = hge->Ini_GetInt("system", "SCREEN_HEIGHT", 1050);
	const int WINDOWED = hge->Ini_GetInt("system", "WINDOWED", 1);

	hge->System_SetState(HGE_WINDOWED, WINDOWED);
	hge->System_SetState(HGE_SCREENWIDTH, screen_width);
	hge->System_SetState(HGE_SCREENHEIGHT, screen_height);

	if(hge->System_Initiate())
	{
		/*
		if(goto_localdata_folder()){
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}
		*/
		res_manager = new hgeResourceManager("resources.txt");

		hge->Resource_AttachPack("demo.paq");
/*
		if(!download_resources())
		{
			// If one of the data files is not found, display
			// an error message and shutdown.
			hge->System_Log("Can't download video!");
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}
*/
		// Load sound and textures
		quad.tex=hge->Texture_Load(hge->Ini_GetString("system", "BACKGROUND", "bg_new.png"));
		tex=hge->Texture_Load("cursor.png");
		snd=hge->Effect_Load("menu.wav");
//		video=hge->Resource_Load((get_localdata_folder() + "\\I-15bis.ogg").c_str(), &video_size);
		
//		HTEXTURE circle;
//		circle = hge->Texture_Load("fire_glow_strip5.png");
//		hgeAnimation *anim = new hgeAnimation(circle, 4, 12, 0, 0, 32, 32); //circle, 9, 3, 0, 0, 32, 32);
//		anim->SetMode(HGEANIM_FWD | HGEANIM_LOOP);
//		anim->Play();
//		mover = new MovableItem(anim, 400, 400);
//		mover->setParent(&scene);

		/////////// Lua mover!/////////
		L = lua_open();
		luaL_openlibs(L);
		luabind::open(L);
		luabind::set_pcall_callback(&lua_error_handler);
		MovableItem::luabind(L);
		Lualib::luabind(L);
		
//		ScreenItem::setParentVisual(&scene);

		LuaWidget::luabind(L);
		LuaImageWidget::luabind(L);
		LuaImage::luabind(L);
		LuaText::luabind(L);
		LuaAnimation::luabind(L);
		LuaFrame::luabind(L);
		LuaTextWidget::luabind(L);
		LuaScreenItem::luabind(L);
		LuaSoundEffect::luabind(L);
		LuaTimer::luabind(L, "Timer");
		Layers::luabind(L);

		// Tcl
		interp.eval("for {set i 0} {$i != 1} {incr i} { puts [pwd] }");

//		TextWidget::setParentVisual(&scene);
//		TextWidget* t = new TextWidget("Hello\nthere", 360+100, 280, "courier20b");
//		t->setOverResponse(true);
//		t->setDragResponse(true);

		if(luaL_dofile(L, "lib.lua") != 0)
			lua_error_handler(L);
			//std::cerr << lua_tostring(L, -1);

		luaL_dostring(
			L,
			"IM = Image(\"fire_glow_strip5.png\", screen_width / 2, screen_height / 2)"
			);

/*		luaL_dostring(
		L,
		"m2 = MovableItem(0, 0)\n"
		"m2:move(10, 10)"
		);

		luaL_dostring(
		L,
		"function right(arg)\n"
		"  arg:move(1, 0)\n"
		"end\n"
		"function left(arg)\n"
		"  arg:move(-1, 0)\n"
		"end\n"
		"m2.callback=right\n"
		);

		MovableItem* m2 = luabind::object_cast<MovableItem*>(
			luabind::globals(L)["m2"]
		);
		m2->setParent(&scene);
*/
		//////////////////////////////

		if(!quad.tex || !tex || !snd)
		{
			// If one of the data files is not found, display
			// an error message and shutdown.
			MessageBox(NULL, "Can't load resource files. See demo.log for datails", "Error", MB_OK | MB_ICONERROR | MB_APPLMODAL);
			hge->System_Shutdown();
			hge->Release();
			return 0;
		}

		// Set up the quad we will use for background animation
		quad.blend=BLEND_ALPHABLEND | BLEND_COLORMUL | BLEND_NOZWRITE;

		for(int i=0;i<4;i++)
		{
			// Set up z-coordinate of vertices
			quad.v[i].z=0.5f;
			// Set up color. The format of DWORD col is 0xAARRGGBB
			quad.v[i].col=0xFFFFFFFF;
		}

		// original tex dimenstions
		float tw = hge->Texture_GetWidth(quad.tex, true);
		float th = hge->Texture_GetHeight(quad.tex, true);
		// k = orig / power_of_2
		float kw = tw / hge->Texture_GetWidth(quad.tex, false);
		float kh = th / hge->Texture_GetHeight(quad.tex, false);
		quad.v[0].x=0; quad.v[0].y=0; 
		quad.v[1].x=tw; quad.v[1].y=0; 
		quad.v[2].x=tw; quad.v[2].y=th; 
		quad.v[3].x=0; quad.v[3].y=th;

		quad.v[0].tx=0;    quad.v[0].ty=0;
		quad.v[1].tx=kw; quad.v[1].ty=0;
		quad.v[2].tx=kw; quad.v[2].ty=kh;
		quad.v[3].tx=0;    quad.v[3].ty=kh;

		// Load the font, create the cursor sprite
		fnt=new hgeFont("font1.fnt");
		spr=new hgeSprite(tex,0,0,32,32);

		mm = new MainMenu(&menu_listener);
		mm->setParent(&scene);
		mm->show();
//		player = new VideoPlayer();
//		player->setParent(&scene);
//		player->Open(video, video_size, hgeVector(40, 40), hgeVector(320, 240));

		// Let's rock now!
		hge->System_Start();

		/// Lua!!!
		lua_close(L);

//		delete mover;
//		delete anim;
//		delete player;
		delete mm;
		// Delete created objects and free loaded resources
		delete fnt;
		delete spr;
		hge->Resource_Free(video);
//		hge->Texture_Free(circle);
		hge->Effect_Free(snd);
		hge->Texture_Free(tex);
		hge->Texture_Free(quad.tex);
		hge->Resource_RemoveAllPacks();
	}

	// Clean up and shutdown
	hge->System_Shutdown();
	hge->Release();
	return 0;
}
