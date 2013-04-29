#pragma once

#include <cassert>

#include "main.h"
#include "WantFrameUpdate.h"

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
class UserInputBasic: WantFrameUpdate
{
public:
	static UserInputBasic* getInstance(){ return &instance; }
	void setClient(RawInputObject* c){
		client = c;
	}
	RawInputObject* getClient(){
		return client;
	}

private:
	virtual void Update(float dt);				// called every frame
	RawInputObject::input_state state;			// ctrl, mouse, etc...
	RawInputObject* client;						// events consumer

	static UserInputBasic instance;

	UserInputBasic(){
		client = 0;
	}

	void on_mouse_move(float x, float y){

		client->onMouseMove(x, y, state);

		state.mx = x;
		state.my = y;
	}

	void on_mouse_down(int btn){
		assert(btn >= 0 && btn < 3);

		client->onMouseDown(btn, state);

		state.btn[btn] = true;
	}

	void on_mouse_up(int btn){
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
		if(key == HGEK_SHIFT)
			state.shift = true;
		else if(key == HGEK_CTRL)
			state.ctrl = true;
		else if(key == HGEK_ALT)
			state.alt = true;	
	}

	void on_key_up(int key){
		if(key == HGEK_SHIFT)
			state.shift = false;
		else if(key == HGEK_CTRL)
			state.ctrl = false;
		else if(key == HGEK_ALT)
			state.alt = false;
	}
};
