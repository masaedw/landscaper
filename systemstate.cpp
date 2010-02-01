#include "systemstate.h"

namespace space
{
	//===========================
	//派生アブストラクト
	//ゲーム中の場面に関するもの
	//===========================
	SystemState::SystemState(ProgramSystemMain *_system, STATE _st)
		: systemstate(_st), systemmain(_system), isNeed(true)
	{counter = 0;}
	SystemState::~SystemState(){}

	SystemState::STATE SystemState::getSystemState() const {return systemstate;}
	void SystemState::DisplayReload(){}

}
