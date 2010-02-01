#ifndef _GAMESETTING_H_
#define _GAMESETTING_H_

#include "systemstate.h"
#include "input.h"

#include "gamemain.h"
#include "gamenetwork.h"
#include "basesetting.h"

namespace space
{
	//接続設定
	class GameSetting : public SystemState
	{
	protected:

	public:
		//クライアントならtrue,サーバーならfalse
		bool isClient;
		//ランダムシード
		unsigned int randomseed;
		//自分のプレイヤーナンバー
		unsigned int my_number;
		std::string my_name;

		//==========
		//入力関連
		//==========
		const Input *input;
		unsigned short pushtime_left;
		unsigned short pushtime_middle;
		unsigned short pushtime_right;
		//key=ボタン番号 , value=押されたフレーム数
		std::map<unsigned short,unsigned short> pushtime_key;
		//前のマウス位置
		Matrix21<int> bmousepos;
		bool mousemove;

		//値を貰うやつ
		std::string return_string;
		bool afterreturn;

		//ネットワーク関連
		TCPsocket server_sock;
		std::vector<TCPsocket> client_sock;
		SDLNet_SocketSet sock_set;
		IPaddress ip;
		std::vector<std::string> names;

		//その他情報
		unsigned int player_num;
		std::string hostname;
		unsigned short port;

		//画面遷移
		int state;
		int selectitem;

	public:
		GameSetting(ProgramSystemMain *_sys);
		virtual ~GameSetting();

		void draw();
		void update();
		void after_all();

		void setnextstate(int);
	};
	
}

#endif
