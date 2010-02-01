#include "gamenetwork.h"

namespace space{

	//接続設定
	GameNetwork::GameNetwork(
		ProgramSystemMain *_sys,
		std::vector<Input*> &_in, unsigned int _mynum,
		bool _iscl, std::vector<TCPsocket> &_cl, TCPsocket &_sv)
		: SystemState(_sys,GAMENETWORK)
	{
		//入力設定
		inputs = _in;
		my_number = _mynum;
		for(unsigned int i=0;i<inputs.size();i++){
			inputs[i]->clear();
			pushtimes.push_back(Pushtime());
		}
		//ソケット設定
		isClient = _iscl;
		server_sock = _sv;
		client_sock = _cl;
		//ソケットセットを作る
		sock_set = SDLNet_AllocSocketSet(client_sock.size()+1);
		SDLNet_TCP_AddSocket(sock_set,server_sock);
		for(unsigned int i=0;i<client_sock.size();i++){
			SDLNet_TCP_AddSocket(sock_set,client_sock[i]);
		}

		//カウンタ
		FinishBegin_cnt = UINT_MAX;
	}

	//接続終了処理
	void GameNetwork::ending(){
		//ソケットセット開放
		if(sock_set!=NULL){
			SDLNet_FreeSocketSet(sock_set);
			sock_set=NULL;
		}
		//クライアントソケット全部開放
		for(unsigned int i=0;i<client_sock.size();i++){
			SDLNet_TCP_Close(client_sock[i]);
		}
		client_sock.clear();
		//サーバーソケット開放
		if(server_sock!=NULL){
			SDLNet_TCP_Close(server_sock);
			server_sock = NULL;
		}
	}

	GameNetwork::~GameNetwork(){
		//ここで何か残っている場合、何かが起こってしまったということである。
		if(client_sock.size()>0)
		{
			//今度こそ閉じる。
			ending();
		}
	}

	//drawなし
	void GameNetwork::draw()
	{
		SDL_Rect _rect= {systemmain->screen->w-80,systemmain->screen->h-20,0,0};
		//draw_str("It Works! (Network)",systemmain->screen,&_rect,systemmain->screen->h);
	}

	//コマンド送信
	bool GameNetwork::genCommandC(RawData &data, unsigned short pnum){
		//キーボード(いまは0で。)
		unsigned short keys = 0;
		//data.write(&keys,2);
		
		//キーボードの押されたり放されたりした分だけ。
		for(unsigned int j=0;j<keys;j++){
		  //まぁここにはなにかかきたいよね
		}
		//マウスの分
		unsigned char mouses = 0;
		if(pushtimes[pnum].pushtime_left==1  || pushtimes[pnum].pull_left  !=0)mouses++;
		if(pushtimes[pnum].pushtime_middle==1|| pushtimes[pnum].pull_middle!=0)mouses++;
		if(pushtimes[pnum].pushtime_right==1 || pushtimes[pnum].pull_right !=0)mouses++;
		if(pushtimes[pnum].mouseIsMove) mouses+=128;
		data.write(&mouses,1);

		//はじめにマウスの座標
		if(pushtimes[pnum].mouseIsMove){
			Matrix21<short> _mousepos(inputs[pnum]->mouse.getpos().x, inputs[pnum]->mouse.getpos().y);
			int zippos = _mousepos.x*4096 + _mousepos.y;
			data.write(&zippos,3);
		}

		//マウスで変化の起こった分を送る
		unsigned char mousestate;
		//left
		if(pushtimes[pnum].pushtime_left==1){
		  mousestate = 0 + 1*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getleft().getPushpos().x,inputs[pnum]->mouse.getleft().getPushpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}
		else if(pushtimes[pnum].pull_left !=0){
		  mousestate = 0 + 0*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getleft().getPullpos().x,inputs[pnum]->mouse.getleft().getPullpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}
		//middle
		if(pushtimes[pnum].pushtime_middle==1){
		  mousestate = 1 + 1*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getmiddle().getPushpos().x,inputs[pnum]->mouse.getmiddle().getPushpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}
		else if(pushtimes[pnum].pull_middle !=0){
		  mousestate = 1 + 0*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getmiddle().getPullpos().x,inputs[pnum]->mouse.getmiddle().getPullpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}
		//left
		if(pushtimes[pnum].pushtime_right==1){
		  mousestate = 2 + 1*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getright().getPushpos().x,inputs[pnum]->mouse.getright().getPushpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}
		else if(pushtimes[pnum].pull_right !=0){
		  mousestate = 2 + 0*4;
		  data.write(&mousestate,1);
		  Matrix21<short> _pos(inputs[pnum]->mouse.getright().getPullpos().x,inputs[pnum]->mouse.getright().getPullpos().y);
		  data.write(&_pos.x,2);
		  data.write(&_pos.y,2);
		}


		return true;
	}

	//コマンド送信
	bool GameNetwork::sendCommandC(unsigned int i,unsigned short pnum){
		
		RawData rdata;
		if(!genCommandC(rdata,pnum))return false;
		unsigned int com = 'c' + (256)*rdata.size() + (256*256*256)*(pnum);
		
		RawData senddata;
		senddata.write(&com,4);
		senddata.write(&rdata[0],rdata.size());
		int sendsize = SDLNet_TCP_Send(client_sock[i],&senddata[0],senddata.size());
		if(sendsize != senddata.size()) return false;

		return true;
	}

	//コマンド受け取り
	bool GameNetwork::decompCommandC(RawData &rdata,unsigned short pnum){
		//キーボード
		unsigned short keys=0;
		//rdata.read((void*)&keys,2);

		//キーボードの押されたり放されたりした分だけ。
		for(unsigned int j=0;j<keys;j++){
			//キーボード情報を受け取る
			unsigned short keystate;
			rdata.read((void*)&keystate,2);
			unsigned short keynum = keystate % 512;
			unsigned short isPush = keystate / 512;
			//押された
			if(isPush!=0){
				inputs[pnum]->keyboard.pushkey(keynum);
			}
			//引かれた
			else{
				inputs[pnum]->keyboard.pullkey(keynum);
			}
		}
		//マウスの分
		unsigned char mouses;
		rdata.read((void*)&mouses,1);

		//最初にマウスの座標
		if(mouses/128==1){
			int zippos = 0;
			rdata.read((void*)&zippos,3);
			Matrix21<int> mousepos(zippos/4096,zippos%4096);
			inputs[pnum]->mouse.setpos(mousepos);
			mouses %= 128;
		}

		//マウスの押されたり放されたりした分だけ。
		for(unsigned int j=0;j<mouses;j++){
			//マウス情報を受け取る
			unsigned char mousestate;
			rdata.read((void*)&mousestate,1);
			unsigned char keynum = mousestate % 4;
			unsigned char isPush = mousestate / 4;
			Matrix21<short> _pos;
			rdata.read((void*)&_pos.x,2);
			rdata.read((void*)&_pos.y,2);
			Matrix21<int> pos;
			pos.x=_pos.x; pos.y=_pos.y;
			//押された
			if(isPush!=0){
				switch(keynum)
				{
				case 0:
					inputs[pnum]->mouse.setleft().push(pos);
					break;
				case 1:
					inputs[pnum]->mouse.setmiddle().push(pos);
					break;
				case 2:
					inputs[pnum]->mouse.setright().push(pos);
					break;
				}
			}
			//引かれた
			else{
				switch(keynum)
				{
				case 0:
					inputs[pnum]->mouse.setleft().pull(pos);
					break;
				case 1:
					inputs[pnum]->mouse.setmiddle().pull(pos);
					break;
				case 2:
					inputs[pnum]->mouse.setright().pull(pos);
					break;
				}
			}
		}

		return true;
	}

	//コマンド受け取り
	bool GameNetwork::recvCommandC(unsigned int i,unsigned short pnum,unsigned int size){
		RawData rdata;
		rdata.assign(size);
		int recvsize = SDLNet_TCP_Recv(client_sock[i],(void*)&rdata[0],size);
		if(recvsize<0 || recvsize!=size) return false;
		if(!decompCommandC(rdata,pnum))return false;
		return true;
	}

    //入力時間
    void GameNetwork::setpushtime(unsigned short num){
		//入力のこと
		Input ctrinp = *inputs[num];
		Pushtime *ctrpush = &pushtimes[num];
		//引かれたボタンリストをクリア
		ctrpush->pull_key.clear();
		
		//mouse
		//left
		ctrpush->pull_left=0;
		if( ctrinp.mouse.getleft().isPush() ) ctrpush->pushtime_left++;
		else{
		  if(ctrpush->pushtime_left!=0)ctrpush->pull_left = 1;
		  ctrpush->pushtime_left = 0;
		}
		//middle
		ctrpush->pull_middle=0;
		if( ctrinp.mouse.getmiddle().isPush() ) ctrpush->pushtime_middle++;
		else{
		    if(ctrpush->pushtime_middle!=0)ctrpush->pull_middle = 1;
		    ctrpush->pushtime_middle = 0;
		}
		//right
		ctrpush->pull_right=0;
		if( ctrinp.mouse.getright().isPush() ) ctrpush->pushtime_right++;
		else{
		  if(ctrpush->pushtime_right!=0)ctrpush->pull_right = 1;
		  ctrpush->pushtime_right = 0;
		}
		//マウスの位置が変わったら
		if( ctrpush->bpos != ctrinp.mouse.getpos() ){
			ctrpush->bpos = ctrinp.mouse.getpos();
			ctrpush->mouseIsMove = true;
		}
		else ctrpush->mouseIsMove = false;
		
		//key
		const Keyboard::KeyMap &km = ctrinp.keyboard.getkeys();
		std::map<unsigned short,unsigned short> &now = ctrpush->pushtime_key;
		//切る
		std::map<unsigned short,unsigned short>::iterator nowit = now.begin();
		for(;nowit!=now.end();nowit++)
		{
			if( km.find(nowit->first) == km.end() ){
			  std::map<unsigned short,unsigned short>::iterator tit = nowit;
			  ctrpush->pull_key.push_back(nowit->first);
			  nowit--;now.erase(tit);
			  if(nowit==now.end())break;
			}
			else { nowit->second++; }
		}
		//追加する
		Keyboard::KeyMap::const_iterator kmit = km.begin();
		for(;kmit!=km.end();kmit++)
		{
			if( now.find(kmit->first) == now.end() ) now[kmit->first]=1;
		}	 
    }

	//update
	void GameNetwork::update()
	{
		//終わるときならカウントだけする。
		if(counter > FinishBegin_cnt){
			if(counter-FinishBegin_cnt>=60){
				ProgramSystemMain *psm=systemmain;
				std::list<SystemState*>::iterator sl=psm->nowstates.begin();
				for(;sl!=psm->nowstates.end();sl++)(*sl)->isNeed=false;
				psm->setGameSetting();
				psm->setFadeIn(60);
			}
		}
		else{

			//====================
			//ローカルの入力について
			//====================
			setpushtime(my_number);
	        
			//===========
			//ネットワーク入力のこと
			//==========

			//通信エラー検出用
			try{

			if(isClient){
				//送る
				if(!sendCommandC(0,my_number))throw "Send Error.";

				//誰の分がかえって来たのかフラグ
				std::bitset<16> whoreturns(0x00);
				//自分はセットしておく
				whoreturns.set(my_number);

				
				//全部帰ってくるまで待つ
				while(whoreturns.count() < inputs.size()){

					//ソケットの何かあった時の数字を返す
					if(SDLNet_CheckSockets(sock_set,7000) > 0)
					{
						//どうも何かしら来た感じだったら
						if(SDLNet_SocketReady(client_sock[0]))
						{
							//コマンドに関して
							char command;
							unsigned int pnum;
							unsigned int datasize;

							//受け取る。ミスったら強制終了。
							unsigned int block;
							if( SDLNet_TCP_Recv(client_sock[0],(void*)(&block),4) < 4 ){
								throw "Command Recv Error.";
							}
							//情報を抽出する。
							else{
								command = (char)block;
								datasize = (block/256)%(256*256);
								pnum = block / (256*256*256);
							}
		
							//各コマンドの処理
							//操作情報が来た
							if(command == 'c'){
								if(!recvCommandC(0,pnum,datasize))throw "Data 'c' Recv Error";
								setpushtime(pnum);
								//かえって来たよ
								whoreturns.set(pnum);
							}
							//強制終了コマンド(なぞなやつが着たら落ちる)
							//else if(command == 'k'){
							else{
								throw "Unknown Command.";
							}
						}
					}
					//タイムアウトやらかしたら終了する。
					else{
						throw "Timeout.";
					}
				}
				

			}
			else{

				//誰の分が送られて来たのかフラグ
				std::bitset<16> whoreturns(0x00);
				//自分はセットしておく
				whoreturns.set(my_number);
				
				//何か来るまで待つ
				while(whoreturns.count() < inputs.size()){

					//ソケットの何かあった時の数字を返す			
					if(SDLNet_CheckSockets(sock_set,7000) > 0)
					{
						//全クライアントに関して
						for(unsigned int i=0;i<client_sock.size();i++){
							//どうも何かしら来た感じだったら
							if(SDLNet_SocketReady(client_sock[i]))
							{
								//コマンドに関して
								char command;
								unsigned int pnum;
								unsigned int datasize;

								//受け取る。ミスったら強制終了。
								unsigned int block;
								if( SDLNet_TCP_Recv(client_sock[i],(void*)(&block),4) < 4 ){
									throw "Command Recv Error.";
								}
								//情報を抽出する。
								else{
									command = (char)block;
									datasize = (block/256)%(256*256);
									pnum = block / (256*256*256);
								}

								//各コマンドの処理
								//操作情報が来た
								if(command == 'c'){
									//コマンドを受け取る
									if(!recvCommandC(i,pnum,datasize)) throw "Data 'c' Recv Error.";
									//押す時間設定
									setpushtime(pnum);
									//かえって来たよ
									whoreturns.set(pnum);
								}
								//強制終了コマンド(謎のやつが着たら落ちる)
								//else if(command == 'k'){
								else{
									throw "Unknown Command.";}
							}
						}
					}
					//タイムアウトやらかしたら終了する。
					else{throw "Timeout.";}

				}
				

				//送る
				for(unsigned int i=0;i<client_sock.size();i++){
					RawData rdata;
					for(unsigned int j=0;j<client_sock.size();j++){
						//相手と同じ奴は送らない
						if(i!=j){
							if(!sendCommandC(i,j+1)) throw "Send Error.";
						}
						//同じ奴になったらそんときにサーバーの奴を送る
						else{
							if(!sendCommandC(i,0)) throw "Send Error.";
						}
					}
				}
				
			}

			}
			catch(char* msg){
				//ネットワークエラー記録
				std::ofstream srm("networkerror.log",std::ios::out|std::ios::app);
				srm << "(" << my_number << ")" << msg << "::" << time(NULL) << std::endl;
				//後の処理
				FinishBegin_cnt = counter;
				systemmain->setFadeOut(60);
			}
		}
		counter++;
		return;
	}

	//全部終わったあとに何かあった時の処理
	void GameNetwork::after_all()
	{
		return;
	}

}
