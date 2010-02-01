#ifndef _BASESETTING_H_
#define _BASESETTING_H_

#include "systemstate.h"
#include "input.h"

#include "gamesetting.h"
#include "gamemain.h"

namespace space{

class BaseSetting : public SystemState
{
public:

	//終わりフラグ兼カウンタ
	unsigned int FinishBegin_cnt;

	struct Player{
		//==========
		//入力関連
		//==========
		const Input *input;
		unsigned short pushtime_left;
		unsigned short pushtime_middle;
		unsigned short pushtime_right;
		//key=ボタン番号 , value=押されたフレーム数
		std::map<unsigned short,unsigned short> pushtime_key;

		//========
		//設定項目
		//========
		//名前
		std::string name;
		//拠点タイプ(マイナスで未選択)
		int typeofBase;

	};
	std::vector<Player> player;
	unsigned int player_ctrl;
		
	std::vector< Circle2<double> > basebutton_circle;
	std::vector<const SDLSURFACE*> basebutton_surface;
	std::vector<std::string> basebutton_name;
	
	unsigned int randomseed;

public:
	BaseSetting(ProgramSystemMain *_sys,std::vector<BaseSetting::Player> pl,unsigned int m,unsigned int _rds);
	virtual ~BaseSetting(void);

	virtual void draw();
	virtual void update();
};

}// end of namespace space

#endif
