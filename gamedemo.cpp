#include "gamedemo.h"

namespace space{

void GameDemo::init(){
	//ヘッダ形式
	//バージョン(4) + ランダムシード(4) + プレイ人数(1) + 拠点種別(プレイ人数) + 名前(プレイ人数*(1+名前の長さ))

	//プレイヤーども
	std::vector<GameMain::Player> pls;
	
	//まずバージョン
	unsigned int version;
	repfile.read((char*)&version,4);
	//異なったら死ぬ。
	//if(version!=systemmain->version)endflg = true;

	//乱数の種
	unsigned int randomseed;
	repfile.read((char*)&randomseed,4);
	//次人数
	unsigned char playersize;
	repfile.read((char*)&playersize,1);
	//人数分だけ拠点種別
	for(unsigned int i=0;i<playersize;i++){
		GameMain::Player pl;
		unsigned int tob=0;
		repfile.read((char*)&tob,1);
		pl.typeofBase = tob;
		//ついでに入力を渡しておく。
		systemmain->input[i+1].clear();
		pl.input = &systemmain->input[i+1];
		pls.push_back(pl);
	}
	//名前
	for(unsigned int i=0;i<playersize;i++){
		unsigned char size;
		char str[256];
		repfile.read((char*)&size,1);
		repfile.read(str,size);
		str[size]='\0';
		pls[i].name = str;
	}

	//ゲームメイン作成
	gamemain = new GameMain(systemmain,pls,0,randomseed,false);
	systemmain->nowstates.push_back(gamemain);
	dispdemo = new StateDisplayDemo(systemmain);
	systemmain->nowstates.push_back(dispdemo);
}

GameDemo::GameDemo(ProgramSystemMain *_sys,std::string _rep)
	: SystemState(_sys,GAMEDEMO)
{
	repfile.open(_rep.c_str(),std::ios::in|std::ios::binary);
	//どうでもよさそうなカウンタ
	FinishBegin_cnt = UINT_MAX;
	//フラグ
	endflg = false;
}

GameDemo::~GameDemo(void)
{
	repfile.close();
}

void GameDemo::draw(){
	//なにもしちゃいけません。
}

void GameDemo::update(){

	if(counter==0)init();

	if(!endflg){

		//===========
		//入力のこと(リプレイファイルから読み込む)
		//==========
		//リプレイバイナリ形式 = フレーム区切り(1) + ( データサイズ(1)+マウス地点(2+2)+追加チャンク(データサイズ-4) ) (人数)
		unsigned char pnum,dsize;
		//人数分
		for(unsigned i=0;i<gamemain->player.size();i++)
		{
			//playernum
			pnum = i;
			//データサイズ
			repfile.read((char*)&dsize,1);

			//データが5で割れないとき,終了フラグを立てる。
			if(dsize%5!=0){endflg=true;break;}
			//データがある場合は読み込む
			for(int i=0;i<dsize/5;i++){
				unsigned char button;
				Matrix21<int> bpos(0,0);
				//ボタン押したorひいたデータ
				repfile.read((char*)&button,1);
				//その位置
				repfile.read((char*)&bpos.x,2);
				repfile.read((char*)&bpos.y,2);
				//ボタンによって変わるよ
				//0,1,2(左、真ん中、右)押した
				//4,5,6(左、真ん中、右)引いた
				//3(マウスの位置)
				switch(button){
					case 0:
						systemmain->input[pnum+1].mouse.setleft().push(bpos);
						break;
					case 1:
						systemmain->input[pnum+1].mouse.setmiddle().push(bpos);
						break;
					case 2:
						systemmain->input[pnum+1].mouse.setright().push(bpos);
						break;
					case 3:
						systemmain->input[pnum+1].mouse.setpos(bpos);
						break;
					case 4:
						systemmain->input[pnum+1].mouse.setleft().pull(bpos);
						break;
					case 5:
						systemmain->input[pnum+1].mouse.setmiddle().pull(bpos);
						break;
					case 6:
						systemmain->input[pnum+1].mouse.setright().pull(bpos);
						break;
				}
			}
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
			gamemain->isNeed = false;
			dispdemo->isNeed = false;
			systemmain->setTitle();
			//フェードイン
			systemmain->setFadeIn(30);
		}
	}
	//それ以外はいつも通り。
	else{
		static bool push=false;
		//なんか押されたら,または勝負が決着してたら
		if( systemmain->input[0].mouse.getleft().isPush() || systemmain->input[0].keyboard.ispush(SDLK_RETURN) || (gamemain->deadcount+1>=gamemain->player.size()) || repfile.eof() || endflg){
			//おわる
			FinishBegin_cnt = counter;
			//フェードアウト
			systemmain->setFadeOut(30);
			systemmain->sound_bgm.fadeout(480);
		}
		if( systemmain->input[0].mouse.getright().isPush()){
			if(!push){
				gamemain->player_ctrl++;
				if(gamemain->player_ctrl >= gamemain->player.size())gamemain->player_ctrl=0;
				gamemain->changePlayerCtrl(gamemain->player_ctrl);
				push = true;
			}
		}
		else{push = false;}
	}

	//カウンタ
	counter++;

	return;
}

}//end of namespace space
