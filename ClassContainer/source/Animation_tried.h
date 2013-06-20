#pragma once
#include "Entity/SpriteAnimationRenderComponent.h"
#include "Entity/InterpolateComponent.h"

class Animation: public SpriteAnimationRenderComponent
{
public:
	Animation(std::string file, std::string anim, float fps);
	~Animation();

	virtual void OnAdd(Entity* e);
	virtual void OnRemove();

	virtual float getWidth(){
		// TODO: what about speed of this?!
		return GetResourceManager()->GetSpriteAnimationSet(file_name)->GetAnimation(anim_name)->GetFrame(0)->GetBoundingBox().x;
	}
	virtual float getHeight(){
		// TODO: what about speed of this?!
		return GetResourceManager()->GetSpriteAnimationSet(file_name)->GetAnimation(anim_name)->GetFrame(0)->GetBoundingBox().y;
	}

	int getNumFrames(){
		GetResourceManager()->GetSpriteAnimationSet(file_name)->GetAnimation(anim_name)->GetFrameCount();
	}
	void play(){
		float dur = getNumFrames() / fps;
		float phase = GetVar("phase")->GetFloat();
		ic->GetVar("duration_ms")->Set((1.0f-phase)*dur);
	}
	void pause(){
		ic->GetVar("duration_ms")->Set(0);
	}
	void setFrame(int n){
		// TODO test if it really sets the start of this frame!
		GetVar("phase")->Set((float)n / getNumFrames());
	}
	int getFrame(){
		float phase = GetVar("phase")->GetFloat();
		return GetResourceManager()->GetSpriteAnimationSet(file_name)->GetAnimation(anim_name)->GetFrameAtPhase(phase);
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
		mode_reverse = l;
		update_ic_target();
	}
protected:
	virtual void onFinish(){}
private:
	std::string file_name, anim_name;
	float fps;
	InterpolateComponent* ic;
	bool mode_loop, mode_reverse;

private:
	void update_ic_target(){
		InterpolateComponent::eOnFinish mode;
		if(mode_loop == 0 && mode_reverse==0)
			mode = InterpolateComponent::ON_FINISH_STOP;
		else if(mode_loop == 0 && mode_reverse==1){
			assert(0 && "Cannot do animation reverse when no looping");
		}
		else if(mode_loop == 1 && mode_reverse==0){
			mode = InterpolateComponent::ON_FINISH_REPEAT;
		}
		else if(mode_loop == 1 && mode_reverse==1){
			mode = InterpolateComponent::ON_FINISH_BOUNCE;
		}
			
		ic->GetVar("target")->Set(mode);
	}
};

class LuaAnimation: public Animation{
public:
	LuaAnimation(luabind::object conf);
	LuaAnimation(std::string name);
	static void luabind(lua_State* L);

private:
	luabind::object onFinish_cb;
	virtual void onFinish(){
		if(onFinish_cb)
			luabind::call_function<void>(onFinish_cb, this);
	}
};