#include "result.h"

namespace space{

Result::Result(ProgramSystemMain *_sys,GameMain *gm) : SystemState(_sys,RESULT)
{

	//どうでもよさそうなカウンタ
	FinishBegin_cnt = UINT_MAX;

	//配列初期化
	player.clear();
	for(unsigned int i=0;i<gm->player.size();i++){
		player.push_back(Player());
		//入力初期化
		player[i].pushtime_left = 1;
		player[i].pushtime_middle = 0;
		player[i].pushtime_right = 0;
		player[i].pushtime_key.clear();
		player[i].input = gm->player[i].input;
		player[i].pushtime_key[SDLK_RETURN] = 1;
		//状況初期化
		player[i].isReady = false;
	}

	//順位集計
	rank.clear();
	for(unsigned int i=0;i<gm->player.size();i++){rank.push_back(i);}
	//hp多い順 > 倒した数多い順 > 倒された数少ない順 でランク配列をソート
	for(unsigned int j=0;j<gm->player.size();j++){
		for(unsigned int i=j+1;i<gm->player.size();i++){
			//HPが多いなら
			if(gm->player[rank[j]].base->hp < gm->player[rank[i]].base->hp) std::swap(rank[j],rank[i]);
			else if(gm->player[rank[j]].base->hp == gm->player[rank[i]].base->hp){
				//倒した数が多いなら
				if(gm->player[rank[j]].killed_cnt < gm->player[rank[i]].killed_cnt) std::swap(rank[j],rank[i]);
				else if(gm->player[rank[j]].killed_cnt == gm->player[rank[i]].killed_cnt){
					//倒された数が少ないなら
					if(gm->player[rank[j]].bekilled_cnt > gm->player[rank[j]].bekilled_cnt) std::swap(rank[j],rank[i]);
				}
			}
		}
	}
	//結果集計
	for(unsigned int i=0;i<gm->player.size();i++){
		player[i].name = gm->player[i].name;
		player[i].spendmoney = gm->player[i].spendmoney;
		player[i].killed = gm->player[i].killed_cnt;
		player[i].bekilled = gm->player[i].bekilled_cnt;
		player[i].typeofBase = gm->player[i].typeofBase;
	}
	for(unsigned int i=0;i<rank.size();i++){
		player[rank[i]].rank=i+1;
	}

	//決定ボタンの位置
	readybutton_rect = WinRect(
		0,0,
		systemmain->screen->w,
		systemmain->screen->h);
	player_ctrl = gm->player_ctrl;
}

Result::~Result(void)
{
}

void Result::draw(){
	
	std::stringstream ss;

	//bgmが再生されてなかったら再生。
	if(!systemmain->sound_bgm.isPlaying()){
		systemmain->sound_bgm.load("sound/waittime.wav");
		systemmain->sound_bgm.fadein(480);
	}

	//背景の背景
	//SDL_FillRect(systemmain->screen,NULL,0x003366);
	//背景
	{
		SDL_Rect yr = {
			systemmain->screen->w/2,systemmain->screen->h/2,
			systemmain->screen->w  ,systemmain->screen->h};
		SDLFILESRFS::inst().get("image/Result.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
		//拠点によって画像をかえる
		SDL_Rect ra = {0,0,240,19};
		yr.y=415;yr.x=490;
		if(player[rank[0]].typeofBase==BASETYPE_LOVE)ra.y=38;
		else if(player[rank[0]].typeofBase==BASETYPE_MONEY)ra.y=19;
		else if(player[rank[0]].typeofBase==BASETYPE_POWER)ra.y=0;
		//画像表示
		SDLFILESRFS::inst().get("image/resulttxt.gif").Draw(systemmain->screen,&yr,&ra,systemmain->screen->h);
	}
	
	//勝った人は誰？
	for(unsigned int i=0;i<player.size();i++){
		if(player[i].rank==1){
			ss.str("");
			ss << player[i].name << " WON!";
			systemmain->bmpfont.draw(
				systemmain->screen,
				75,300,60.0/(double)systemmain->bmpfont.getH(),
				ss.str().c_str(),
				systemmain->screen->h);
			break;
		}
	}

	//4人分の勝敗
	for(unsigned int i=0;i<player.size();i++){
		//拠点によって画像をかえる
		{
			SDL_Rect ra = {150,0,150,150};
			if(rank[0]==i)ra.x=0;
			SDL_Rect yr = {45+142*i+67,165,0,0};
			     if(player[i].typeofBase==BASETYPE_LOVE)
				SDLFILESRFS::inst().get("image/love.gif").Draw(systemmain->screen,&yr,&ra,systemmain->screen->h);
			else if(player[i].typeofBase==BASETYPE_MONEY)
				SDLFILESRFS::inst().get("image/moneybase.gif").Draw(systemmain->screen,&yr,&ra,systemmain->screen->h);
			else if(player[i].typeofBase==BASETYPE_POWER)
				SDLFILESRFS::inst().get("image/power.gif").Draw(systemmain->screen,&yr,&ra,systemmain->screen->h);
		}
		//プレイヤー名
		{
			ss.str("");
			ss << player[i].name;
			systemmain->bmpfontred.draw(
				systemmain->screen,
				45+(142*i),
				235,
				ss.str().c_str(),
				systemmain->screen->h);
		}
		//使ったお金
		{
			char str[256];
			sprintf(str,"SPEND %5d",player[i].spendmoney);
			systemmain->bmpfont.draw(
				systemmain->screen,
				45+(142*i),
				210,
				str,
				systemmain->screen->h);
		}
		//倒した数
		{
			char str[256];
			sprintf(str,"KILLES%5d",player[i].killed);
			systemmain->bmpfont.draw(
				systemmain->screen,
				45+(142*i),
				195,
				str,
				systemmain->screen->h);
		}
		//倒された数
		{
			char str[256];
			sprintf(str,"KILLED%5d",player[i].bekilled);
			systemmain->bmpfont.draw(
				systemmain->screen,
				45+(142*i),
				180,
				str,
				systemmain->screen->h);
		}
		//勝敗
		{
			char str[256];
			sprintf(str,"RANK  %5d",player[i].rank);
			systemmain->bmpfont.draw(
				systemmain->screen,
				45+(142*i),
				105,
				str,
				systemmain->screen->h);
		}
		//Readyのこと
		{
			ss.str("");
			if(player[i].isReady) ss << "READY...";
			else ss << "";
			systemmain->bmpfont.draw(
				systemmain->screen,
				45+(142*i),
				85,
				ss.str().c_str(),
				systemmain->screen->h);
		}
		//プレイヤー番号
		//(+5,+35)
		{
			SDL_Rect tems = {0,20*i,34,20};
			SDL_Rect temp = {60+(142*i),266,0,0};
			space::SDLFILESRFS::inst().get("image/player.gif").Draw(systemmain->screen,&temp,&tems,systemmain->screen->h);
		}
	}

	//readybutton_rect.Draw(systemmain->screen,255,255,0,255,systemmain->screen->h);

}

void Result::update(){
	
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
		if(counter-FinishBegin_cnt > 60){
			//次へ。
			isNeed = false;
			systemmain->setGameSetting();
			//フェードイン
			systemmain->setFadeIn(60);
		}
	}
	//それ以外はいつも通り。
	else{
		//全プレイヤーについて
		for(unsigned int i=0;i<player.size();i++){
			//左クリックが押された
			if( player[i].pushtime_left == 1 ){
				//ボタンが押されたとき
				if(readybutton_rect.Col_Dot(player[i].input->mouse.getleft().getPushpos())){
					player[i].isReady = !player[i].isReady;
					if(player[i].isReady)systemmain->sound_enter.play();
					else systemmain->sound_cancel.play();
				}
			}
		}

		//全員おしたっぽいなら
		bool allover=true;
		for(unsigned int i=0;i<player.size();i++){if(!player[i].isReady)allover=false;}
		if(allover){
			//おわる
			FinishBegin_cnt = counter;
			//フェードアウト
			systemmain->setFadeOut(60);
		}
	}

	//カウンタ
	counter++;

	return;
}

}//end of namespace space
