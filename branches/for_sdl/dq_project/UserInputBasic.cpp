#include "UserInputBasic.h"

// singleton
UserInputBasic UserInputBasic::instance;

void UserInputBasic::Update(float dt){
	hgeInputEvent evt;
	while(hge->Input_GetEvent(&evt)){

		if(evt.type == INPUT_MOUSEMOVE){
			on_mouse_move(evt.x, evt.y);
		}// move
		else if(evt.type==INPUT_MBUTTONDOWN || evt.type==INPUT_MBUTTONUP){
			int key;
			if(evt.key == HGEK_LBUTTON)
				key = 0;
			else if(evt.key == HGEK_RBUTTON)
				key = 1;
			else if(evt.key == HGEK_MBUTTON)
				key = 2;
			else
				assert(false);

			if(evt.type==INPUT_MBUTTONDOWN){
				// call dblick instead of down if needed
				if(evt.flags & HGEINP_REPEAT)
					on_db_click();
				else
					on_mouse_down(key);
			}
			else if(evt.type==INPUT_MBUTTONUP)
				on_mouse_up(key);
			else
				assert(false);
		}// mouse up/down
		else if(evt.type==INPUT_KEYUP || evt.type==INPUT_KEYDOWN){
			int key = evt.key;
			int chr = evt.chr;

			if(evt.type==INPUT_KEYDOWN){
				// down only first time
				if(!(evt.flags & HGEINP_REPEAT))
					on_key_down(key);
				// char - every time (except control keys)
				if(chr)on_char(chr);
			}
			else if(evt.type==INPUT_KEYUP){
				on_key_up(key);
			}
			else
				assert(false);
		}// key up/down
	}// while event
}