#ifndef _PROGRAMSYSTEMMAIN_H_
#define _PROGRAMSYSTEMMAIN_H_

#include "input.h"
#include "sound.h"

namespace space
{
	//全システムメイン
	class SystemState;
	class GameMain;
	class ProgramSystemMain : public DrawAndUpdate
	{
	public:
		//バージョン
		unsigned int version;

		//場面
		std::list<SystemState*> nowstates;
		//入力の実体
		Input input[16];
		//画面のサーフェイス
		SDL_Surface* screen;
		//FPS設定用
		FPScontrol fpscontrol;
		//ビットマップフォント
		BMPFONT bmpfont;
		BMPFONT bmpfontred;
		//音
		SDLMIXERCHUNK sound_move;//移動
		SDLMIXERCHUNK sound_enter;//決定
		SDLMIXERCHUNK sound_cancel;//キャンセル
		SDLMIXERCHUNK sound_setting;//セッティング
		SDLMIXERCHUNK sound_bgm;//bgm設定のようなもの
		SDLMIXERCHUNK sound_warning;//警告音

	public:

		ProgramSystemMain();
		~ProgramSystemMain();

		void setTitle();
		void setGameSetting();
		void setBaseSetting();
		void setFieldSetting();
		void setGameMain();
		void setResult(GameMain *gm);
		void setFadeOut(unsigned int _count);
		void setFadeIn(unsigned int _count);
		void setGameDemo();
		void setTextInput(SystemState* ret, std::string exp, std::string* strret);

		void draw();
		void update();
		void after_all();
		void DisplayReload();

		int  initialize();
		void mainloop();
		void finalize();

		void ending();
		void taskclear();

	};

}

#endif
