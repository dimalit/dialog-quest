#include "PlatformPrecomp.h"
#include "Animation.h"

Animation::Animation(std::string file, std::string anim, float fps)
	:file_name(file), anim_name(anim), fps(fps), ic(NULL), mode_loop(0), mode_reverse(0)
{
}

Animation::~Animation()
{
}

void Animation::OnAdd(Entity* e){
	SpriteAnimationRenderComponent::OnAdd(e);
	// HACK: components need all parameters AFTER attach...
	GetVar("fileName")->Set(file_name);
	GetVar("animationName")->Set(anim_name);

	ic = e->addComponent(new InterpolateComponent());
	ic->GetVar("component_name")->Set(this->GetName());
	ic->GetVar("var_name")->Set("phase");
	ic->GetVar("interpolation")->Set(uint32(INTERPOLATE_LINEAR));
	ic->GetVar("target")->Set(1.0f);
}

void Animation::OnRemove(){
	GetParent()->RemoveComponentByAddress(ic);
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