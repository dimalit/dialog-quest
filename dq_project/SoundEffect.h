#pragma once

#include <string>
#include "main.h"

class SoundEffect
{
public:
	SoundEffect(std::string path);
	~SoundEffect();
	void play(int volume = 100, int pan = 0, float pitch = 1.0, bool loop = false);
	float getLength();
private:
	HEFFECT hef;
	HCHANNEL channel;
};
