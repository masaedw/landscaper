#ifndef _COMMON_H_
#define _COMMON_H_

#pragma warning(disable:4996)

#include<vector>
#include<queue>
#include<list>
#include<set>
#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<map>
#include<bitset>

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>

#include "setsdl.h"
#include "graphic.h"


namespace space{
	//描画用の超高速乱数
	inline unsigned long xor128(){
		static unsigned long x=123456789,y=362436069,z=521288629,w=88675123;
		unsigned long t;
		t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) );
	}

	//ポインタキャスト
	template <class T>
	inline T pointer_cast(void *p){ return (T)p; }
	template <class T>
	inline T const_pointer_cast(const void *p){ return (T)p; }

	//基本アブストラクト
	//「描画及び更新が存在する」
	class DrawAndUpdate  
	{
	public:
		DrawAndUpdate(){}
		virtual ~DrawAndUpdate(){}

		virtual void update() = 0;
		virtual void draw() = 0;
		virtual void after_all(){}
	};

	//output font texture
	void draw_str(const std::string &);
	void draw_str(const std::string &,SDL_Surface*,SDL_Rect*,unsigned int wh=0);

	//fps調整用wait
	void wait_byFPS(unsigned int hope_fps,bool view=false);
	void wait_byFPS(unsigned int hope_fps,SDL_Surface*,SDL_Rect*,unsigned int wh=0);

	//FPS制御クラス
	class FPScontrol{
	private:
		unsigned int temp_fps;
		double measured_fps;
		unsigned int measure_count;
		unsigned int measurestart_count;
		unsigned int fps_count;
		unsigned int waitstart_count;

		bool onlyupdate_flg;

	public:
		void setFPS(double fps){fps_count = (unsigned int)(1000/fps);}
		void setMeasureFPS(double fps){ measure_count = (unsigned int)(1000/fps);}
		double getSettingFPS(){return 1000.0/(double)fps_count;}
		double getMeasureFPS(){return 1000.0/(double)measure_count;}
		double getFPS(){return measured_fps;}
		bool onlyUpdate(){return onlyupdate_flg;}

		FPScontrol(double fps=60.00,double mfps=2.00){
			setFPS(fps);
			setMeasureFPS(mfps);
			temp_fps=0;
			measured_fps=0.0;
			measurestart_count=0;
			waitstart_count=0;
			onlyupdate_flg = false;
		}
		virtual ~FPScontrol(){}
		
		//draw後に呼び出す待ち関数
		virtual void delay(){
			//とりあえず測る
			unsigned int t3 = SDL_GetTicks();

			//draw飛ばしフラグ
			//一回飛ばしたら元に戻す。
			if(onlyupdate_flg) onlyupdate_flg = false;
			//飛ばしてなくて、60fpsでなさそうなら、draw飛ばしフラグを立てる。
			else if(t3 - waitstart_count > fps_count) onlyupdate_flg = true;

			//待ち
			while(t3 - waitstart_count < fps_count) t3 = SDL_GetTicks();
			waitstart_count = t3;

			//FPS計測
			if(t3 - measurestart_count > measure_count) {
				measured_fps = (1000.0 * temp_fps)/(double)(t3 - measurestart_count);
				measurestart_count = t3;
				temp_fps = 0; 
			}
			temp_fps++;
		}

	};

}


#endif
