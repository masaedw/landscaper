#ifndef _GAMENETWORK_H_
#define _GAMENETWORK_H_

#include "systemstate.h"
#include "network.h"

#include "gamemain.h"
#include "input.h"

namespace space{
	//接続設定
	class GameNetwork : public SystemState
	{
	protected:

	public:
		//クライアントなら1,サーバーなら0
		bool isClient;

		//==========
		//入力関連
		//==========
		std::vector<Input*> inputs;
		//押した時間
		struct Pushtime{
			unsigned short pushtime_left;
		        unsigned short pull_left;
			unsigned short pushtime_middle;
		        unsigned short pull_middle;
			unsigned short pushtime_right;
		        unsigned short pull_right;
			//key=ボタン番号 , value=押されたフレーム数
			std::map<unsigned short,unsigned short> pushtime_key;
			//放されたキー
			std::list<unsigned short> pull_key;
			//マウスの位置が変わった
			Matrix21<int> bpos;
			bool mouseIsMove;
			
			void clear(){
				pushtime_left = 0;
				pull_left = 0;
				pushtime_middle = 0;
				pull_middle = 0;
				pushtime_right = 0;
				pull_right = 0;
				pushtime_key.clear();
				pull_key.clear();
				bpos=Matrix21<int>(2000,2000);
				mouseIsMove = false;
			}
			Pushtime(){clear();}
		};
		std::vector<Pushtime> pushtimes;
		//自分の番号
		unsigned int my_number;

		//===================
		//ネットワーク関連
		//===================
		TCPsocket server_sock;
		std::vector<TCPsocket> client_sock;
		SDLNet_SocketSet sock_set;
		
		//========
		//そのた
		//========
		//終わりフラグ兼カウンタ
		unsigned int FinishBegin_cnt;

	public:
		GameNetwork(
			ProgramSystemMain *_sys,
			std::vector<Input*> &_in, unsigned int _mynum,
			bool _iscl, std::vector<TCPsocket> &_cl, TCPsocket &_sv);
		virtual ~GameNetwork();

		void draw();
		void update();
		void after_all();
		
		//入力時間のこと
		void setpushtime(unsigned short num);
		
		//コマンド受け取り
		bool recvCommandC(unsigned int i,unsigned short pnum,unsigned int size);
		bool sendCommandC(unsigned int i,unsigned short pnum);
		bool genCommandC(RawData &data,unsigned short pnum);
		bool decompCommandC(RawData &rdata,unsigned short pnum);

		//接続の終了
		void ending();

	};
}

#endif
