#ifndef _FIELDSETTING_H_
#define _FIELDSETTING_H_

#include "systemstate.h"
#include "input.h"

#include "gamesetting.h"

namespace space{

class FieldSetting : public SystemState
{
public:
	FieldSetting(ProgramSystemMain* sys);
	virtual ~FieldSetting(void);

	virtual void draw();
	virtual void update();
};

}// end of namespace space

#endif
