#ifndef _EFFECTSTATE_H_
#define _EFFECTSTATE_H_

#include "systemstate.h"
#include "input.h"

namespace space{

//フェードアウト
class StateFadeOut : public SystemState
{
public:
	unsigned int maxcount;
public:
	StateFadeOut(ProgramSystemMain* sys,unsigned int _count);
	virtual ~StateFadeOut(void);

	virtual void draw();
	virtual void update();
};

//フェードイン
class StateFadeIn : public SystemState
{
public:
	unsigned int maxcount;
public:
	StateFadeIn(ProgramSystemMain* sys,unsigned int _count);
	virtual ~StateFadeIn(void);

	virtual void draw();
	virtual void update();
};

//デモプレイ表示
class StateDisplayDemo : public SystemState{
public:
	StateDisplayDemo(ProgramSystemMain* sys);
	virtual ~StateDisplayDemo(void){}

	virtual void draw();
	virtual void update();
};

//ダミー
class StateDummy : public SystemState{
public:
	StateDummy(ProgramSystemMain* sys) : SystemState(sys,STATEDUMMY){isNeed=false;}
	virtual ~StateDummy(void){}

	virtual void draw(){}
	virtual void update(){}
};

}// end of namespace space

#endif
