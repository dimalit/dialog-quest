#pragma once

#include <string>
#include <cassert>
#include <luabind/luabind.hpp>

class LuaImageWidget: public ImageWidget, public LuaWidget
{
public:
	LuaWidget(luabind::object conf, float x = 0, float y = 0):W(x, y){
		std::string names[] = {"normal", "over", "checked"};
		view_type types[] = {VIEW_NORMAL, VIEW_OVER, VIEW_CHECKED};

		for(int i=0; i<3; i++){
			luabind::object o = conf[names[i].c_str()];
			if(o){
				std::string file = luabind::object_cast<std::string>(o["file"]);
				float w = luabind::object_cast<float>(o["width"]);
				float h = luabind::object_cast<float>(o["height"]);
				float tex_x = luabind::object_cast<float>(o["tex_x"]);
				float tex_y = luabind::object_cast<float>(o["tex_y"]);
				// optional
				float nframes = 1;
				float fps = 1;
				try{
					nframes = luabind::object_cast<float>(o["nframes"]);
					fps = luabind::object_cast<float>(o["fps"]);
				}catch(...){}
				setAnim(types[i], file, w, h, tex_x, tex_y, nframes, fps);

				// set rect for mouse
				if(i==0)
					this->rect = hgeRect(x - w/2, y - h/2, x + w/2, y + h/2);
			}// if
		}// for
		update_view();
	}
	static void luabind(lua_State* L, const char* classname);

	luabind::object getRect(){
		luabind::object t = luabind::newtable(L);
		t["x1"] = rect.x1;
		t["y1"] = rect.y1;
		t["x2"] = rect.x2;
		t["y2"] = rect.y2;

		return t;
	}
private:
	LuaWidget(const LuaWidget& w){assert(false);}
	LuaWidget& operator=(const LuaWidget& w){assert(false);}
	static lua_State* L;

	luabind::object onDrag_cb;
	luabind::object onClick_cb;
	luabind::object onDbClick_cb;
	luabind::object onDragStart_cb;
	luabind::object onDragEnd_cb;

	virtual void onDrag(float dx, float dy){
		W::onDrag(dx, dy);
		if(onDrag_cb)
			luabind::call_function<void>(onDrag_cb, this);
	}
	virtual void onClick(){
		W::onClick();
		if(onClick_cb)
			luabind::call_function<void>(onClick_cb, this);
	}
	virtual void onDbClick(){
		W::onDbClick();
		if(onDbClick_cb)
			luabind::call_function<void>(onDbClick_cb, this);
	}
	virtual void onDragStart(float x, float y){
		W::onDragStart(x, y);
		if(onDragStart_cb)
			luabind::call_function<void>(onDragStart_cb, this);
	}
	virtual void onDragEnd(float x, float y){
		W::onDragEnd(x, y);
		if(onDragEnd_cb)
			luabind::call_function<void>(onDragEnd_cb, this);
	}
};

template <class W>
void LuaWidget<W>::luabind(lua_State* L, const char* classname){
	LuaWidget<W>::L = L;

	luabind::module(L) [
	luabind::class_<LuaWidget>(classname)
	.def(luabind::constructor<luabind::object, float, float>())
	.property("x", &LuaWidget::getX)
	.property("y", &LuaWidget::getY)
	.def("move", &LuaWidget::move)
	.def("moveTo", &LuaWidget::moveTo)
	.property("rect", &LuaWidget::getRect)
	.property("visible", &LuaWidget::getVisible, &LuaWidget::setVisible)
	.property("enabled", &LuaWidget::getEnabled, &LuaWidget::setEnabled)
	.property("checked", &LuaWidget::getChecked, &LuaWidget::setChecked)
	.property("over_response", &LuaWidget::getOverResponse, &LuaWidget::setOverResponse)
	.property("drag_response", &LuaWidget::getDragResponse, &LuaWidget::setDragResponse)
	.property("click_response", &LuaWidget::getClickResponse, &LuaWidget::setClickResponse)
	.def_readwrite("onClick", &LuaWidget::onClick_cb)
	.def_readwrite("onDbClick", &LuaWidget::onDbClick_cb)
	.def_readwrite("onDrag", &LuaWidget::onDrag_cb)
	.def_readwrite("onDragStart", &LuaWidget::onDragStart_cb)
	.def_readwrite("onDragEnd", &LuaWidget::onDragEnd_cb)
	];	
}
