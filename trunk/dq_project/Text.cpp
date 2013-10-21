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
	component->GetVar("textAlignment")->Set((uint32)align);
	component->GetVar("firstLineDecrement")->Set(50.0f);
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

LuaTextBoxItem::LuaTextBoxItem(std::string txt, float width, eAlignment align)
	:TextBoxItem(txt, width, align){}
LuaTextBoxItem::LuaTextBoxItem(std::string txt, float width, eAlignment align, eFont font)
	:TextBoxItem(txt, width, align, font){}

void LuaTextBoxItem::luabind(lua_State* L){

	// bind also StairsProfile
	luabind::module(L) [
		luabind::class_<LuaStairsProfile>("StairsProfile")
			.def(luabind::constructor<>())
			.def("at", &LuaStairsProfile::at)
			.def("add", &LuaStairsProfile::add)
			.def("setInterval", &LuaStairsProfile::setInterval)
			.def("shifted", &LuaStairsProfile::shifted)
	];

	luabind::module(L) [
		luabind::class_<LuaTextBoxItem, LuaScreenItem>("TextBoxItem")
			// TODO how to bind alignment constants to Lua?
			.def(luabind::constructor<std::string, float, eAlignment>())
			.def(luabind::constructor<std::string, float, eAlignment, eFont>())
			.property("text", &LuaTextBoxItem::getText, &LuaTextBoxItem::setText)
			.property("font", &LuaTextBoxItem::getFont, &LuaTextBoxItem::setFont)
			.property("scale", &LuaTextBoxItem::getScale, &LuaTextBoxItem::setScale)
			.property("firstLineDecrement", &LuaTextBoxItem::getFirstLineDecrement, &LuaTextBoxItem::setFirstLineDecrement)
			.property("lastLineEndX", &LuaTextBoxItem::getLastLineEndX)
			.property("lastLineEndY", &LuaTextBoxItem::getLastLineEndY)
			.property("leftObstacles", &LuaTextBoxItem::getLeftObstacles, &LuaTextBoxItem::setLeftObstacles)
			.property("rightObstacles", &LuaTextBoxItem::getRightObstacles, &LuaTextBoxItem::setRightObstacles)
			//.def("getLeftObstacle", &LuaTextBoxItem::getLeftObstacle)
			//.def("getRightObstacle", &LuaTextBoxItem::getRightObstacle)
			//.def("setLeftObstacle", &LuaTextBoxItem::setLeftObstacle)
			//.def("setRightObstacle", &LuaTextBoxItem::setRightObstacle)
	];
}