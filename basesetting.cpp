#include "basesetting.h"

namespace space{

	BaseSetting::BaseSetting(ProgramSystemMain *_sys,std::vector<BaseSetting::Player> pl,unsigned int me,unsigned int _rds)
	: SystemState(_sys,BASESETTING)
{
	randomseed = _rds;

	//どうでもよさそうなカウンタ
	FinishBegin_cnt = UINT_MAX;

	//集計
	player.clear();
	player = pl;
	for(unsigned int i=0;i<player.size();i++){
		//入力初期化
		player[i].pushtime_left = 1;
		player[i].pushtime_middle = 0;
		player[i].pushtime_right = 0;
		player[i].pushtime_key.clear();
		//状況初期化
		player[i].typeofBase = -1;

	}
	//自分の番号
	player_ctrl = me;

	//拠点ボタンの位置と画像
	basebutton_circle.push_back(Circle2<double>(150,270,150));
	basebutton_surface.push_back(&SDLFILESRFS::inst().get("image/moneybase.gif"));
	basebutton_name.push_back("MONEY");
	basebutton_circle.push_back(Circle2<double>(330,270,150));
	basebutton_surface.push_back(&SDLFILESRFS::inst().get("image/love.gif"));
	basebutton_name.push_back("LOVE");
	basebutton_circle.push_back(Circle2<double>(510,270,150));
	basebutton_surface.push_back(&SDLFILESRFS::inst().get("image/power.gif"));
	basebutton_name.push_back("POWER");

}

BaseSetting::~BaseSetting(void)
{
}

void BaseSetting::draw(){


	//背景の背景
	//SDL_FillRect(systemmain->screen,NULL,0x003366);
	//背景
	{
		SDL_Rect yr = {
			systemmain->screen->w/2,systemmain->screen->h/2,
			systemmain->screen->w  ,systemmain->screen->h};
		SDLFILESRFS::inst().get("image/BS_background.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
	}

	//拠点選択のところ表示
	{
		for(unsigned int i=0;i<basebutton_surface.size();i++){
			SDL_Rect src = {150,0,150,150};
			SDL_Rect dst = {(Sint16)basebutton_circle[i].p.x,(Sint16)basebutton_circle[i].p.y,0,0};
			if( ( !player[player_ctrl].input->mouse.getleft().isPush() && basebutton_circle[i].Col_Dot(player[player_ctrl].input->mouse.getpos()) ) ||
				( i==player[player_ctrl].typeofBase  && !basebutton_circle[i].Col_Dot(player[player_ctrl].input->mouse.getpos()) ) ){
				src.x=0;dst.y+=5; }
			//ボタン
			basebutton_surface[i]->Draw(systemmain->screen,&dst,&src,systemmain->screen->h);
			//説明(+5,-65)
			src.x=0;src.y=55*i;src.w=140;src.h=55;
			dst.y=(Sint16)basebutton_circle[i].p.y-110;
			SDLFILESRFS::inst().get("image/all_para.gif").Draw(systemmain->screen,&dst,&src,systemmain->screen->h);
			//なまえ(10,35)
			systemmain->bmpfontred.draw(
				systemmain->screen,
				dst.x-20,
				dst.y+33,
				2.0,
				basebutton_name[i].c_str(),
				systemmain->screen->h);
		}
	}

	//4人分の表示
	{
		std::vector<SDL_Rect> dsp;
		std::stringstream ss;

		//背景
		for(unsigned int i=0;i<4;i++){
			SDL_Rect temp = {42+i*142,30,135,60};
			dsp.push_back(temp);
			temp.x += temp.w/2;
			temp.y += temp.h/2;
			space::SDLFILESRFS::inst().get("image/goldcard.gif").Draw(systemmain->screen,&temp,NULL,systemmain->screen->h);
		}

		//プレイヤーの数だけ表示
		for(unsigned int i=0;i<player.size();i++){
			SDL_Rect temp;
			SDL_Rect tems;

			//なまえ(+48,+38)
			ss.str("");
			ss << player[i].name;
			systemmain->bmpfont.draw(
				systemmain->screen,
				dsp[i].x+48,
				dsp[i].y+38,
				ss.str().c_str(),
				systemmain->screen->h);
		
			//プレイヤー番号
			//(+5,+35)
			tems.x=0;tems.y=20*i;tems.w=34;tems.h=20;
			temp.x=dsp[i].x+5+tems.w/2; temp.y=dsp[i].y+35+tems.h/2;
			space::SDLFILESRFS::inst().get("image/player.gif").Draw(systemmain->screen,&temp,&tems,systemmain->screen->h);

			//Ready?(+20,+8)
			tems.x=0;tems.y=0;tems.w=110;tems.h=20;
			if(player[i].typeofBase>=0)tems.x=110;
			temp.x=dsp[i].x+10+tems.w/2; temp.y=dsp[i].y+8+tems.h/2;
			space::SDLFILESRFS::inst().get("image/choosingok.gif").Draw(systemmain->screen,&temp,&tems,systemmain->screen->h);
		}
	}


}

void BaseSetting::update(){
	
	if(counter < FinishBegin_cnt){
		//bgmが再生されてなかったら再生。
		if(!systemmain->sound_bgm.isPlaying()){
			systemmain->sound_bgm.load("sound/waittime.wav");
			systemmain->sound_bgm.fadein(480);
		}
	}

	//===========
	//入力のこと
	//==========
	for(unsigned i=0;i<player.size();i++)
	{
		Input ctrinp = *(player[i].input);
		//mouse
		if( ctrinp.mouse.getleft().isPush() ) player[i].pushtime_left++;
		else  player[i].pushtime_left = 0;
		if( ctrinp.mouse.getmiddle().isPush() ) player[i].pushtime_middle++;
		else  player[i].pushtime_middle = 0;
		if( ctrinp.mouse.getright().isPush() ) player[i].pushtime_right++;
		else  player[i].pushtime_right = 0;
		//key
		const Keyboard::KeyMap &km = ctrinp.keyboard.getkeys();
		std::map<unsigned short,unsigned short> &now = player[i].pushtime_key;
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
	//カウンタが一定貯まったら別の画面に移行したりする。
	if(counter>FinishBegin_cnt){
		if(counter-FinishBegin_cnt > 30){
			//次へ。
			isNeed = false;
			std::vector<GameMain::Player> pls;
			for(unsigned int i=0;i<player.size();i++)
			{
				GameMain::Player pl;
				((Input*)player[i].input)->clear();
				pl.input = player[i].input;
				pl.name = player[i].name;
				pl.typeofBase = player[i].typeofBase;
				pls.push_back(pl);
			}
			systemmain->nowstates.push_back(new GameMain(systemmain,pls,player_ctrl,randomseed));
			//フェードイン
			systemmain->setFadeIn(30);
		}
	}
	//それ以外はいつも通り。
	else{
		//全プレイヤーについて
		for(unsigned int i=0;i<player.size();i++){
			//左クリックが押された
			if( player[i].pushtime_left == 1 ){
				//全ボタンに関して
				unsigned int j;
				for(j=0;j<basebutton_circle.size();j++){
					//押されたとき
					if(basebutton_circle[j].Col_Dot(player[i].input->mouse.getleft().getPushpos())){
						if(j==player[i].typeofBase){
							player[i].typeofBase = -1;
							systemmain->sound_cancel.play();
						}
						else{
							player[i].typeofBase = j;
							systemmain->sound_enter.play();
						}
						break;
					}
				}
				if(j==basebutton_circle.size()){player[i].typeofBase=-1;}
			}
		}

		//全員おしたっぽいなら
		bool allover=true;
		for(unsigned int i=0;i<player.size();i++){if(player[i].typeofBase<0)allover=false;}
		if(allover){
			//おわる
			FinishBegin_cnt = counter;
			//フェードアウト
			systemmain->setFadeOut(30);
			systemmain->sound_bgm.fadeout(480);
		}
	}

	//カウンタ
	counter++;

	return;
}

}//end of namespace space
