#pragma once
#include "WantFrameUpdate.h"
#include "screenresource.h"

class Animation: public ScreenResource, WantFrameUpdate
{
public:
	Animation(std::string tex, float w, float h, float tex_x, float tex_y, float nframes, float fps);
	Animation(hgeAnimation*);
	~Animation();

	virtual void Render(float x, float y, float rot);
	virtual void Update(float dt);
	virtual float getWidth(){
		return ani->GetWidth();
	}
	virtual float getHeight(){
		return ani->GetHeight();
	}

	int getNumFrames(){
		if(ani)
			return ani->GetFrames();
		else
			return 0;
	}
	void play(){
		if(ani)ani->Play();
	}
	void pause(){
		if(ani)ani->Stop();
	}
	void setFrame(int n){
		ani->SetFrame(n);
	}
	int getFrame(){
		return ani->GetFrame();
	}
	bool getLoop(){
		return ani->GetMode() & HGEANIM_LOOP;
	}
	void setLoop(bool l){
		if(l)
			ani->SetMode( ani->GetMode() | HGEANIM_LOOP );
		else
			ani->SetMode( ani->GetMode() & (~HGEANIM_LOOP) );
	}
	// we keep REV=PINGPONG
	bool getReverse(){
		return ani->GetMode() & HGEANIM_REV;
	}
	void setReverse(bool r){
		if(r)
			ani->SetMode( ani->GetMode() | HGEANIM_REV | HGEANIM_PINGPONG );
		else
			ani->SetMode( ani->GetMode() & ~(HGEANIM_REV | HGEANIM_PINGPONG) );
	}
protected:
	virtual void onFinish(){}
private:
	hgeAnimation* ani;
	HTEXTURE tex;
	bool need_delete;		// need to delete ani
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