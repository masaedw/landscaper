#include "effectstate.h"

namespace space{

//フェードアウト
StateFadeOut::StateFadeOut(ProgramSystemMain *_sys,unsigned int _count) : SystemState(_sys,FADEOUT)
{
	maxcount = _count;
	
}
StateFadeOut::~StateFadeOut(void){}
void StateFadeOut::draw(){
	unsigned int valc = (unsigned int)(255.0 * (double)(counter)/(double)(maxcount));
	valc = (valc>255) ? 255 : valc;
	SDL_Rect r = {0,0,systemmain->screen->w,systemmain->screen->h};
	Sint16 x[4] = {0,r.w,r.w,0};
	Sint16 y[4] = {0,0,r.h,r.h};
	
	filledPolygonRGBA(systemmain->screen,x,y,4,0,0,0,valc);
}
void StateFadeOut::update(){
	if(counter>maxcount)isNeed=false;
	else counter++;
}

//フェードイン
StateFadeIn::StateFadeIn(ProgramSystemMain *_sys,unsigned int _count) : SystemState(_sys,FADEIN)
{
	maxcount = _count;
	
}
StateFadeIn::~StateFadeIn(void){}
void StateFadeIn::draw(){
	unsigned int valc = (unsigned int)(255.0 * (double)(counter)/(double)(maxcount));
	valc = (valc>255) ? 255 : valc;
	valc = 255 - valc;
	SDL_Rect r = {0,0,systemmain->screen->w,systemmain->screen->h};
	Sint16 x[4] = {0,r.w,r.w,0};
	Sint16 y[4] = {0,0,r.h,r.h};
	
	filledPolygonRGBA(systemmain->screen,x,y,4,0,0,0,valc);
}
void StateFadeIn::update(){
	if(counter>=maxcount)isNeed=false;
	else counter++;
}

//デモの表示
StateDisplayDemo::StateDisplayDemo(ProgramSystemMain *_sys) : SystemState(_sys,DISPLAYDEMO){counter=0;}
void StateDisplayDemo::draw(){

	//ちょっとした背景
	{
		char str[] = "DEMO -LANDSCAPER-";
		int length = strlen(str)*systemmain->bmpfont.getW();
		SDL_Rect ra= {(counter%(systemmain->screen->w+length))-length,systemmain->screen->h/2,350,14};
		SDL_Rect r = {0,ra.y,systemmain->screen->w,14};
		
		Sint16 x[4] = {r.x,r.x+r.w,r.x+r.w,r.x};
		Sint16 y[4] = {r.y,r.y,r.y+r.h,r.y+r.h};
		
		filledPolygonRGBA(systemmain->screen,x,y,4,0,64,192,(unsigned char)(63.0+96.0+96.0*sin((double)(counter*2.0)*M_PI/180.0)));
		systemmain->bmpfont.draw(systemmain->screen,ra.x,systemmain->screen->h-ra.y-ra.h,str,systemmain->screen->h);
		
	}

}
void StateDisplayDemo::update(){counter++;}


}//end of namespace space

