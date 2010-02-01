#include "programsystemmain.h"
#include "systemstate.h"

#include "gamemain.h"
#include "gamesetting.h"
#include "title.h"
#include "result.h"
#include "fieldsetting.h"
#include "basesetting.h"
#include "effectstate.h"
#include "gamedemo.h"
#include "textinput.h"

namespace space
{
	//=================
	//全システムメイン
	//=================

	//簡単作成メソッド
	//ゲームメイン
	void ProgramSystemMain::setGameMain(){
		std::vector<GameMain::Player> _pls;
		
		GameMain::Player _pl;
		
		for(int i=0;i<16;i++)
		{
			input[i] = Input();
			_pl.input = &input[i];
			_pls.push_back(_pl);
		}
		nowstates.push_back(new GameMain(this,_pls,0,(unsigned int)time(NULL)) );
	}
	//ローカルセッティング
	void ProgramSystemMain::setGameSetting(){
		nowstates.push_back(new GameSetting(this));
	}
	//拠点設定
	void ProgramSystemMain::setBaseSetting(){
		std::vector<BaseSetting::Player> _pls;
		
		BaseSetting::Player _pl;
		
		for(int i=0;i<16;i++)
		{
			input[i] = Input();
			_pl.input = &input[i];
			_pls.push_back(_pl);
		}
		nowstates.push_back( new BaseSetting(this,_pls,0,(unsigned int)time(NULL)) );
	}
	//ステージ設定
	void ProgramSystemMain::setFieldSetting(){
		nowstates.push_back(new FieldSetting(this));
	}
	//タイトル
	void ProgramSystemMain::setTitle(){
		nowstates.push_back(new Title(this));
	}
	//リザルト
	void ProgramSystemMain::setResult(GameMain* gm){
		nowstates.push_back(new Result(this,gm));
	}
	//フェードアウト
	void ProgramSystemMain::setFadeOut(unsigned int _count){
		nowstates.push_back(new StateFadeOut(this,_count));
	}
	//フェードイン
	void ProgramSystemMain::setFadeIn(unsigned int _count){
		nowstates.push_back(new StateFadeIn(this,_count));
	}
	//デモ
	void ProgramSystemMain::setGameDemo(){
		nowstates.push_back(new GameDemo(this,"replay/demo.rep"));
	}
	//文字列入力
	void ProgramSystemMain::setTextInput(SystemState* ret, std::string exp, std::string* strret){
		nowstates.push_back(new TextInput(this,ret,exp,strret));
	}

	ProgramSystemMain::ProgramSystemMain() : DrawAndUpdate()
	{}
	ProgramSystemMain::~ProgramSystemMain()
	{
		ending();
	}

	void ProgramSystemMain::taskclear(){
		std::list<SystemState*>::iterator it=nowstates.begin();
		for(;it!=nowstates.end();it++) delete *it;
		nowstates.clear();
	}

	void ProgramSystemMain::ending(){
		
		sound_move.free();
		sound_enter.free();
		sound_cancel.free();
		sound_setting.free();
		sound_bgm.free();
		sound_warning.free();

		taskclear();
	}

	void ProgramSystemMain::draw()
	{
		//これ一行で書けたんかい・・・末恐ろしいC++
		//draw
		std::for_each(nowstates.begin(),nowstates.end(),std::mem_fun(&DrawAndUpdate::draw));
	}
	void ProgramSystemMain::update()
	{
		//update
		std::for_each(nowstates.begin(),nowstates.end(),std::mem_fun(&DrawAndUpdate::update));
	}
	void ProgramSystemMain::after_all()
	{
		//after_update
		std::for_each(nowstates.begin(),nowstates.end(),std::mem_fun(&DrawAndUpdate::after_all));
	}

	void ProgramSystemMain::DisplayReload()
	{
		//画像読み直し。ということで一旦全部クリア。
		GLFILETEXS::inst().clear();
		//リロード
		std::for_each(nowstates.begin(),nowstates.end(),std::mem_fun(&SystemState::DisplayReload));
	}

	void ProgramSystemMain::mainloop()
	{

		while(1){
			//===========================
			// すべてのイベントを処理する
			//===========================
			SDL_Event event;
			Matrix21<int> _temp;
			while (SDL_PollEvent(&event)) {
				switch (event.type)
				{
				//画面サイズ変更
				case SDL_VIDEORESIZE:
					SDL_FreeSurface(screen);
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, 32,SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RLEACCEL | SDL_HWACCEL);
					DisplayReload();
					break;
				//キー入力
				case SDL_KEYDOWN:
					if(event.key.keysym.sym == SDLK_ESCAPE) { return; }
					if(event.key.keysym.sym == SDLK_RETURN && input[0].keyboard.ispush(SDLK_RALT)) {
						unsigned long flags=screen->flags,w=screen->w,h=screen->h;
						SDL_FreeSurface(screen);
						if(flags & SDL_FULLSCREEN) screen=SDL_SetVideoMode(w,h,32,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RLEACCEL|SDL_HWACCEL);
						else screen=SDL_SetVideoMode(w,h,32,SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_RLEACCEL|SDL_HWACCEL|SDL_FULLSCREEN);
						DisplayReload();
						break;
					}
					input[0].keyboard.pushkey(event.key.keysym.sym);
					break;
				case SDL_KEYUP:
					input[0].keyboard.pullkey(event.key.keysym.sym);
					break;
				//マウス入力
				case SDL_MOUSEMOTION:
					input[0].mouse.setpos(Matrix21<int>(
						event.motion.x,
						screen->h-event.motion.y)
						);
					break;
				case SDL_MOUSEBUTTONDOWN:
					_temp = Matrix21<int>(
						event.button.x,
						screen->h-event.button.y
						);
					switch(event.button.button)
					{
					case SDL_BUTTON_LEFT:
						input[0].mouse.setleft().push(_temp);
						break;
					case SDL_BUTTON_MIDDLE:
						input[0].mouse.setmiddle().push(_temp);
						break;
					case SDL_BUTTON_RIGHT:
						input[0].mouse.setright().push(_temp);
						break;
					}
					break;
				case SDL_MOUSEBUTTONUP:
					_temp = Matrix21<int>(
						event.button.x,
						screen->h-event.button.y
						);
					switch(event.button.button)
					{
					case SDL_BUTTON_LEFT:
						input[0].mouse.setleft().pull(_temp);
						break;
					case SDL_BUTTON_MIDDLE:
						input[0].mouse.setmiddle().pull(_temp);
						break;
					case SDL_BUTTON_RIGHT:
						input[0].mouse.setright().pull(_temp);
						break;
					}
					break;
				//ジョイスティック入力(今回は無視)
				case SDL_JOYAXISMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
					break;
				//終わり
				case SDL_QUIT:
					return;
				//その他
				default:
					break;
				}
				
			}

			//=====
			//実行
			//=====
			update();

			//==========
			//描画(SDL)
			//==========
			//static int dc=0;
			//dc = (dc>=4) ? 0 : dc+1;
			//if(dc==0){
			if(!fpscontrol.onlyUpdate()){
				draw();
				{
					char _str[30];
					sprintf(_str,"FPS::%5.2lf",fpscontrol.getFPS());
					stringRGBA(screen, screen->w-81, screen->h-9, _str, 0xff, 0xff, 0xff, 0xff);
				}
				SDL_Flip(screen);
			}
			fpscontrol.delay();
			
			//==========
			//全部の後。
			//==========
			after_all();

			//===============
			//いらないの削除
			//===============
			std::list<SystemState*>::iterator it=nowstates.begin();
			for(;it!=nowstates.end();)
			{
				if(!(*it)->isNeed)
				{
					delete *it;
					it = nowstates.erase(it);
				}
				else it++;
			}

		}
	}


	// 初期化する
	// 成功したときは0を、失敗したときは-1を返す
	int ProgramSystemMain::initialize()
	{
		//===================
		//バージョン
		//===================
		version =
			0 * (256*256*256) +
			0 * (256*256) +
			3 * (256) +
			1;

		//==================
		//全部の初期化
		//==================
		//SDLの初期化
		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			char s[256];
			sprintf(s, "SDL_Init Failed!!! ：%s\n", SDL_GetError());
			throw s;
		}
		//SDL_TTFの初期化
		if (TTF_Init() < 0){
			char s[256];
			sprintf(s, "TTF_Init Failed!!! ：%s\n", TTF_GetError());
			throw s;
		}
		//SDL_Netの初期化
		if (SDLNet_Init() < 0){
			char s[256];
			sprintf(s, "SDLNet_Init Failed!!! ：%s\n", SDLNet_GetError());
			throw s;
		}
		//SDL_imageの初期化…だが古いと出来ない
		//if (IMG_Init(0xffffffff) < 0){
		//	return -1;
		//}
		//SDL_mixerの初期化。これはいつも通りのやつ。
		if (Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024) < 0){
			char s[256];
			sprintf(s, "Mix_OpenAudio Failed!!! ：%s\n", Mix_GetError());
			throw s;
		}
		//SDL_mixerの初期化…だがやっぱり古いと出来ない
		//if (Mix_Init(0xf) < 0){
		//	return -1;
		//}

		//==========================
		//SDLの画面初期化の前設定
		//==========================
		//タイトルを決定
		char str[256];
		sprintf(str,"Landscaper  Ver. %d.%d.%d.%d",(version/(256*256*256))%256,(version/(256*256))%256,(version/(256))%256,version%256);
		SDL_WM_SetCaption(str, NULL);
		SDL_WM_SetIcon(IMG_Load("image/icon.bmp"),NULL);

		//============================
		//画面の初期化
		//=============================
		//画面のサーフェイスを取得する
		screen = SDL_SetVideoMode(
			640,
			480,
			32,
			SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RLEACCEL | SDL_HWACCEL);
		if (screen == NULL) {
			char s[256];
			sprintf(s, "SDL_SetVideoMode Failed!!! ：%s\n", SDL_GetError());
			throw s;
		}

		//============================
		//SDL最初の設定
		//===========================
		//キーリピート無効化
		SDL_EnableKeyRepeat(0,SDL_DEFAULT_REPEAT_INTERVAL);


		//===========================
		//最初の設定。
		//==========================
		bmpfont.open("image/game_pas_small.gif");
		bmpfont.fontsetting(11,15,-31);
		bmpfontred.open("image/game_pas_small_r.gif");
		bmpfontred.fontsetting(11,15,-31);

		//タイトル
		setTitle();
		//static std::string ret;
		//setTextInput(new Title(this),"this is a test",&ret);

		//おとがいっぱい！
		Mix_AllocateChannels(16);
		sound_move.load("sound/move.wav");
		sound_move.set_channel(0);
		sound_move.setChunkVolume(80);
		sound_enter.load("sound/ok.wav");
		sound_enter.set_channel(1);
		sound_enter.setChunkVolume(64);
		sound_cancel.load("sound/cancel.wav");
		sound_cancel.set_channel(2);
		sound_cancel.setChunkVolume(112);
		sound_setting.load("sound/setting.wav");
		sound_setting.set_channel(3);
		sound_setting.setChunkVolume(48);
		sound_bgm.load("sound/bgm.wav");
		sound_bgm.set_channel(4);
		sound_bgm.setChunkVolume(108);
		sound_warning.load("sound/warn.wav");
		sound_warning.set_channel(5);
		sound_warning.setChunkVolume(64);

		return 0;
	}

	void ProgramSystemMain::finalize()
	{
		//後始末
		ending();

		//=================
		//SDLを終わらせる
		//=================
		//Mix_Quit();
		Mix_CloseAudio();
		//IMG_Quit();
		SDLNet_Quit();
		TTF_Quit();
	    SDL_Quit();
	}

}
