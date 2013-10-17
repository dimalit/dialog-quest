#include "PlatformPrecomp.h"
#include "Animation.h"

Animation::Animation(std::string tex, int frames_x, int frames_y, float tex_x, float tex_y, float nframes, float fps)
:tex_file(tex), frames_x(frames_x), frames_y(frames_y), nframes(nframes), fps(fps)
{	
	mode_loop = 0;
	mode_reverse = 0;
}

Animation::~Animation()
{
}

void Animation::OnAdd(Entity* e){
	OverlayRenderComponent::OnAdd(e);
	// HACK components need all parameters AFTER attach...
	GetVar("fileName")->Set(tex_file);

	VariantList params((uint32)frames_x, (uint32)frames_y);
	GetFunction("SetupAnim")->sig_function(&params);

//	GetParent()->GetVar("size2d")->Set(frame_box.x, frame_box.y);
//	GetParent()->GetVar("pos2d")->Set(frame_box.x/2, frame_box.y/2);

	// this will be our "phase": 0..nframes
	GetVar("frame")->Set(0.0f);
	GetVar("frame")->GetSigOnChanged()->connect(1, boost::bind(&Animation::OnFrameChange, this, _1));

	ic = new InterpolateComponent();
	e->AddComponent(ic);
	ic->GetVar("component_name")->Set(this->GetName());
	ic->GetVar("var_name")->Set("frame");
	ic->GetVar("interpolation")->Set(uint32(INTERPOLATE_LINEAR));
	ic->GetVar("target")->Set((float)nframes);

	// apply mode
	update_ic_target();
}

void Animation::OnRemove(){
	GetParent()->RemoveComponentByAddress(ic);
}

// compute frameX and frameY for renderer
void Animation::OnFrameChange(Variant* val){

	// do not use final value
	int frame = GetVar("frame")->GetFloat();
	bool finished = 0;
	if(frame==nframes){
		finished = 1;
		frame = nframes - 1;
	}
// TODO: Commented out - why was it here?!
//	if(GetVar("frame")->GetFloat() == 0.0f)
//		finished = 1;

	GetVar("frameY")->Set((uint32)(frame / frames_x));
	GetVar("frameX")->Set((uint32)(frame % frames_x));

	if(finished)
		onFinish();
}

LuaAnimation::LuaAnimation(luabind::object conf):
	Animation(
		luabind::object_cast<std::string>(conf["file"]),
		luabind::object_cast<float>(conf["frames_x"]),
		luabind::object_cast<float>(conf["frames_y"]),
		luabind::object_cast<float>(conf["tex_x"]),
		luabind::object_cast<float>(conf["tex_y"]),
		luabind::object_cast<float>(conf["nframes"]),
		luabind::object_cast<float>(conf["fps"])
	)
{
}

LuaAnimation::~LuaAnimation(){
}

void LuaAnimation::luabind(lua_State* L){
	luabind::module(L) [
		luabind::class_<LuaAnimation, EntityComponent>("Animation")
			.def(luabind::constructor<luabind::object>())
			.property("width", &LuaAnimation::getWidth)
			.property("height", &LuaAnimation::getHeight)
			.property("scaleX", &LuaAnimation::getScaleX, &LuaAnimation::setScaleX)
			.property("scaleY", &LuaAnimation::getScaleY, &LuaAnimation::setScaleY)
			.property("frame", &LuaAnimation::getFrame, &LuaAnimation::setFrame)
			.property("num_frames", &LuaAnimation::getNumFrames)
			.property("loop", &LuaAnimation::getLoop, &LuaAnimation::setLoop)
			.property("reverse", &LuaAnimation::getReverse, &LuaAnimation::setReverse)
			.def("play", &LuaAnimation::play)
			.def("stop", &LuaAnimation::stop)
			.def_readwrite("onFinish", &LuaAnimation::onFinish_cb)
	];
}