#include "PlatformPrecomp.h"
#include "BaseApp.h"
#include "SoundEffect.h"

SoundEffect::SoundEffect(std::string path)
{
	filename = path;
	GetAudioManager()->Preload(filename);
	handle = 0;
}

SoundEffect::~SoundEffect(void)
{
}

//TODO What about stop() in SoundEffect if we have loop?
void SoundEffect::play(int volume, int pan, float pitch, bool loop){
	handle = GetAudioManager()->Play(filename, loop);
	GetAudioManager()->SetVol(handle, volume/100.0f);
	GetAudioManager()->SetPan(handle, pan/100.0f);
	//TODO We ignore pitch!
	//TODO Also what about units of volume and pan?
	assert(pitch==1.0f);
}

float SoundEffect::getLength(){
	float len = GetAudioManager()->GetLength(handle);
		assert(len > 0.0f);
	return len;
	//assert(channel);
	//return hge->Channel_GetLength(channel);
}