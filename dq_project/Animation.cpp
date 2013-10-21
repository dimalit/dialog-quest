#include "PlatformPrecomp.h"

#include "LuaScreenItem.h"

#include "Animation.h"

AnimatedItem::AnimatedItem(std::string tex, int frames_x, int frames_y, float tex_x, float tex_y, int nframes, float fps)
	:nframes(nframes), fps(fps)
{	
	mode_loop = 0;
	mode_reverse = 0;

	component = new OverlayRenderComponent();
	entity->AddComponent(component);

	component->GetVar("fileName")->Set(tex);

	VariantList params((uint32)frames_x, (uint32)frames_y);
	component->GetFunction("SetupAnim")->sig_function(&params);

	// this will be our "phase": 0..nframes
	component->GetVar("frame")->Set(0.0f);
	component->GetVar("frame")->GetSigOnChanged()->connect(1, boost::bind(&AnimatedItem::OnFrameChange, this, _1));

	ic = new InterpolateComponent();
	entity->AddComponent(ic);
	ic->GetVar("component_name")->Set(component->GetName());
	ic->GetVar("var_name")->Set("frame");
	ic->GetVar("interpolation")->Set(uint32(INTERPOLATE_LINEAR));
	ic->GetVar("target")->Set((float)nframes);

	// apply mode
	update_ic_target();
}

AnimatedItem::~AnimatedItem()
{
}

// compute frameX and frameY for renderer
void AnimatedItem::OnFrameChange(Variant* val){

	// do not use final value
	int frame = component->GetVar("frame")->GetFloat();
	bool finished = 0;
	if(frame==nframes){
		finished = 1;
		frame = nframes - 1;
	}
// TODO: Commented out - why was it here?!
//	if(GetVar("frame")->GetFloat() == 0.0f)
//		finished = 1;

	int frames_x =  component->GetVar("totalFramesX")->GetUINT32();
	component->GetVar("frameY")->Set((uint32)(frame / frames_x));
	component->GetVar("frameX")->Set((uint32)(frame % frames_x));

	if(finished)
		onFinish();
}

LuaAnimatedItem::LuaAnimatedItem(luabind::object conf):
	AnimatedItem(
		luabind::object_cast<std::string>(conf["file"]),
		luabind::object_cast<int>(conf["frames_x"]),
		luabind::object_cast<int>(conf["frames_y"]),
		luabind::object_cast<float>(conf["tex_x"]),
		luabind::object_cast<float>(conf["tex_y"]),
		luabind::object_cast<int>(conf["nframes"]),
		luabind::object_cast<float>(conf["fps"])
	)
{
}

LuaAnimatedItem::~LuaAnimatedItem(){
}

void LuaAnimatedItem::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaAnimatedItem, LuaScreenItem>("AnimatedItem")
			.def(luabind::constructor<luabind::object>())
			.property("scaleX", &LuaAnimatedItem::getScaleX, &LuaAnimatedItem::setScaleX)
			.property("scaleY", &LuaAnimatedItem::getScaleY, &LuaAnimatedItem::setScaleY)
			.property("frame", &LuaAnimatedItem::getFrame, &LuaAnimatedItem::setFrame)
			.property("num_frames", &LuaAnimatedItem::getNumFrames)
			.property("loop", &LuaAnimatedItem::getLoop, &LuaAnimatedItem::setLoop)
			.property("reverse", &LuaAnimatedItem::getReverse, &LuaAnimatedItem::setReverse)
			.def("play", &LuaAnimatedItem::play)
			.def("stop", &LuaAnimatedItem::stop)
			.def_readwrite("onFinish", &LuaAnimatedItem::onFinish_cb)
	];
}
