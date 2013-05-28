#include "PlatformPrecomp.h"
#include "ImageTestMenu.h"
#include "Entity/EntityUtils.h"
#include "MainMenu.h"
#include "ScreenItem.h"
#include "Image.h"

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

	luaL_dostring(
	L,
	"p = Image(\"interface/test.bmp\")\n"
//	"si = ScreenItem(5, 5)\n"
	"si = Mover(0,0,p)\n"
//	"si:move(100,25)\n"
//	"si.view = p\n"
//	"si.rotation = 1\n"
	);

/*	ScreenItem* si;
	Image *p = new Image("interface/test.bmp");
	si = new ScreenItem;
	si->setView(p);
*/

	//Entity *pButtonEntity;
	//pButtonEntity = CreateTextButtonEntity(pBG, "Back", 240, 290, "Back"); 
	//pButtonEntity->GetShared()->GetFunction("OnButtonSelected")->sig_function.connect(&ImageTestOnSelect);
	//pButtonEntity->GetVar("alignment")->Set(uint32(ALIGNMENT_CENTER));
	//AddHotKeyToButton(pButtonEntity, VIRTUAL_KEY_BACK); //for android's back button, or escape key in windows
	return pBG;
}

