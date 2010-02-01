#include "systemstate.h"

#include "gamemain.h"
#include "gamesetting.h"
#include "gamenetwork.h"
#include "effectstate.h"

namespace space
{

	//次に遷移
	void GameSetting::setnextstate(int next)
	{

		//変わる元の処理
		switch(state){
			//初期化専用
			case -1:
				{
					isClient = false;
					player_num = 2;
					counter = 0;
					//ネットワークが残っていたらここで壊します。
					std::list<SystemState*> t = systemmain->nowstates;
					std::list<SystemState*>::iterator it =
						std::find_if(t.begin(),t.end(),std::bind2nd(SystemState::stateof(),GAMENETWORK));
					if(it!=t.end()){
						GameNetwork *gmn = pointer_cast<GameNetwork*>(*it);
						gmn->isNeed=false;
					}
				}
				break;
			case 0:
				break;
			case 2:
			case 4:
				if(next!=5){
					for(unsigned int i=0;i<client_sock.size();i++){
						SDLNet_TCP_Close(client_sock[i]);
					}
					client_sock.clear();
					if(server_sock!=NULL) SDLNet_TCP_Close(server_sock);
				}
				SDLNet_FreeSocketSet(sock_set);
				break;
			default:
				break;
		}

		//変わる先の処理
		pushtime_key[SDLK_RETURN]=2;
		pushtime_key[SDLK_q]=2;
		selectitem = 0;
		std::ifstream iptxt;
		char str[30];
		switch(next){
			case 0:
				{
					isClient = false;
					//ifstream
					iptxt.open("ip.txt");
					iptxt.getline(str,30,':');
					hostname = str;
					iptxt >> port;
					iptxt >> my_name;
					iptxt.close();
					break;
				}
			case 1:
				{
					break;
				}

			//サーバー側
			case 2:
				{
					if( SDLNet_ResolveHost(&ip,NULL,port) <0 ) {setnextstate(3);return;}
					if( (server_sock = SDLNet_TCP_Open(&ip)) ==NULL ) {setnextstate(3);return;}
					client_sock.clear();
					sock_set = SDLNet_AllocSocketSet(player_num);
					SDLNet_TCP_AddSocket(sock_set,server_sock);
					names.clear();
					names.push_back(my_name);

					break;
				}
			//クライアント側
			case 4:
				{
					if( SDLNet_ResolveHost(&ip,hostname.c_str(),port) <0 ) {setnextstate(3);return;}
					server_sock = NULL;
					client_sock.clear(); client_sock.push_back(SDLNet_TCP_Open(&ip));
					if( ! client_sock[0] ) {setnextstate(3);return;}
					sock_set = SDLNet_AllocSocketSet(1);
					SDLNet_TCP_AddSocket(sock_set,client_sock[0]);
					
					names.clear();
					std::string::size_type size = my_name.length()+1;
					SDLNet_TCP_Send(client_sock[0],&systemmain->version,4);
					SDLNet_TCP_Send(client_sock[0],&size,sizeof(std::string::size_type));
					SDLNet_TCP_Send(client_sock[0],const_cast<char*>(my_name.c_str()),size);
					
					break;
				}

			case 3:
				systemmain->sound_cancel.play();
				{
					break;
				}
			case 5:
				//変わったときに音を鳴らす。
				systemmain->sound_enter.play();

				//================
				//ゲーム登録
				//================
				{
					//プレイヤー構造体の作成
					std::vector<BaseSetting::Player> pls;
					for(unsigned int i=0;i<player_num;i++)
					{
						BaseSetting::Player pl;
						pl.input = &systemmain->input[i];
						pl.name = names[i];
						pls.push_back(pl);
					}

					//インプットだけ入れ替え(0に自分の入力のはずの奴が入ってるからそれと入れ替え)
					const Input *intemp = pls[my_number].input;
					pls[my_number].input = pls[0].input;
					pls[0].input = intemp;

					//ゲームメインの設定
					BaseSetting* basesetting = new BaseSetting(systemmain,pls,my_number,randomseed);
					//ネットワークの設定
					std::vector<Input*> inp;
					for(unsigned int i=0;i<pls.size();i++)
						inp.push_back( const_cast<Input*>(pls[i].input) );
					GameNetwork* gamenetwork = new space::GameNetwork( systemmain,inp,my_number,isClient,client_sock,server_sock);

					//ゲームメインはネットワークよりもうしろに登録するようにする
					//ネットワークの登録
					systemmain->nowstates.push_back( gamenetwork );
					//拠点の設定画面登録
					systemmain->nowstates.push_back( basesetting );
				}
				break;
			default:
				break;
		}
		//実際にかえる
		state = next;

	}

	//接続設定
	GameSetting::GameSetting(ProgramSystemMain *_sys) : SystemState(_sys,GAMESETTING)
	{
		//入力設定
		input = &_sys->input[0];
		//初期設定
		state = -1;
		//
		bmousepos = input->mouse.getpos();
		return_string = "";
		afterreturn = false;
	}

	GameSetting::~GameSetting(){
	}

	//draw
	void GameSetting::draw()
	{

		//ホスト、クライアントの表示
		char _str[256];

		//state==5(休眠状態)では描画を行わない。
		if(state==5)return;
		//state==-1(未初期化状態)では、すぐに0(初期画面)に切り替える
		if(state==-1)setnextstate(0);

		//======
		//SDL
		//======

		//背景の背景
		SDL_FillRect(systemmain->screen,NULL,0x003366);
		//背景
		{
			SDL_Rect yr = {
				systemmain->screen->w/2,systemmain->screen->h/2,
				systemmain->screen->w  ,systemmain->screen->h};
			SDLFILESRFS::inst().get("image/title.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
		}

		//logo
		{
			SDL_Rect rect;
			rect.x = (Sint16)(systemmain->screen->w/2);
			rect.y = (Sint16)(330 - 90.0/(counter*0.25+1));
			SDLFILESRFS::inst().get("image/title_logo.gif").Draw(systemmain->screen,&rect,NULL,systemmain->screen->h);
		}

		//場面に応じた表示
		//ちょっとした背景
		{
			SDL_Rect r;
			if(state == 0){
				r.x = 90;
				//r.y = 480-(220-(isClient?1:0)*15-1);
				if(selectitem==0 || selectitem==1)
					r.y = 480-(219-selectitem*15);
				else
					r.y = 480-(194-selectitem*15);
				r.w = 200;
				r.h = 15;
			}
			else{
				r.x = 90;
				r.y = 480-(55-1);
				r.w = 350;
				r.h = 15;
			}
			Sint16 x[4] = {r.x,r.x+r.w,r.x+r.w,r.x};
			Sint16 y[4] = {r.y,r.y,r.y+r.h,r.y+r.h};
			
			filledPolygonRGBA(systemmain->screen,x,y,4,0,128,128,(unsigned char)(63.0+96.0+96.0*sin((double)(counter*1.75)*M_PI/180.0)));
		}
		

		//設定表示

		//ポート表示
		{
			SDL_Rect _rect;
			sprintf(_str,"PORT :: %d",port);
			_rect.y = 135;
			_rect.x = 90;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//ホスト名表示
		{
			sprintf(_str,"HOST :: %s",hostname.c_str());
			SDL_Rect _rect;
			_rect.y = 150;
			_rect.x = 90;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//なまえ表示
		{
			sprintf(_str,"NAME :: %s",my_name.c_str());
			SDL_Rect _rect;
			_rect.y = 120;
			_rect.x = 90;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//選択矢印
		{
			SDL_Rect _rect;
			_rect.x = 68;
			
			if(state==0){
				if(selectitem==0 || selectitem==1)
					_rect.y = 205-selectitem*15;
				else
					_rect.y = 180-selectitem*15;
			}
			else{
				_rect.y = 205 - (isClient?1:0)*15;
			}
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"=>",systemmain->screen->h);
		}
		//CONNECT MODE
		{
			SDL_Rect _rect;
			_rect.y = 235;
			_rect.x = 90;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"CONNECT MODE",systemmain->screen->h);
		}
		//SERVER
		{
			SDL_Rect _rect;
			_rect.y = 205;
			_rect.x = 122;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"SERVER",systemmain->screen->h);
		}
		//CLIENT
		{
			SDL_Rect _rect;
			_rect.y = 190;
			_rect.x = 122;
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"CLIENT",systemmain->screen->h);
		}
		//プレイヤー数
		if( (state==0 && selectitem==0) || (state!=0 && !isClient) ){
			SDL_Rect _rect;
			_rect.y = 205;
			_rect.x = 220;
			if(player_num==2) sprintf(_str,"  %dP >",player_num);
			else if(player_num==4) sprintf(_str,"< %dP  ",player_num);
			else sprintf(_str,"< %dP >",player_num);
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}

		//OKorNG
		if(state==1){
			WinRect r;
			r.x=90; r.y=15; r.w=40; r.h=15;
			r.x=90+50*selectitem-15;
			boxRGBA(systemmain->screen,r.x,systemmain->screen->h-r.y,r.x+r.w,systemmain->screen->h-r.y-r.h,96,96,192,127+64+64*sin(counter*M_PI/180.0));
			r.x=90; r.y=15; r.w=30; r.h=15;
			systemmain->bmpfont.draw(systemmain->screen,r.x,r.y,"OK",systemmain->screen->h);
			r.x+=r.w+20;
			systemmain->bmpfont.draw(systemmain->screen,r.x,r.y,"NG",systemmain->screen->h);
			if(counter%60<47){
				r.x=90+50*selectitem-15;
				systemmain->bmpfont.draw(systemmain->screen,r.x,r.y,">",systemmain->screen->h);
			}
			
		}
		else if(state==3){
			WinRect r;
			r.x=75; r.y=15; r.w=40; r.h=15;
			boxRGBA(systemmain->screen,r.x,systemmain->screen->h-r.y,r.x+r.w,systemmain->screen->h-r.y-r.h,96,96,192,127+64+64*sin(counter*M_PI/180.0));
			if(counter%60<47) systemmain->bmpfont.draw(systemmain->screen,r.x,r.y,">",systemmain->screen->h);
			r.x=90; r.y=15; r.w=30; r.h=15;
			systemmain->bmpfont.draw(systemmain->screen,r.x,r.y,"OK",systemmain->screen->h);
		}



		//モードによる注意文字
		if(counter%90<70){
			//TEST
			SDL_Rect _rect= {90,40,0,0};

			if(state == 0){ sprintf(_str,"CHOOSE YOUR MODE"); }
			else if(state == 1){ sprintf(_str,"OK?"); }
			else if(state == 2){ sprintf(_str,"Please wait for Connection..."); }
			else if(state == 4){ sprintf(_str,"Please wait for Response..."); }
			else if(state == 3){ sprintf(_str,"TCP Conection Failed!!!"); }
			else if(state == 5){ sprintf(_str,"Please wait ..."); }

			//文字表示
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
			
		}

	}

	//update
	void GameSetting::update()
	{
		//bgmが再生されてなかったら再生。
		if(!systemmain->sound_bgm.isPlaying()){
			systemmain->sound_bgm.load("sound/waittime.wav");
			systemmain->sound_bgm.fadein(480);
		}

		//===========
		//入力のこと
		//==========
		Input ctrinp = *input;
		//mouse
		if( ctrinp.mouse.getleft().isPush() ) pushtime_left++;
		else pushtime_left = 0;
		if( ctrinp.mouse.getmiddle().isPush() ) pushtime_middle++;
		else pushtime_middle = 0;
		if( ctrinp.mouse.getright().isPush() ) pushtime_right++;
		else pushtime_right = 0;
		//前のmousepos
		if( bmousepos != ctrinp.mouse.getpos() ){
			bmousepos = ctrinp.mouse.getpos();
			mousemove = true;
		}
		else mousemove = false;

		//key
		const Keyboard::KeyMap &km = ctrinp.keyboard.getkeys();
		std::map<unsigned short,unsigned short> &now = pushtime_key;
		//切る
		std::map<unsigned short,unsigned short>::iterator nowit = now.begin();
		for(;nowit!=now.end();nowit++)
		{
			if( km.find(nowit->first) == km.end() ){
			  std::map<unsigned short,unsigned short>::iterator tit = nowit;
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


		//===================
		//設定画面遷移のこと
		//===================
		//設定
		if(state == 0){
			//上下操作について
			if( pushtime_key.count(SDLK_DOWN)  && pushtime_key[SDLK_DOWN]==1 ){
				if(selectitem < 4) selectitem++;
				systemmain->sound_move.play();
			}
			if( pushtime_key.count(SDLK_UP)  && pushtime_key[SDLK_UP]==1 ){
				if(selectitem > 0) selectitem--;
				systemmain->sound_move.play();
			}

			//左右操作について
			if(pushtime_key.count(SDLK_LEFT) && pushtime_key[SDLK_LEFT]==1 ){
				//クライアントサーバー選択
				if(selectitem == 0){
					if(player_num > 2) player_num--;
					systemmain->sound_setting.play();
					
				}
			}
			if( pushtime_key.count(SDLK_RIGHT) && pushtime_key[SDLK_RIGHT]==1){
				if(selectitem == 0){
					if(player_num < 4) player_num++;
					systemmain->sound_setting.play();
				}
			}
	
			//ENTERで次へ,クリックでも次へ
			if( (pushtime_key.count(SDLK_RETURN) && pushtime_key[SDLK_RETURN]==1) || pushtime_left==1){
				bool isGone=false;
				{
					Matrix21<int> p=input->mouse.getleft().getPushpos();
					WinRect r;
					r.x=209;r.y=204;r.w=44;r.h=16;
					if(r.Col_Dot(p)){
						isGone = true;
						if(player_num>2)player_num--;
					}
					r.x=253;r.y=204;r.w=44;r.h=16;
					if(r.Col_Dot(p)){
						isGone = true;
						if(player_num<4)player_num++;
					}
				}
				if(isGone){//飛ばす。なにもしない。
				}
				else if(selectitem==0 || selectitem==1){
					isClient = (selectitem==1);
					setnextstate(1);
				}
				else if(selectitem==2){
					(*std::find(systemmain->nowstates.begin(),systemmain->nowstates.end(),this)) = new StateDummy(systemmain);
					systemmain->setTextInput(this,"INPUT SERVER NAME",&return_string);
				}
				else if(selectitem==3){
					(*std::find(systemmain->nowstates.begin(),systemmain->nowstates.end(),this)) = new StateDummy(systemmain);
					systemmain->setTextInput(this,"INPUT YOUR PORT",&return_string);
				}
				else if(selectitem==4){
					(*std::find(systemmain->nowstates.begin(),systemmain->nowstates.end(),this)) = new StateDummy(systemmain);
					systemmain->setTextInput(this,"INPUT YOUR NAME",&return_string);
				}
				pushtime_key[SDLK_RETURN]=2;
				systemmain->sound_enter.play();
			}
			//q押したらタイトル
			if( (pushtime_key.count(SDLK_q) && pushtime_key[SDLK_q]==1) || pushtime_right==1){
				isNeed = false;
				systemmain->setTitle();
				systemmain->sound_cancel.play();
			}

			//何か帰ってきたとき。
			if(return_string!=""){
				if(selectitem==2){
					hostname = return_string;
					//ofstream
					std::ofstream iptxt("ip.txt");
					iptxt << hostname << ":" << port << std::endl << my_name << std::endl;
					iptxt.close();
				}
				else if(selectitem==3){
					port = atoi(return_string.c_str());
					//ofstream
					std::ofstream iptxt("ip.txt");
					iptxt << hostname << ":" << port << std::endl << my_name << std::endl;
					iptxt.close();
				}
				else if(selectitem==4){
					my_name = return_string;
					//ofstream
					std::ofstream iptxt("ip.txt");
					iptxt << hostname << ":" << port << std::endl << my_name << std::endl;
					iptxt.close();
				}
				return_string = "";
			}
			
		}
		//接続確認
		else if(state == 1){
			//左右操作について
			if(pushtime_key.count(SDLK_LEFT) && pushtime_key[SDLK_LEFT]==1 ){
				if(selectitem > 0) selectitem--;
				systemmain->sound_move.play();
			}
			if( pushtime_key.count(SDLK_RIGHT) && pushtime_key[SDLK_RIGHT]==1){
				if(selectitem < 1) selectitem++;
				systemmain->sound_move.play();
			}

			//戻る
			if((pushtime_key.count(SDLK_q) && pushtime_key[SDLK_q]==1) || pushtime_right==1){
				systemmain->sound_cancel.play();
				setnextstate(0);
			}
			//すすむ
			if((pushtime_key.count(SDLK_RETURN) && pushtime_key[SDLK_RETURN]==1) || pushtime_left==1){
				if(selectitem==0){
					systemmain->sound_enter.play();
					if(isClient)setnextstate(4);
					else setnextstate(2);
				}
				else if(selectitem==1){
					systemmain->sound_cancel.play();
					setnextstate(0);
				}
			}
		}
		//サーバー側の待ち
		else if(state==2){
			//ソケットをチェックして、何かあった時にはその数が帰る(つまり０だったら何もないので何もしない)					
			if(SDLNet_CheckSockets(sock_set,0))
			{
				//ソケットレディーな感じだったらクライアントとして登録してあげる。
				if(SDLNet_SocketReady(server_sock))
				{
					client_sock.push_back(SDLNet_TCP_Accept(server_sock));
					SDLNet_TCP_AddSocket(sock_set,client_sock.back());

					//バージョン確認をする
					unsigned int version;
					SDLNet_TCP_Recv(client_sock.back(),(void*)&version,4);
					//バージョン違いは落とす
					if(version!=systemmain->version){
						SDLNet_TCP_Close(client_sock.back());
						SDLNet_TCP_DelSocket(sock_set,client_sock.back());
						client_sock.pop_back();
						//setnextstate(3);
						//return;
					}
					else{
						//名前を受け取る
						std::string::size_type size;
						char str[256];
						SDLNet_TCP_Recv(client_sock.back(),(void*)&size,sizeof(std::string::size_type));
						SDLNet_TCP_Recv(client_sock.back(),(void*)str,size);
						names.push_back(str);
					}
				}
			}
			//クライアント数が規定に達すれば、なんかしら全員に送ったりする。
			if(client_sock.size() + 1 == player_num)
			{
				randomseed = time(NULL);
				my_number = 0;
				for(unsigned int i=0;i<client_sock.size();i++){
					//書き込む
					RawData senddata;
					senddata.write(&randomseed,4);
					unsigned char sendtemp = (player_num) + (i+1)*16;
					senddata.write(&sendtemp,1);


					//送る
					int sendsize = SDLNet_TCP_Send(client_sock[i],&senddata[0],senddata.size());
					if(sendsize != senddata.size()) throw "(gamesetting.cpp) Initialize Send Error!!";

					for(unsigned int j=0;j<names.size();j++){
						std::string::size_type size = names[j].length()+1;
						SDLNet_TCP_Send(client_sock[i],&size,sizeof(std::string::size_type));
						SDLNet_TCP_Send(client_sock[i],const_cast<char*>(names[j].c_str()),size);
					}
				}
				//次へ
				setnextstate(5);
			}
			//規定に達していないとき
			else{
				//全クライアントに関して受け取りを試みる
				for(unsigned int i=0;i<client_sock.size();i++){
					//どうも何かしら来た感じだったら
					if(SDLNet_SocketReady(client_sock[i])){
						//ここできたということはつまり落ちたということです。
						//自分も落ちます
						setnextstate(3);
						break;
					}
				}
				
				//q押したら前の画面に戻ったりする
				if((pushtime_key.count(SDLK_q) && pushtime_key[SDLK_q]==1) || pushtime_right==1){
					setnextstate(0);
					systemmain->sound_cancel.play();
				}
			}
		}
		//クライアントの待ち
		else if(state==4){
			//ソケットの何かあった時の数字を返す			
			if(SDLNet_CheckSockets(sock_set,0))
			{
				//どうも何かしら来た感じだったら
				if(SDLNet_SocketReady(client_sock[0]))
				{
					//受け取る
					unsigned char recvtemp = 0;
					RawData rdata;
					rdata.assign(5);
					int recvsize = SDLNet_TCP_Recv(client_sock[0],(void*)&rdata[0],5);
					//受信サイズ確認
					if(recvsize<0 || recvsize!=5){
						//throw "(gamesetting.cpp) Initialize Recv Error!!";
						setnextstate(3);
						return;
					}

					//読み込む。
					rdata.read((void*)&randomseed,4);
					rdata.read((void*)&recvtemp,1);
					player_num = recvtemp % 16;
					my_number  = recvtemp / 16;
					
					//名前も読み込む
					names.clear();
					for(unsigned int j=0;j<player_num;j++){
						std::string::size_type size;
						char str[256];
						SDLNet_TCP_Recv(client_sock[0],(void*)&size,sizeof(std::string::size_type));
						SDLNet_TCP_Recv(client_sock[0],(void*)str,size);
						names.push_back(str);
					}

					//次へ
					setnextstate(5);
				}
			}
			//q押したら前の画面に戻ったりする
			if( (pushtime_key.count(SDLK_q) && pushtime_key[SDLK_q]==1) || pushtime_right==1 ){
				setnextstate(0);
				systemmain->sound_cancel.play();
			}
		}
		//接続失敗コール
		else if(state==3){
			selectitem = 0;
			//戻る
			if( (pushtime_key.count(SDLK_RETURN) && pushtime_key[SDLK_RETURN]==1) || pushtime_left==1 ){
				setnextstate(0);
				systemmain->sound_enter.play();
			}
			//戻る
			if((pushtime_key.count(SDLK_q) && pushtime_key[SDLK_q]==1) || pushtime_right==1){
				systemmain->sound_cancel.play();
				setnextstate(0);
			}
		}
		//休眠状態
		else if(state==5){
			//基本なにもしない
		}

		//マウス操作に関して。
		if(mousemove){
			Matrix21<int> p = input->mouse.getpos();
			WinRect r;
			if(state==0){
				r.x=90;r.y=205;r.w=200;r.h=15;
				if(r.Col_Dot(p))selectitem=0;
				r.x=90;r.y=190;r.w=200;r.h=15;
				if(r.Col_Dot(p))selectitem=1;
				r.x=90;r.y=150;r.w=200;r.h=15;
				if(r.Col_Dot(p))selectitem=2;
				r.x=90;r.y=135;r.w=200;r.h=15;
				if(r.Col_Dot(p))selectitem=3;
				r.x=90;r.y=120;r.w=200;r.h=15;
				if(r.Col_Dot(p))selectitem=4;
			}
			else if(state==1){
				r.x=90; r.y=14; r.w=40; r.h=16;
				if(r.Col_Dot(p))selectitem=0;
				r.x=140; r.y=14; r.w=40; r.h=16;
				if(r.Col_Dot(p))selectitem=1;
			}
			else if(state==3){
				r.x=90; r.y=14; r.w=40; r.h=16;
				if(r.Col_Dot(p))selectitem=0;
			}
		}

		counter++;

	}

	//全部終わったあとに何かあった時の処理
	void GameSetting::after_all()
	{
		//休眠状態で削除。
		if(state==5){
			isNeed = false;
		}
	}
	
}
