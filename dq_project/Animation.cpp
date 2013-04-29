#include <hgeanim.h>
#include "main.h"
#include "Animation.h"

Animation::Animation(std::string tex, float w, float h, float tex_x, float tex_y, float nframes, float fps)
{
	this->tex = hge->Texture_Load(tex.c_str());
	if(!this->tex){ani = 0; return;}
	ani = new hgeAnimation(this->tex, nframes, fps, tex_x, tex_y, w, h);
	need_delete = (bool)ani;	// true or false
}

// get copy!
Animation::Animation(hgeAnimation* a){
	tex = 0;
	ani = new hgeAnimation(*a);
	need_delete = true;
}

Animation::~Animation()
{
	if(ani && need_delete)delete ani;
	if(tex)hge->Texture_Free(tex);
}

void Animation::Render(float x, float y, float rot){
	if(ani)
		ani->RenderEx(x, y, rot);
}

void Animation::Update(float dt){
//	std::cout << t << "\n";
	if(ani){
		int f1  = ani->GetFrame();
		bool p1 = ani->IsPlaying();
		ani->Update(dt);
		int f2 = ani->GetFrame();
		bool p2 = ani->IsPlaying();
//		if(f1 != f2 || p1 != p2)
//		std::cout << f1 << " -> " << f2 << " === " << p1 << " -> " << p2 << "\n";
		// handle finish
		// (if over/under flow)
		if(		p1!=p2
				||
				(
					(f1!=f2)
					&&
					f2 != ((getReverse()) ? f1 - 1 : f1 + 1)
				)
				)
		{
//			std::cout << "onFinish()\n";
			onFinish();
		}
	}
}

LuaAnimation::LuaAnimation(luabind::object conf):
	Animation(
		luabind::object_cast<std::string>(conf["file"]),
		luabind::object_cast<float>(conf["width"]),
		luabind::object_cast<float>(conf["height"]),
		luabind::object_cast<float>(conf["tex_x"]),
		luabind::object_cast<float>(conf["tex_y"]),
		luabind::object_cast<float>(conf["nframes"]),
		luabind::object_cast<float>(conf["fps"])
	)
{
}
LuaAnimation::LuaAnimation(std::string name):
	Animation(res_manager->GetAnimation(name.c_str()))
{
}

void LuaAnimation::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaAnimation, ScreenResource>("Animation")
			.def(luabind::constructor<luabind::object>())
			.def(luabind::constructor<std::string>())
			.property("frame", &LuaAnimation::getFrame, &LuaAnimation::setFrame)
			.property("num_frames", &LuaAnimation::getNumFrames)
			.property("loop", &LuaAnimation::getLoop, &LuaAnimation::setLoop)
			.property("reverse", &LuaAnimation::getReverse, &LuaAnimation::setReverse)
			.def("play", &LuaAnimation::play)
			.def("pause", &LuaAnimation::pause)
			.def_readwrite("onFinish", &LuaAnimation::onFinish_cb)
	];
}