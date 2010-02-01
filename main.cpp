#include "systemstate.h"

// ƒƒCƒ“ŠÖ”
int main(int argc, char* argv[])
{
	space::ProgramSystemMain sysmain;

	try{
		sysmain.initialize();
	    sysmain.mainloop();
		sysmain.finalize();
	}
	catch(char *msg){
		std::ofstream srm("gameerror.log",std::ios::out|std::ios::app);
		srm <<  msg << "::" << time(NULL) << std::endl;
	}

    return 0;
}
