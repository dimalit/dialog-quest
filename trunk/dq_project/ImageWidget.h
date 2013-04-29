#pragma once

#include <hgeAnim.h>
#include <hgerect.h>

#include "Widget.h"

class ImageWidget: public virtual Widget
{
public:
	// screen representation
	virtual void Render();
	virtual void Update(float dt);
protected:
	ImageWidget(float x = 0.0f, float y = 0.0f);
	~ImageWidget(void);
	void setAnim(view_type type, std::string tex, float w, float h, float tex_x, float tex_y, float nframes = 1, float fps = 0);
	// utility
	virtual void update_view();
private:
	// no value semantic!
	ImageWidget& operator=(const ImageWidget&){assert(false);}
	ImageWidget(const ImageWidget&):Widget(0,0){assert(false);}

	// screen
	HTEXTURE tex_normal, tex_over, tex_disabled,
			 tex_checked,tex_checked_over,tex_normal2checked;
	hgeAnimation *ani_normal, *ani_over, *ani_checked, *ani_normal2checked;
	hgeAnimation* anim;
};
