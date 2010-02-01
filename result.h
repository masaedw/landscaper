#ifndef _RESULT_H_
#define _RESULT_H_

#include "systemstate.h"
#include "input.h"

#include "gamesetting.h"
#include "gamemain.h"

namespace space{

class Result : public SystemState
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

		//決定ボタンのこと。
		bool isReady;

		//========
		//結果
		//========
		std::string name;
		int rank;
		int spendmoney;
		int killed;
		int bekilled;
		int typeofBase;
	};
	std::vector<Player> player;
	unsigned int player_ctrl;
	std::vector<int> rank;
		
	WinRect readybutton_rect;

public:
	Result(ProgramSystemMain* sys,GameMain* gm);
	virtual ~Result(void);

	virtual void draw();
	virtual void update();
};

}// end of namespace space

#endif
