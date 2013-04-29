#include <cmath>
#include "main.h"
#include "Frame.h"

Frame::Frame(float w, float h)
{
	width = w;
	height = h;
}

Frame::~Frame()
{
}

void Frame::Render(float x, float y, float rot){

	float x1, y1, x2, y2, x3, y3, x4, y4;

	x1 = x;
	y1 = y;

	x2 =   getWidth()*cos(rot) + x;
	y2 = - getWidth()*sin(rot) + y;

	x3 =   getWidth()*cos(rot) + getHeight()*sin(rot) + x;
	y3 = - getWidth()*sin(rot) + getHeight()*cos(rot) + y;

	x4 = getHeight()*sin(rot) + x;
	y4 = getHeight()*cos(rot) + y;

	unsigned long c = 0xFFFFFF00;		// yellow
	hge->Gfx_RenderLine(x1, y1, x2, y2, c);
	hge->Gfx_RenderLine(x2, y2, x3, y3, c);
	hge->Gfx_RenderLine(x3, y3, x4, y4, c);
	hge->Gfx_RenderLine(x4, y4, x1, y1, c);
}

void LuaFrame::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaFrame, ScreenResource>("Frame")
			.property("width", &LuaFrame::getWidth, &LuaFrame::setWidth)
			.property("height", &LuaFrame::getHeight, &LuaFrame::setHeight)
			.def(luabind::constructor<float, float>())
	];
}