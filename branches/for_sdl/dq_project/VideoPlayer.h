#ifndef HEADER_VideoPlayer_h
#define HEADER_VideoPlayer_h
 
#if (_MSC_VER > 1000) || (__GNUC__ >= 3)
#pragma once
#endif
 
#include <string>
#include <memory>

#include "Visual.h"
#include "WantFrameUpdate.h"

#include <hge.h>
#include <hgeSprite.h>
#include <hgeVector.h>

class VideoPlayer: WantFrameUpdate, public Visual
{
public:
	VideoPlayer();
	~VideoPlayer();
	bool Open(const std::string& fileName, const hgeVector& posPlayer, const hgeVector& sizePlayer=hgeVector(0.0f, 0.0f));
	bool Open(void* data, DWORD size, const hgeVector& posPlayer, const hgeVector& sizePlayer=hgeVector(0.0f, 0.0f));
	void Close();
	void Update(float time);
	void Render();
	bool IsPlaying() const;
	HTEXTURE GetTexture() const
	{
		return mTexture;
	}
	const char* LoadFile(const std::string& fileName, DWORD& size);
 
private: // disable copying
	VideoPlayer(const VideoPlayer&);
	VideoPlayer& operator=(const VideoPlayer&);
 
private:
	class Impl;
	std::auto_ptr<Impl> mImpl;
	HTEXTURE mTexture;
	float mTime;
	HGE* mHge;
	hgeSprite* mSprite;
	hgeVector mScale;
	hgeVector mPos;
};
 
#endif // HEADER_VideoPlayer_h