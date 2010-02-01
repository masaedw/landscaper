#ifndef _GAMEDEMO_H_
#define _GAMEDEMO_H_

#include "systemstate.h"
#include "input.h"

#include "effectstate.h"
#include "gamemain.h"

namespace space{

class GameDemo : public SystemState
{
public:
	//flag
	bool endflg;

	//repfile
	std::fstream repfile;

	//終わりフラグ兼カウンタ
	unsigned int FinishBegin_cnt;

	GameMain* gamemain;
	StateDisplayDemo* dispdemo;

public:
	GameDemo(ProgramSystemMain *_sys,std::string _rep);
	virtual ~GameDemo(void);

	void init();

	virtual void draw();
	virtual void update();
};

}// end of namespace space

#endif
