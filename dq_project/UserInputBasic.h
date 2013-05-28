#pragma once

#include "WantFrameUpdate.h"
//#include "main.h"
#include "Entity/TouchHandlerComponent.h"
#include "Entity/Component.h"
#include "Entity/EntityUtils.h"
#include "PlatformSetup.h"

#include <cassert>

class RawInputObject{
public:
	struct input_state{
		float mx, my;
		bool btn[3];
		bool ctrl, shift, alt;
		input_state(){
			mx = my = 0;
			btn[0] = btn[1] = btn[2] = false;
			ctrl = shift = alt = false;
		}
	};

	virtual void onMouseMove(float x, float y, input_state state){}
	virtual void onMouseDown(int btn, input_state state){}
	virtual void onMouseUp(int btn, input_state state){}
	virtual void onDbClick(input_state state){}
	virtual void onChar(int chr, input_state state){}
};

// singleton for HGE user input processing
class UserInputBasic//: WantFrameUpdate
{
public:
	static UserInputBasic* getInstance(){
		if(instance == NULL)
			instance = new UserInputBasic();
		return instance;
	}
	void setClient(RawInputObject* c){
		client = c;
	}
	RawInputObject* getClient(){
		return client;
	}

private:
//	virtual void Update(float dt);				// not needed anymore, but I need code...
	Entity* entity;

	RawInputObject::input_state state;			// ctrl, mouse, etc...
	RawInputObject* client;						// events consumer

	static UserInputBasic* instance;

	UserInputBasic(){
		client = 0;

		// create entity
		entity = new Entity();
		AddFocusIfNeeded(entity);

		// make it fullscreen
		entity->GetVar("pos2d")->Set(0,0);
//		entity->GetVar("size2d")->Set(GetPrimaryGLX(), GetPrimaryGLY());
		entity->GetVar("size2d")->Set(GetScreenSizeX(), GetScreenSizeY());

		// add touch handler
		// TODO Who will delete? (memory management)
		EntityComponent* c = new TouchHandlerComponent();
		entity->AddComponent(c);

		// connect callbacks
		entity->GetFunction("OnOverMove")->sig_function.connect(boost::bind(&UserInputBasic::on_mouse_move, this, _1));
		entity->GetFunction("OnTouchStart")->sig_function.connect(boost::bind(&UserInputBasic::on_mouse_down, this, _1));
		entity->GetFunction("OnTouchEnd")->sig_function.connect(boost::bind(&UserInputBasic::on_mouse_up, this, _1));
	}
	~UserInputBasic(){
		delete entity;
	}

	// point, entity, finger_id, true
	void on_mouse_move(VariantList* args){

		float x = args->Get(0).GetVector2().x;
		float y = args->Get(0).GetVector2().y;

		client->onMouseMove(x, y, state);

		state.mx = x;
		state.my = y;
	}

	// point, entity, finger_id, true
	void on_mouse_down(VariantList* args){

		float x = args->Get(0).GetVector2().x;
		float y = args->Get(0).GetVector2().y;
		int btn = args->Get(2).GetUINT32();
		assert(btn >= 0 && btn < 3);

		// move it there - so UserInputDispatcher gets move event
		// also update state with current coords
		on_mouse_move(args);
		client->onMouseDown(btn, state);

		state.btn[btn] = true;
	}

	// point, entity, finger_id, true
	void on_mouse_up(VariantList* args){

		int btn = args->Get(2).GetUINT32();
		assert(btn >= 0 && btn < 3);

		client->onMouseUp(btn, state);

		state.btn[btn] = false;	
	}

	void on_db_click(){
		client->onDbClick(state);
	}

	void on_char(int chr){
		client->onChar(chr, state);
	}

	void on_key_down(int key){
		//if(key == HGEK_SHIFT)
		//	state.shift = true;
		//else if(key == HGEK_CTRL)
		//	state.ctrl = true;
		//else if(key == HGEK_ALT)
		//	state.alt = true;	
	}

	void on_key_up(int key){
		//if(key == HGEK_SHIFT)
		//	state.shift = false;
		//else if(key == HGEK_CTRL)
		//	state.ctrl = false;
		//else if(key == HGEK_ALT)
		//	state.alt = false;
	}
};
