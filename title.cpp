#include "title.h"

namespace space{

Title::Title(ProgramSystemMain *_sys) : SystemState(_sys,TITLE)
{
	pushtime_left = 1;
	pushtime_middle = 0;
	pushtime_right = 0;
	pushtime_key.clear();
	pushtime_key[SDLK_RETURN] = 1;
	input = &systemmain->input[0];
}

Title::~Title(void)
{
}

void Title::draw(){
	char _str[256];
	

	//背景の背景
	SDL_FillRect(systemmain->screen,NULL,0x003366);
	//背景
	{
		SDL_Rect yr = {
			systemmain->screen->w/2,systemmain->screen->h/2,
			systemmain->screen->w  ,systemmain->screen->h};
		SDLFILESRFS::inst().get("image/title.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
	}	
	//ロゴ表示
	{
		SDL_Rect yr = {
			systemmain->screen->w/2,systemmain->screen->h/2,
			systemmain->screen->w  ,systemmain->screen->h};
		SDLFILESRFS::inst().get("image/title_logo.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
	}

	//点滅で指示だよ
	if(counter%52 < 40)
	{
		sprintf(_str,"PRESS ENTER OR CLICK TO GAME START.");
		systemmain->bmpfont.draw(
			systemmain->screen,
			systemmain->screen->w/2-(strlen(_str)*11)/2,
			systemmain->screen->h/2-100,
			_str,
			systemmain->screen->h);
	}
}

void Title::update(){

	//bgmが再生されてなかったら再生。
	if(!systemmain->sound_bgm.isPlaying()){
		systemmain->sound_bgm.load("sound/waittime.wav");
		systemmain->sound_bgm.fadein(480);
	}

	//===========
	//入力のこと
	//==========
	{
		Input ctrinp = *input;
		//mouse
		if( ctrinp.mouse.getleft().isPush() ) pushtime_left++;
		else pushtime_left = 0;
		if( ctrinp.mouse.getmiddle().isPush() ) pushtime_middle++;
		else pushtime_middle = 0;
		if( ctrinp.mouse.getright().isPush() ) pushtime_right++;
		else pushtime_right = 0;
		//key
		const Keyboard::KeyMap &km = ctrinp.keyboard.getkeys();
		std::map<unsigned short,unsigned short> &now = pushtime_key;
		//切る
		std::map<unsigned short,unsigned short>::iterator nowit = now.begin();
		for(;nowit!=now.end();nowit++)
		{
			if( km.find(nowit->first) == km.end() ){
			  std::map<unsigned short,unsigned short>::iterator tit = nowit;
			  nowit--;now.erase(tit);
			  if(nowit==now.end())break;
			}
			else { nowit->second++; }
		}
		//追加する
		Keyboard::KeyMap::const_iterator kmit = km.begin();
		for(;kmit!=km.end();kmit++)
		{
			if( now.find(kmit->first) == now.end() ) now[kmit->first]=1;
		}
	}

	//====================
	//とくにないけどまぁ。
	//====================
	//Enterまたはクリック時
	if(	(pushtime_key.count(SDLK_RETURN) && pushtime_key[SDLK_RETURN]==1) ||
		(pushtime_left == 1) ){
		//次へ。
		isNeed = false;
		systemmain->setGameSetting();
		systemmain->sound_enter.play();
		return;
	}

	//カウンタが一定貯まったら別の画面に移行したりする。
	if(counter > 600){
		isNeed = false;
		systemmain->setGameDemo();
		systemmain->setFadeIn(30);
	}
	else if(counter > 570){
		systemmain->setFadeOut(30);
	}

	//カウンタ
	counter++;
}

}//end of namespace space

