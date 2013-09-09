#include "PlatformPrecomp.h"
#include "ImageTestMenu.h"
#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "ScreenItem.h"
#include "Image.h"
#include "Text.h"
#include "Animation.h"
#include "Texture.h"

void ImageTestOnSelect(VariantList *pVList) //0=vec2 point of click, 1=entity sent from
{
	Entity *pEntClicked = pVList->m_variant[1].GetEntity();

	LogMsg("Clicked %s entity at %s", pEntClicked->GetName().c_str(),pVList->m_variant[1].Print().c_str());

	if (pEntClicked->GetName() == "Back")
	{
//		SlideScreen(pEntClicked->GetParent(), false);
		GetMessageManager()->CallEntityFunction(pEntClicked->GetParent(), 0, "OnDelete", NULL);
		MainMenuCreate(pEntClicked->GetParent()->GetParent());
	}
}

Entity * ImageTestMenuCreate(Entity *pParentEnt)
{
	Entity *pBG = 0;//CreateOverlayEntity(pParentEnt, "TouchTest", "interface/summary_bg.rttex", 0,0);
//	AddFocusIfNeeded(pBG);

//	Entity *pTouchTestEnt = pBG->AddEntity(new Entity(new TouchTestComponent));

//	CompositeVisual* root = new CompositeVisual();
//	root->acquireEntity(pBG);
//	ScreenItem::setParentVisual(root);

	//luaL_dostring(
	//L,
	//"p = Image(\"interface/test.bmp\")\n"
	//"si = Mover(80,80,p)\n"
	//);

	//luaL_dostring(
	//L,
	//"t = TextItem(screen_width / 2, screen_height / 2, \"Hello from Lua!\")\n"
	//);

	//luaL_dostring(
	//L,
	//"p = Text(\"Hello from Lua!\")\n"
	//"si = ScreenItem(100, 100)\n"
	//"si.view = p\n"
	//"si.hpx = p.width\n"
	//"si.hpy=0\n"
	//);

	//ScreenItem* si;
	//TextBox *p = new TextBox("Programmed!", 50, 50, ALIGNMENT_UPPER_LEFT);
	//si = new ScreenItem(300,300);
	//si->setView(p);
	//Layers::root_visual()->entity->PrintTreeAsText();

	//luaL_dostring(
	//L,
	//"a = AnimatedItem(0, 0, \"explosion.anim\")\n"
	//);

//	luaL_dostring(
//	L,
////	"a = Animation{file = \"interface/explosion.jpg\", width = 32, height = 32, tex_x = 0, tex_y = 0, nframes = 7, fps = 16}\n"
//"a = Animation(load_config(\"DropArea.anim\"))\n"
//	"si = ScreenItem(300, 300)\n"
//	"si.view = a\n"
//	"a.loop = true\n"
//	"a:play()\n"
//	);

	//Animation *a = new Animation("interface/test.bmp", 32, 32, 0, 0, 16, 16);
	//si = new ScreenItem(200, 200);
	//si->setView(a);
	//a->setLoop(1);
	//a->play();

	//if(luaL_dostring(
	//L,
	//"lt_si = SimpleItem(root, 150,12)\n"
	//"lt_si.view = TextBox(\"Long long text\", 300, 300, 0)\n"
	//"print(lt_si.height)\n"
	//)!=0){
	//	LogMsg( "%s\n", lua_tostring(L,-1) );
	//	return 0;
	//};

	if(luaL_dostring(
	L,
	"s = SoundEffect(\"audio/click.wav\")\n"
	"s:play()\n"
	)!=0){
		LogMsg( "%s\n", lua_tostring(L,-1) );
		return 0;
	};

	//SimpleItem* si;
	//Texture *t = new Texture("interface/flask.rttex", 300, 300);
	//si = new SimpleItem(root_item(), 150,150);
	//si->setView(t);

	//if(luaL_dostring(
	//L,
	//"local t = Texture(\"interface/flask.rttex\", 300, 300)\n"
	//"local si = SimpleItem(150, 150)\n"
	//"si.view = t\n"
	//"root:add(si)\n"
	//)!=0){
	//	LogMsg( "%s\n", lua_tostring(L,-1) );
	//	return 0;
	//};

	lua_pushcfunction(L, lua_error_handler);		// don't move it below: order matters
	if(luaL_loadfile(L, "init.lua")!=0){
		std::cout << lua_tostring(L,-1) << "\n";
		return 0;
	}
	lua_pcall(L, 0, LUA_MULTRET, -2);

//	luaL_dostring(
//	L,
//	"t = Timer(function(timer) print(\"Timer!\") end, 2)\n"
//	"d1 = DropArea(100, 100, TwoStateAnimation(Animation(load_config(\"DropArea.anim\"))))\n"
//	"d2 = DropArea(200, 100, TwoStateAnimation(Animation(load_config(\"DropArea.anim\"))))\n"
//	"d3 = DropArea(300, 100, TwoStateAnimation(Animation(load_config(\"DropArea.anim\"))))\n"
//	"d4 = DropArea(400, 100, TwoStateAnimation(Animation(load_config(\"DropArea.anim\"))))\n"
//	);

	//luaL_dostring(
	//L,
	//"b = Button(100, 100, TwoStateAnimation(Animation(load_config(\"Start.anim\"))))\n"
	//"d = DropArea(200, 200, TwoStateAnimation(Animation(load_config(\"DropArea.anim\"))))"
	//);

	//pBG = Layers::get_layer(0)->entity;
	//Entity *pButtonEntity;
	//pButtonEntity = CreateTextButtonEntity(pBG, "Back", 240, 290, "Back"); 
	//pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->sig_function.connect(&ImageTestOnSelect);
	//pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
	//AddHotKeyToButton(pButtonEntity, VIRTUAL_KEY_BACK); //for android's back button, or escape key in windows
	return pBG;
}

