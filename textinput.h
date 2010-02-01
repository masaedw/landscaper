#ifndef _TEXTINPUT_H_
#define _TEXTINPUT_H_

#include "systemstate.h"


namespace space
{
	//接続設定
	class TextInput : public SystemState
	{
	public:
		//入力する文字列の説明
		std::string explanation;
		//戻る先のステート
		SystemState *state_return;
		//戻す文字列のアドレス
		std::string *string_return;

		//
		int sc1,sc2,sc3,sc4;

		//背景の位置
		WinRect winpos;

		//スペース、バックスペース、決定
		WinRect sprect,bsrect,retrect;

		//==========
		//入力関連
		//==========
		const Input *input;
		unsigned short pushtime_left;
		unsigned short pushtime_middle;
		unsigned short pushtime_right;
		//key=ボタン番号 , value=押されたフレーム数
		std::map<unsigned short,unsigned short> pushtime_key;

		//文字列
		std::vector<char> dispchar;

	public:
		TextInput(ProgramSystemMain *_sys, SystemState* ret, std::string exp, std::string* strret);
		virtual ~TextInput(){}

		void draw();
		void update();
		void after_all();
	};
	
}


#endif