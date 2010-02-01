#include "common.h"

namespace space{

	//output font texture
	void draw_str(const std::string &str)
	{
		static TTF_Font *font = TTF_OpenFont("TT0246M_.TTF",14);
		static SDL_Color white = {255,255,255,255};
		GLTEXTURE fonttex;
		SDL_Surface *fontsf = TTF_RenderText_Solid(font,str.c_str(),white);
		if(!fontsf) exit(-1);
		fonttex.LoadSurface( fontsf );
		glEnable(GL_ALPHA_TEST);
		fonttex.draw();
		glDisable(GL_ALPHA_TEST);
	}
	//サーフェイスに出力
	void draw_str(const std::string &str,SDL_Surface *w,SDL_Rect *r,unsigned int wh)
	{
		SDLSURFACE sdlsrf;
		sdlsrf.LoadText(str);
		sdlsrf.Draw(w,r,NULL,wh);
	}

	//fps調整用wait
	void wait_byFPS(unsigned int hope_fps,bool view){ 
		static unsigned int fps = 0;
		static unsigned int t1=0,t2=0;
		static char str[256] = "wait...";

		unsigned int t3;

		do{
			t3 = SDL_GetTicks();
		}while(t3 - t1 < 1000/hope_fps);
		t1 = t3;

		if(view)
		{
			if(t1 - t2 > 1000) {
				sprintf(str,"%5.2lf", (1000.0 * fps)/(double)(t1-t2));
				t2 = t1;
				fps = 0; 
			}
			draw_str(str);

			fps++;
		}
	}
	//fps調整用wait(SDLで描画だよ)
	void wait_byFPS(unsigned int hope_fps,SDL_Surface* w,SDL_Rect* r,unsigned int wh){ 
		static unsigned int fps = 0;
		static unsigned int t1=0,t2=0;
		static char str[256] = "wait...";

		unsigned int t3;

		//待ち
		do{
			t3 = SDL_GetTicks();
		}while(t3 - t1 < 1000/hope_fps);
		t1 = t3;

		//描画
		if(t1 - t2 > 1000) {
			sprintf(str,"%5.2lf", (1000.0 * fps)/(double)(t1-t2));
			t2 = t1;
			fps = 0; 
		}
		draw_str(str,w,r,wh);
		fps++;
		
	} 
}
