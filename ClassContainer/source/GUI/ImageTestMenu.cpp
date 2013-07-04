#include "PlatformPrecomp.h"
#include "ImageTestMenu.h"
#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "ScreenItem.h"
#include "Image.h"
#include "Text.h"
#include "Animation.h"
#include "lua_layers.h"

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
	//Text *p = new Text("Hello!");
	//si = new ScreenItem(100,100);
	//si->setView(p);

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

	luaL_dostring(
	L,
	"local lt_si = ScreenItem(100,100)\n"
	"lt_si.view = TextBox(\"Long long text\", 300, 300, 0)\n"
	);

	if(luaL_dofile(L, "test_mosaic.lua") != 0){
		LogMsg( "%s\n", lua_tostring(L,-1) );
		return false;
	}

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

	pBG = Layers::get_layer(0)->entity;
	Entity *pButtonEntity;
	pButtonEntity = CreateTextButtonEntity(pBG, "Back", 240, 290, "Back"); 
	pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->sig_function.connect(&ImageTestOnSelect);
	pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
	AddHotKeyToButton(pButtonEntity, VIRTUAL_KEY_BACK); //for android's back button, or escape key in windows
	return pBG;
}

