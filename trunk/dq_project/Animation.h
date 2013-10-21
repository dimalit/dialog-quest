#pragma once
#include "ScreenItem.h"
#include "Entity/OverlayRenderComponent.h"
#include "Entity/InterpolateComponent.h"
#include <luabind/luabind.hpp>

class AnimatedItem: virtual public ScreenItem
{
public:
	AnimatedItem(std::string tex, int frames_x, int frames_y, float tex_x, float tex_y, int nframes, float fps);
	~AnimatedItem();

	void OnFrameChange(Variant* val);

	// TODO Make everything const.
	void setScaleX(float sx){
		assert(getParent());
		CL_Vec2f scale = entity->GetVar("scale2d")->GetVector2();
		scale.x = sx;
		entity->GetVar("scale2d")->Set(scale);
	}
	void setScaleY(float sy){
		assert(getParent());
		CL_Vec2f scale = entity->GetVar("scale2d")->GetVector2();
		scale.y = sy;
		entity->GetVar("scale2d")->Set(scale);
	}
	float getScaleX(){
		assert(getParent());
		return entity->GetVar("scale2d")->GetVector2().x;
	}
	float getScaleY(){
		assert(getParent());
		return entity->GetVar("scale2d")->GetVector2().y;
	}

	int getNumFrames(){
		return nframes;
	}
	void play(){
		assert(GetVar("frame")->GetFloat() == 0.0f);
		int dur = 1000 * getNumFrames() / fps;
		ic->GetVar("duration_ms")->Set((uint32)dur);
	}
	void stop(){
		ic->GetVar("duration_ms")->Set((uint32)0);
	}
	// TODO Implement pause? In ic?
	void setFrame(float n){
		component->GetVar("frame")->Set(n);
	}
	float getFrame(){
		return component->GetVar("frame")->GetFloat();
	}
	bool getLoop(){
		return mode_loop;
	}
	void setLoop(bool l){
		mode_loop = l;
		update_ic_target();
	}
	bool getReverse(){
		return mode_reverse;
	}
	void setReverse(bool r){
		mode_reverse = r;
		update_ic_target();
	}
protected:
	virtual void onFinish(){}
private:
	OverlayRenderComponent* component;
	int nframes;
	float fps;

	InterpolateComponent* ic;
	bool mode_loop, mode_reverse;

private:
	void update_ic_target(){
		InterpolateComponent::eOnFinish mode;
		if(mode_loop == 0 && mode_reverse==0)
			mode = InterpolateComponent::ON_FINISH_STOP;
		else if(mode_loop == 0 && mode_reverse){
			assert(0 && "Cannot do animation reverse when no looping");
		}
		else if(mode_loop && mode_reverse==0){
			mode = InterpolateComponent::ON_FINISH_REPEAT;
		}
		else if(mode_loop && mode_reverse){
			mode = InterpolateComponent::ON_FINISH_BOUNCE;
		}
			
		ic->GetVar("on_finish")->Set((uint32)mode);
	}
};

class LuaAnimatedItem: public AnimatedItem, public LuaScreenItem{
public:
	LuaAnimatedItem(luabind::object conf);
	~LuaAnimatedItem();
	static void luabind(lua_State* L);

private:
	luabind::object onFinish_cb;
	virtual void onFinish(){
		if(onFinish_cb)
			luabind::call_function<void>(onFinish_cb, this);
	}
};