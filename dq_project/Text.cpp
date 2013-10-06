#include "PlatformPrecomp.h"
#include "Text.h"

Text::Text(std::string txt, eFont font)
{
	GetVar("text")->Set(txt);
	setFont(font);
}

Text::~Text()
{
}

void Text::OnAdd(Entity* e){
	TextRenderComponent::OnAdd(e);
	// HACK: components need all parameters AFTER attach...
	std::string t = GetVar("text")->GetString();
	GetVar("text")->Set(t);
}

// TODO: think about enums in Lua and here (don't want to use Proton's enums!)
TextBox::TextBox(std::string txt, int width, eAlignment align, eFont font)
{
	GetVar("text")->Set(txt);
	GetVar("textAlignment")->Set((uint32)align);
	GetVar("firstLineDecrement")->Set((uint32)50);
	this->width = width;
	setFont(font);
}

TextBox::~TextBox()
{
}

void TextBox::OnAdd(Entity* e){
	TextBoxRenderComponent::OnAdd(e);
	// TODO MUST first set size then text. Very bad!
	e->GetVar("size2d")->Set(width, 0);
	std::string t = GetVar("text")->GetString();
	GetVar("text")->Set(t);
}

LuaText::LuaText(std::string text):Text(text){}
LuaText::LuaText(std::string text, eFont font):Text(text, font){}

void LuaText::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaText, EntityComponent>("Text")
			.def(luabind::constructor<std::string>())
			.def(luabind::constructor<std::string, eFont>())
			.property("width", &LuaText::getWidth)
			.property("height", &LuaText::getHeight)
			.property("text", &LuaText::getText, &LuaText::setText)
			.property("font", &LuaText::getFont, &LuaText::setFont)
	];	
}

class LuaStairsProfile: public StairsProfile{
public:
	LuaStairsProfile(){}
	LuaStairsProfile(const StairsProfile& p)
		:StairsProfile(p){}

	int at(int x1, int w) const{
		return (*this)(x1, w);
	}

	void add(int w){
		(*this) += w;
	}

	LuaStairsProfile shifted(int dx) const{
		return LuaStairsProfile(StairsProfile::shifted(dx));
	}
};

const LuaStairsProfile LuaTextBox::getLeftObstacles() const{
	return LuaStairsProfile(left_obstacles);
}
const LuaStairsProfile LuaTextBox::getRightObstacles() const{
	return LuaStairsProfile(right_obstacles);
}
void LuaTextBox::setLeftObstacles(const LuaStairsProfile& p){
	left_obstacles = p;
	GetVar("text")->GetSigOnChanged()->operator()(NULL);
}
void LuaTextBox::setRightObstacles(const LuaStairsProfile& p){
	right_obstacles = p;
	GetVar("text")->GetSigOnChanged()->operator()(NULL);
}

LuaTextBox::LuaTextBox(std::string txt, int width, eAlignment align)
	:TextBox(txt, width, align){}
LuaTextBox::LuaTextBox(std::string txt, int width, eAlignment align, eFont font)
	:TextBox(txt, width, align, font){}

void LuaTextBox::luabind(lua_State* L){

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
		luabind::class_<LuaTextBox, EntityComponent>("TextBox")
			// TODO how to bind alignment constants to Lua?
			.def(luabind::constructor<std::string, int, eAlignment>())
			.def(luabind::constructor<std::string, int, eAlignment, eFont>())
			.property("width", &LuaTextBox::getWidth, &LuaTextBox::setWidth)
			.property("height", &LuaTextBox::getHeight)
			.property("text", &LuaTextBox::getText, &LuaTextBox::setText)
			.property("font", &LuaTextBox::getFont, &LuaTextBox::setFont)
			.property("firstLineDecrement", &LuaTextBox::getFirstLineDecrement, &LuaTextBox::setFirstLineDecrement)
			.property("lastLineEndX", &LuaTextBox::getLastLineEndX)
			.property("lastLineEndY", &LuaTextBox::getLastLineEndY)
			.property("leftObstacles", &LuaTextBox::getLeftObstacles, &LuaTextBox::setLeftObstacles)
			.property("rightObstacles", &LuaTextBox::getRightObstacles, &LuaTextBox::setRightObstacles)
			//.def("getLeftObstacle", &LuaTextBox::getLeftObstacle)
			//.def("getRightObstacle", &LuaTextBox::getRightObstacle)
			//.def("setLeftObstacle", &LuaTextBox::setLeftObstacle)
			//.def("setRightObstacle", &LuaTextBox::setRightObstacle)
	];
}