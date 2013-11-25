#include "PlatformPrecomp.h"
#include "Text.h"

TextItem::TextItem(std::string txt, eFont font)
{
	component = new TextRenderComponent();
	entity->AddComponent(component);

	component->GetVar("text")->Set(txt);
	setFont(font);
}

TextItem::~TextItem()
{
}

// TODO: think about enums in Lua and here (don't want to use Proton's enums!)
TextBoxItem::TextBoxItem(std::string txt, float width, eAlignment align, eFont font)
{
	component = new TextBoxRenderComponent();
	entity->AddComponent(component);

	entity->GetVar("size2d")->Set(width, 0);
	component->GetVar("text")->Set(txt);
	CL_Vec2f h = entity->GetVar("size2d")->GetVector2();
	component->GetVar("textAlignment")->Set((uint32)align);
	component->GetVar("firstLineDecrement")->Set(0.0f);
	setFont(font);
}

TextBoxItem::~TextBoxItem()
{
}

LuaTextItem::LuaTextItem(std::string text):TextItem(text){}
LuaTextItem::LuaTextItem(std::string text, eFont font):TextItem(text, font){}

void LuaTextItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaTextItem, LuaScreenItem>("TextItem")
			.enum_("eFont")
			[
				luabind::value("FONT_SMALL", FONT_SMALL),
				luabind::value("FONT_LARGE", FONT_LARGE),
				luabind::value("FONT_PHONETIC", FONT_PHONETIC),
				luabind::value("FONT_TIMES_14", FONT_TIMES_14)				
			]
			.def(luabind::constructor<std::string>())
			.def(luabind::constructor<std::string, eFont>())
			.property("text", &LuaTextItem::getText, &LuaTextItem::setText)
			.property("font", &LuaTextItem::getFont, &LuaTextItem::setFont)
			.property("scale", &LuaTextItem::getScale, &LuaTextItem::setScale)
	];	
}

class LuaStairsProfile: public StairsProfile{
public:
	LuaStairsProfile(){}
	LuaStairsProfile(const StairsProfile& p)
		:StairsProfile(p){}

	int at(float x1, float w) const{
		return (*this)(x1, w);
	}

	void add(float w){
		(*this) += w;
	}

	LuaStairsProfile shifted(float dx) const{
		return LuaStairsProfile(StairsProfile::shifted(dx));
	}
};

const LuaStairsProfile LuaTextBoxItem::getLeftObstacles() const{
	return LuaStairsProfile(TextBoxItem::getLeftObstacles());
}
const LuaStairsProfile LuaTextBoxItem::getRightObstacles() const{
	return LuaStairsProfile(TextBoxItem::getRightObstacles());
}
void LuaTextBoxItem::setLeftObstacles(const LuaStairsProfile& p){
	TextBoxItem::setLeftObstacles(p);
}
void LuaTextBoxItem::setRightObstacles(const LuaStairsProfile& p){
	TextBoxItem::setRightObstacles(p);
}

LuaTextBoxItem::LuaTextBoxItem(std::string txt)
	:TextBoxItem(txt, 0, ALIGNMENT_UPPER_LEFT){}
LuaTextBoxItem::LuaTextBoxItem(std::string txt, float width)
	:TextBoxItem(txt, width, ALIGNMENT_UPPER_LEFT){}
LuaTextBoxItem::LuaTextBoxItem(std::string txt, float width, eFont font)
	:TextBoxItem(txt, width, ALIGNMENT_UPPER_LEFT, font){}

void LuaTextBoxItem::luabind(lua_State* L){

	// bind also StairsProfile
	luabind::module(L) [
		luabind::class_<LuaStairsProfile>("StairsProfile")
			.def(luabind::constructor<>())
			.def("at", &LuaStairsProfile::at)
			.def("add", &LuaStairsProfile::add)
			.def("setInterval", &LuaStairsProfile::setInterval)
			.def("clear", &LuaStairsProfile::clear)
			.def("shifted", &LuaStairsProfile::shifted)
	];

	luabind::module(L) [
		luabind::class_<LuaTextBoxItem, LuaScreenItem>("TextBoxItem")
			.enum_("eFont")
			[
				luabind::value("FONT_SMALL", FONT_SMALL),
				luabind::value("FONT_LARGE", FONT_LARGE),
				luabind::value("FONT_PHONETIC", FONT_PHONETIC),
				luabind::value("FONT_TIMES_14", FONT_TIMES_14)
			]
			.def(luabind::constructor<std::string>())
			.def(luabind::constructor<std::string, float>())
			.def(luabind::constructor<std::string, float, eFont>())
			.property("text", &LuaTextBoxItem::getText, &LuaTextBoxItem::setText)
			.property("font", &LuaTextBoxItem::getFont, &LuaTextBoxItem::setFont)
			.property("scale", &LuaTextBoxItem::getScale, &LuaTextBoxItem::setScale)
			.property("firstLineDecrement", &LuaTextBoxItem::getFirstLineDecrement, &LuaTextBoxItem::setFirstLineDecrement)
			.property("lastLineEndX", &LuaTextBoxItem::getLastLineEndX)
			.property("lastLineEndY", &LuaTextBoxItem::getLastLineEndY)
			.property("leftObstacles", &LuaTextBoxItem::getLeftObstacles, &LuaTextBoxItem::setLeftObstacles)
			.property("rightObstacles", &LuaTextBoxItem::getRightObstacles, &LuaTextBoxItem::setRightObstacles)
			.property("oneLineWidth", &LuaTextBoxItem::getOneLineWidth)
			//.def("getLeftObstacle", &LuaTextBoxItem::getLeftObstacle)
			//.def("getRightObstacle", &LuaTextBoxItem::getRightObstacle)
			//.def("setLeftObstacle", &LuaTextBoxItem::setLeftObstacle)
			//.def("setRightObstacle", &LuaTextBoxItem::setRightObstacle)
	];
}