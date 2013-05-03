#include "main.h"

#include "MouseInput.h"
#include "WantMouseInput.h"

MouseInput MouseInput::theInput;

MouseInput::MouseInput(void)
{
	prev_x = prev_y = 0.0f;
	over_client = drag_client = 0;
}

MouseInput::~MouseInput(void)
{
}

void MouseInput::Update(float dt){
return;
	hgeInputEvent evt;
	while(hge->Input_GetEvent(&evt)){

		float dx, dy;
		if(evt.type == INPUT_MOUSEMOVE){
			dx = evt.x - prev_x;
			dy = evt.y - prev_y;

			bool out = false;
			WantMouseInput* new_over = 0;

			for(std::set<WantMouseInput*>::iterator i = clients.begin();
				i != clients.end(); ++i)
			{
				WantMouseInput* c = *i;
				bool point_in = c->isPointIn(evt.x, evt.y);

				if(c == drag_client)
					c->onDrag(dx, dy);

				if(c == over_client && point_in)
					c->onMouseMove(evt.x, evt.y);

				if(c == over_client && !point_in){
					c->onMouseOut();
					out = true;
				}
				if(c != over_client && point_in){
					c->onMouseOver(evt.x, evt.y);
					new_over = c;
				}
			}// for
			
			if(out)
				over_client = 0;
			if(new_over){
				over_client = new_over;
				// additional move along with over
				over_client->onMouseMove(evt.x, evt.y);
			}

			prev_x = evt.x;
			prev_y = evt.y;
		}// move
		else if(evt.type==INPUT_MBUTTONDOWN && evt.key==HGEK_LBUTTON){
			for(std::set<WantMouseInput*>::iterator i = clients.begin();
				i != clients.end(); ++i)
			{
				WantMouseInput* c = *i;
				if(c->isPointIn(evt.x, evt.y)){
					c->onDragStart(evt.x, evt.y);
					drag_client = c;
				}
			}// for
		}// down
		else if(evt.type==INPUT_MBUTTONUP && evt.key==HGEK_LBUTTON){
			if(drag_client){
				drag_client->onDragEnd(evt.x, evt.y);
				drag_client = 0;
			}
		}// up
	}// while event
}