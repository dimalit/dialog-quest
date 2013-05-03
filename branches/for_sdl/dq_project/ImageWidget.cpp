#include "ImageWidget.h"

HTEXTURE load(const std::string& path, std::string file){
	HTEXTURE h = hge->Texture_Load((path + '/' + file + ".jpg").c_str());
	if(!h)	 h = hge->Texture_Load((path + '/' + file + ".png").c_str());
	return h;
}

ImageWidget::ImageWidget(float x, float y)
:Widget(x, y)
{
	tex_normal			= 0;
	tex_over			= 0;
	tex_disabled		= 0;
	tex_checked			= 0;
	tex_checked_over	= 0;
	tex_normal2checked	= 0;

	ani_normal			= 0;
	ani_over			= 0;
//	ani_disabled		= 0;
	ani_checked			= 0;
//	ani_checked_over	= 0;
	ani_normal2checked  = 0;
}

void ImageWidget::setAnim(view_type type, std::string tex, float w, float h, float tex_x, float tex_y, float nframes, float fps){
	
	// select tex
	HTEXTURE* ptex;
	hgeAnimation** pani;
	switch(type){
	case VIEW_NORMAL:
		ptex = &tex_normal;
		pani = &ani_normal;
		break;
	case VIEW_OVER:
		ptex = &tex_over;
		pani = &ani_over;
		break;
	case VIEW_CHECKED:
		ptex = &tex_checked;
		pani = &ani_checked;
		break;
	case NORMAL2CHECKED:
		ptex = &tex_normal2checked;
		pani = &ani_normal2checked;
	}

	*ptex = hge->Texture_Load(tex.c_str());
	*pani = new hgeAnimation(*ptex, nframes, fps, tex_x, tex_y, w, h);
	if(*pani){
		(*pani)->SetHotSpot(w/2, h/2);
		if(type != NORMAL2CHECKED)
			(*pani)->Play();
		else
			(*pani)->SetMode(HGEANIM_FWD | HGEANIM_NOLOOP);
	}
}

ImageWidget::~ImageWidget(void)
{
	if(tex_normal)
		hge->Texture_Free(tex_normal);
	if(tex_over)
		hge->Texture_Free(tex_over);
	if(tex_disabled)
		hge->Texture_Free(tex_disabled);
	if(tex_checked)
		hge->Texture_Free(tex_checked);
	if(tex_checked_over)
		hge->Texture_Free(tex_checked_over);
	if(tex_normal2checked)
		hge->Texture_Free(tex_checked_over);
	// TODO: free anim!
}

// screen representation
void ImageWidget::Render(){
	if(visible){
		anim->RenderEx(x, y, rot);
		Widget::Render();
	}
}
void ImageWidget::Update(float dt){
	// sitch anim if no looping and finished
	if(!(anim->GetMode() & HGEANIM_LOOP) && !anim->IsPlaying()){
		update_view();
	}
	anim->Update(dt);
}

void ImageWidget::update_view(){

	// check normal2checked transition first
	if(anim == ani_normal && checked && ani_normal2checked){
		anim = ani_normal2checked;
		anim->Play();
	}
	// default model
	else{
		anim = ani_normal;

		// TODO: внимательно продумать тут логику!
		if(enabled && checked && ani_checked)
			anim = ani_checked;
		else if(enabled && over && !checked && ani_over)
			anim = ani_over;
	//	else if(enabled && over && checked && ani_checked_over)
	//		anim = ani_checked_over;
	}
	float w = anim->GetWidth();
	float h = anim->GetHeight();
	x1 = x - w/2; y1 = y - h/2; x2 = x + w/2; y2 = y + h/2;
}
