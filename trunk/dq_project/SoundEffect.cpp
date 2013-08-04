#include "PlatformPrecomp.h"
#include "BaseApp.h"
#include "SoundEffect.h"

SoundEffect::SoundEffect(std::string path)
{
	filename = path;
	GetAudioManager()->Preload(filename);
}

SoundEffect::~SoundEffect(void)
{
}

//TODO What about stop() in SoundEffect if we have loop?
void SoundEffect::play(int volume, int pan, float pitch, bool loop){
	AudioHandle h = GetAudioManager()->Play(filename, loop);
	GetAudioManager()->SetVol(h, volume/100.0f);
	GetAudioManager()->SetPan(h, pan/100.0f);
	//TODO We ignore pitch!
	//TODO Also what about units of volume and pan?
	assert(pitch==1.0f);
}

float SoundEffect::getLength(){
	// !!!
	return 1;
	//assert(channel);
	//return hge->Channel_GetLength(channel);
}