#include "WantMouseInput.h"

WantMouseInput::WantMouseInput()
{
	MouseInput* input = MouseInput::getInstance();
	input->addClient(this);
}

WantMouseInput::~WantMouseInput(void)
{
	MouseInput* input = MouseInput::getInstance();
	input->removeClient(this);
}
