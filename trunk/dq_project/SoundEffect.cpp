#include "SoundEffect.h"

SoundEffect::SoundEffect(std::string path)
{
	hef = 0;//!!!hge->Effect_Load(path.c_str());
	channel = 0;
}

SoundEffect::~SoundEffect(void)
{
	if(hef)
		hge->Effect_Free(hef);
}

void SoundEffect::play(int volume, int pan, float pitch, bool loop){
	if(hef)
		channel = hge->Effect_PlayEx(hef, volume, pan, pitch, loop);
}

float SoundEffect::getLength(){
	// !!!
	return 1;
	assert(channel);
	return hge->Channel_GetLength(channel);
}