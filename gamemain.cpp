#include "systemstate.h"
#include "gamemain.h"
#include "network.h"

namespace space
{

	PawnPeople* GameMain::setPawn(Object::PLAYERID _pid, double _x, double _y){
		objects.push_back(new PawnPeople(this,_pid,_x,_y) );
		return pointer_cast<PawnPeople*>(objects.back());
	}
	NormalBullet* GameMain::setNormalBullet(Object::PLAYERID _pid, double _x, double _y, double vx, double vy){
		objects.push_back(new NormalBullet(this,_pid,_x,_y,vx,vy) );
		return pointer_cast<NormalBullet*>(objects.back());
	}
	NormalBuilding* GameMain::setNormalBuilding(Object::PLAYERID _pid, double _x,double _y){
		objects.push_back(new NormalBuilding(this,_pid,_x,_y) );
		return pointer_cast<NormalBuilding*>(objects.back());
	}
	Fortress* GameMain::setFortress(Object::PLAYERID _pid, double _x,double _y){
		objects.push_back(new Fortress(this,_pid,_x,_y) );
		return pointer_cast<Fortress*>(objects.back());
	}
	Bomb* GameMain::setBomb(Object::PLAYERID _pid, double _x,double _y){
		objects.push_back(new Bomb(this,_pid,_x,_y) );
		return pointer_cast<Bomb*>(objects.back());
	}
	GroundBomb* GameMain::setGroundBomb(Object::PLAYERID _pid, double _x,double _y){
		objects.push_back(new GroundBomb(this,_pid,_x,_y) );
		return pointer_cast<GroundBomb*>(objects.back());
	}
	Shop* GameMain::setShop(Object::PLAYERID _pid, double _x,double _y){
		objects.push_back(new Shop(this,_pid,_x,_y) );
		return pointer_cast<Shop*>(objects.back());
	}
	Smith* GameMain::setSmith(Object::PLAYERID _pid,double _x,double _y){
		objects.push_back(new Smith(this,_pid,_x,_y));
		return pointer_cast<Smith*>(objects.back());
	}

	void GameMain::changePlayerCtrl(unsigned int ch){
		player_ctrl = ch;
		item[1].r.x=0; item[1].r.y=32*player_ctrl; item[1].r.w=32; item[1].r.h=32;
		item[2].r.x=0; item[2].r.y=32*player_ctrl; item[2].r.w=32; item[2].r.h=32;
		item[3].r.x=0; item[3].r.y=32*player_ctrl; item[3].r.w=32; item[3].r.h=32;
		item[4].r.x=0; item[4].r.y=32*player_ctrl; item[4].r.w=32; item[4].r.h=32;
		item[17].r.x=0; item[17].r.y=0; item[17].r.w=32; item[17].r.h=32;
		item[18].r.x=0; item[18].r.y=32*player_ctrl; item[18].r.w=32; item[18].r.h=32;
	}

	GameMain::GameMain(ProgramSystemMain* _sys, std::vector<Player> _pl,unsigned int _ctrlpl, unsigned int _rands,bool _rep)
		: SystemState(_sys,GAMEMAIN)
	{
		setrep = _rep;

		//お金用のフォントロード
		moneyfont.open("image/number_large.gif");
		moneyfont.fontsetting(18,20,-'0');

		//ステータス用のフォント
		statusfont.open("image/status_num.gif");
		statusfont.fontsetting(9,10,-'0');
		
		//プレイヤーの設定とか。
		player = _pl;	//プレイヤー
		player_ctrl = _ctrlpl;	//自分の番号
		randamseed = _rands;	//ランダムシード
		srand(randamseed);	//種をまく
		objects.clear();

		//フィールドサイズ、及びその他の画面サイズ初期化
		//フィールドサイズ。見た目と分離してない。これは実装を変えなくてはならない。
		//オフセットを実装したため、若干マシになりました。
		field_rect.w=440; field_rect.h=420;
		field_rect.x=10;  field_rect.y=systemmain->screen->h-(field_rect.h+10);
		//各プレイヤーのステータス領域
		states_rect.x=10; states_rect.y=10;
		states_rect.w=field_rect.w;
		states_rect.h=field_rect.y-20;
		//お金表示領域
		money_rect.x = states_rect.x + states_rect.w + 10 + 4;
		money_rect.y = 10;
		money_rect.w = 162;
		money_rect.h = 37;
		//コンパネ領域
		control_rect.x = money_rect.x;
		control_rect.y = money_rect.y + money_rect.h + 5;
		control_rect.w=162;
		control_rect.h=155;
		//その他データ表示領域
		data_rect.x = control_rect.x;
		data_rect.y = control_rect.y + control_rect.h + 20;
		data_rect.w=162;
		data_rect.h=213;
		//アイテム選択するところの座標指定
		for(unsigned int i=0;i<12;i++){
			items_rect[i].x =  control_rect.x + control_rect.w/2 - 74 + (i%4)*38;
			items_rect[i].y =  control_rect.y + control_rect.h/2 - 56 + (i/4)*38;
			items_rect[i].w =  36;
			items_rect[i].h =  36;
		}

		//また変な初期化（アイテムの種類表示）
		{
			//種類変更の当たり判定のやつ
			flippos = WinRect(control_rect.x,control_rect.y+control_rect.h-14,control_rect.w,28);

			//アイテムの種類表示するとこの画像の配列
			std::vector<const SDLSURFACE*> &sv = sortofitems_drawpict;

			//建物
			sv.push_back(&SDLFILESRFS::inst().get("image/building.gif"));
			//その他
			sv.push_back(&SDLFILESRFS::inst().get("image/supply.gif"));
		}


		//初期オブジェクト設置
		//各プレイヤーに関して
		for(unsigned int i=0;i<player.size();i++)
		{
			player[i].bmousepos.x = player[i].input->mouse.getpos().x+500;
			player[i].bmousepos.y = player[i].input->mouse.getpos().y+500;
			for(int j=0;j<12;j++)player[i].shownitems[j] = j+1; //初期アイテム
			player[i].sortofitems = 0; //アイテムの種類
			player[i].score = 0; //スコア初期化
			player[i].bekilled_cnt = 0; //討伐された数初期化
			player[i].killed_cnt = 0; //討伐した数初期化
			player[i].spendmoney = 0; //使用金額初期化
			player[i].money = 300; //所持金初期化
			player[i].selected_item = 0; //選択アイテム初期化
			for(unsigned int j=0;j<64;j++){player[i].items[j]=0;} //取得アイテム初期化
			player[i].selected.clear(); //選択領域を初期化
			player[i].drawoffset=Matrix21<double>(0.0,0.0); //フィールドの表示位置
			player[i].drawoffset_b = player[i].drawoffset;
			player[i].pushtime_left = 0;
			player[i].pushtime_middle = 0;
			player[i].pushtime_right = 0;
			player[i].pushtime_key.clear();
			player[i].ispull_left = false;
			player[i].ispull_middle = false;
			player[i].ispull_right = false;
			player[i].ispull_key.clear();
			//拠点設置
			player[i].base = new PlayerBase(this,i+1,
				field_rect.w/2 + cos(2.0*(double)i*M_PI/(double)player.size())*(double)(field_rect.w/16*7) ,
				field_rect.h/2 + sin(2.0*(double)i*M_PI/(double)player.size())*(double)(field_rect.h/16*7) );
			objects.push_back( player[i].base );
		}

		//フィールドの物体設置
		//火山
		objects.push_back( new FieldVolcano(this,field_rect.w/2,field_rect.h/2) );
		//岩
		for(int i=0;i<10;i++){
			double r = (double)(rand()%220+20);
			double theta = (double)(rand()%36000)/18000.0*M_PI;
			objects.push_back( new FieldRock(this,r*cos(theta)+field_rect.w/2,r*sin(theta)+field_rect.h/2-20) );
		}
		//木
		for(int i=0;i<10;i++){
			double r = (double)(rand()%220+20);
			double theta = (double)(rand()%36000)/18000.0*M_PI;
			objects.push_back( new FieldTree(this,r*cos(theta)+field_rect.w/2,r*sin(theta)+field_rect.h/2-20) );
		}
		//サボテン
		for(int i=0;i<10;i++){
			double r = (double)(rand()%220+20);
			double theta = (double)(rand()%36000)/18000.0*M_PI;
			objects.push_back( new FieldSaboten(this,r*cos(theta)+field_rect.w/2,r*sin(theta)+field_rect.h/2-20) );
		}

		//BGM
		systemmain->sound_bgm.load("sound/bgm.wav");
		systemmain->sound_bgm.play(-1);
		
		//アイテム設定
		for(int i=0;i<64;i++){
			item.push_back(Item());
		}
			
		//建物
		item[1].obj = new space::NormalBuilding();
		item[1].showpic = &(SDLFILESRFS::inst().get("image/ie_all.gif"));
		item[2].obj = new space::Fortress();
		item[2].showpic = &SDLFILESRFS::inst().get("image/yousai_all.gif");
		item[3].obj = new space::Shop();
		item[3].showpic = &SDLFILESRFS::inst().get("image/shop_all.gif");
		item[4].obj = new space::Smith();
		item[4].showpic = &SDLFILESRFS::inst().get("image/smith_all.gif");
		//アイテム
		item[17].obj = new space::Bomb();
		item[17].showpic = &SDLFILESRFS::inst().get("image/bomb.gif");
		item[18].obj = new space::GroundBomb();
		item[18].showpic = &SDLFILESRFS::inst().get("image/groundbomb.gif");

		//画像設定はこの中
		changePlayerCtrl(player_ctrl);

		//終わりようカウント
		forEnd_cnt = 240;

		//さあ、当たり判定の時間だ
		mortontree.Init(8, -250.0f, -250.0f, 750.0f, 750.0f);

		if(setrep){
			//リプレイファイル用意
			std::stringstream ss;
			ss << "replay/" << player_ctrl << time(NULL) << ".rep";
			replayfile.open(ss.str().c_str(),std::ios::out|std::ios::binary);
			//ヘッダ形式
			//バージョン(4) + ランダムシード(4) + プレイ人数(1) + 拠点種別(プレイ人数) + 名前(プレイ人数*(1+名前の長さ))
			replayfile.write((char*)&systemmain->version,4);
			replayfile.write((char*)&randamseed,4);
			int t = player.size();
			replayfile.write((char*)&t,1);
			for(unsigned int i=0;i<player.size();i++){replayfile.write((char*)&player[i].typeofBase,1);}
			for(unsigned int i=0;i<player.size();i++){
				int t = player[i].name.size();
				replayfile.write((char*)&t,1);
				replayfile.write(player[i].name.c_str(),player[i].name.size());
			}
		}

		deadcount = 0;

	}

	GameMain::~GameMain()
	{
		while(!objects.empty())
		{
			delete objects.back();
			objects.pop_back();
		}
		systemmain->sound_bgm.stop();
		//ここでリプレイファイルが閉じられる場合は、つまり正常にゲームが終了していない。
		if(replayfile.is_open()) replayfile.close();
	}

	void GameMain::DisplayReload()
	{
	}

	void GameMain::setting3D()
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(15.0, 1.0, 0.1, 500.0);
	
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glViewport(410,250,200,200);
	}

	void GameMain::update()
	{
		//死亡確認カウント
		deadcount = 0;
		//よく使うもの
		std::list<Object*>::iterator _it,_itj;

		//====================
		//全プレイヤーに対して
		//====================
		//操作情報のあれこれ
		//ついでにリプレイ保存
		static int pss = 2;
		for(unsigned char i=0;i<player.size();i++)
		{
			
			//入力のこと
			Input ctrinp = *player[i].input;
			RawData rd,rlast;
			rd.clear(),rlast.clear();
			unsigned char inp;

			//マウス位置
			if( player[i].bmousepos!=ctrinp.mouse.getpos()){
				inp = 3;
				rd.write(&inp,1);
				rd.write(&ctrinp.mouse.getpos().x,pss);
				rd.write(&ctrinp.mouse.getpos().y,pss);
				player[i].bmousepos = ctrinp.mouse.getpos();
			}

			//mouse
			//LEFT
			if( ctrinp.mouse.getleft().isPush() ){
				player[i].pushtime_left++;
				if(player[i].pushtime_left == 1){
					inp = 0;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getleft().getPushpos().x,pss);
					rd.write(&ctrinp.mouse.getleft().getPushpos().y,pss);
				}
			}
			else{
				if(player[i].pushtime_left!=0){
					player[i].ispull_left=true;
					inp = 4;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getleft().getPullpos().x,pss);
					rd.write(&ctrinp.mouse.getleft().getPullpos().y,pss);
				}
				else player[i].ispull_left=false;
				player[i].pushtime_left = 0;
			}
			//MIDDLE
			if( ctrinp.mouse.getmiddle().isPush() ){
				player[i].pushtime_middle++;
				if(player[i].pushtime_middle == 1){
					inp = 1;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getmiddle().getPushpos().x,pss);
					rd.write(&ctrinp.mouse.getmiddle().getPushpos().y,pss);
				}
			}
			else{
				if(player[i].pushtime_middle!=0){
					player[i].ispull_middle=true;
					inp = 5;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getmiddle().getPullpos().x,pss);
					rd.write(&ctrinp.mouse.getmiddle().getPullpos().y,pss);
				}
				else player[i].ispull_middle=false;
				player[i].pushtime_middle = 0;
			}
			//RIGHT
			if( ctrinp.mouse.getright().isPush() ){
				player[i].pushtime_right++;
				if(player[i].pushtime_right == 1){
					inp = 2;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getright().getPushpos().x,pss);
					rd.write(&ctrinp.mouse.getright().getPushpos().y,pss);
				}
			}
			else{
				if(player[i].pushtime_right!=0){
					player[i].ispull_right=true;
					inp = 6;
					rd.write(&inp,1);
					rd.write(&ctrinp.mouse.getright().getPullpos().x,pss);
					rd.write(&ctrinp.mouse.getright().getPullpos().y,pss);
				}
				else player[i].ispull_right=false;
				player[i].pushtime_right = 0;
			}

			//リプレイバイナリ形式 = ( データサイズ(1)+チャンク(データサイズ) ) (人数)
			unsigned char appendsize = (unsigned char)rd.size();
			rlast.write(&appendsize,1);
			if(appendsize)rlast.write(&rd[0],rd.size());
			//書き込み
			if(setrep) replayfile.write((char*)&rlast[0],rlast.size());

			//key
			const Keyboard::KeyMap &km = ctrinp.keyboard.getkeys();
			std::map<unsigned short,unsigned short> &now = player[i].pushtime_key;
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

			//ついで。
			//死亡かどうかの確認カウント
			if(player[i].base->hp <= 0)deadcount++;
		}


		//全員又は一人以外死んでいたら、カウントで次に進む。
		if(deadcount + 1 >= player.size())
		{
			//リプレイ停止
			int rnd = rand();
			replayfile.write((char*)&rnd,4);
			replayfile.close();

			//終了までのカウント処理など。
			if(forEnd_cnt==0){
				isNeed = false;
				systemmain->setResult(this);
				systemmain->setFadeIn(30);
				systemmain->sound_bgm.stop();
			}
			else if(forEnd_cnt==29){
				systemmain->setFadeOut(30);
				systemmain->sound_bgm.fadeout(480);
			}
			forEnd_cnt--;
		}

		//そうでなければ以下のゲーム処理を実行する。
		else{

			//実際の操作について
			for(unsigned int i=0;i<player.size();i++)
			{
				Input ctrinp = *player[i].input;

				//右クリックされた時の動作
				if(player[i].pushtime_right == 1)
				{
					//アイテム選択は解除。
					player[i].selected_item = 0;
					//フィールド上だったとき
					Matrix21<int> pushpos = ctrinp.mouse.getright().getPushpos();
					if( field_rect.Col_Dot( pushpos ) ){
						std::list<Object*>::iterator it = player[i].selected.begin();
						for(;it!=player[i].selected.end();it++){
							if((*it)->get_class_id() == Object::PEOPLE){
								People* temp = pointer_cast<People*>(*it);
								temp->dstpos.x = pushpos.x-field_rect.x-player[i].drawoffset.x;
								temp->dstpos.y = pushpos.y-field_rect.y-player[i].drawoffset.y;
							}
						}
					}
				}

				//左クリック最初に押される動作
				if(player[i].pushtime_left == 1)
				{
					//コントロール欄押されれば
					if( control_rect.Col_Dot( ctrinp.mouse.getleft().getPushpos() ) )
					{
						for(int j=0;j<12;j++)
						{
							//アイテム欄が押されれば
							if(items_rect[j].Col_Dot( ctrinp.mouse.getleft().getPushpos() ) ){
								player[i].selected_item = player[i].shownitems[j];
							}
						}
					}

					//フィールド内ならば
					else if( field_rect.Col_Dot( ctrinp.mouse.getleft().getPushpos() ) )
					{
						//選択しているアイテムがあった時の操作。
						if(player[i].selected_item != 0)
						{
							//押した位置
							double x = (double)ctrinp.mouse.getleft().getPushpos().x - (double)field_rect.x - player[i].drawoffset.x;
							double y = (double)ctrinp.mouse.getleft().getPushpos().y - (double)field_rect.y - player[i].drawoffset.y;
							bool setaccept = false;

							//おける範囲かどうか
							for(_it=objects.begin();_it!=objects.end();_it++){
								if((*_it)->get_player_id()-1==i){
									Matrix21<double> r( (*_it)->fieldpos.x,(*_it)->fieldpos.y);
									if( (r.x-x)*(r.x-x)+(r.y-y)*(r.y-y) < (*_it)->powerdist*(*_it)->powerdist){
										setaccept=true;
										break;
									}
								}
							}

							//おけるならおく
							if(setaccept){
								Item temp = item[player[i].selected_item];
								//お金消費できたなら
								if(temp.obj && temp.obj->price>0 && player[i].spendMoney(temp.obj->price)){
									//設置
									switch(player[i].selected_item)
									{
									case 1:
										setNormalBuilding(i+1,x,y);
										break;
									case 2:
										setFortress(i+1,x,y);
										break;
									case 3:
										setShop(i+1,x,y);
										break;
									case 4:
										setSmith(i+1,x,y);
										break;
									case 17:
										setBomb(i+1,x,y);
										break;
									case 18:
										setGroundBomb(i+1,x,y);
										break;
									}
								}


							}
						}
						
					}

					//種類変更が押されたとき
					{
						if( flippos.Col_Dot( ctrinp.mouse.getleft().getPushpos() ) ){
							//次に進める
							player[i].sortofitems++;
							if(player[i].sortofitems >= GameMain::sortofitems_drawpict.size())player[i].sortofitems=0;
							//建物の時
							if(player[i].sortofitems==0){
								for(int k=0;k<12;k++) player[i].shownitems[k]=k+1;
							}
							//アイテムの時
							else if(player[i].sortofitems==1){
								for(int k=0;k<12;k++) player[i].shownitems[k]=k+17;
							}
						}
					}

				}			
				//左クリックが引かれたとき。
				else if(player[i].ispull_left)
				{
					//クリックした場所がフィールド内ならば
					if( field_rect.Col_Dot( ctrinp.mouse.getleft().getPushpos() ) )
					{
						//それまでに選択してた分を放棄
						player[i].selected.clear();
						//オブジェクト全部と当たり判定
						std::list<Object*>::iterator it = objects.begin();
						for(;it!=objects.end();it++){
							Matrix21<int> tps = ctrinp.mouse.getleft().getPushpos();
							Matrix21<int> tpl = ctrinp.mouse.getleft().getPullpos();
							WinRect temp(
								tpl.x-field_rect.x-(int)player[i].drawoffset.x,
								tpl.y-field_rect.y-(int)player[i].drawoffset.y,
								tps.x-tpl.x,
								tps.y-tpl.y
							);
							//当たってたら選択
							if(
								temp.Col_Dot(Matrix21<double>((*it)->fieldpos.x,(*it)->fieldpos.y)) &&
								(*it)->get_player_id()-1 == i
							){
								player[i].selected.push_back(*it);
							}
						}
					}
				}

				//右クリックと左クリックが同時か、中クリックが押されたとき
				if( (player[i].pushtime_left>=1 && player[i].pushtime_right>=1) ||
					(player[i].pushtime_middle>=1) ){
					//押されはじめ
					//オフセットから押した位置を引いたものを保存する。
					//オフセット再計算の時はそのときのマウスの位置を足すだけですむ。
					if(	player[i].pushtime_left==1){
						player[i].drawoffset_b = player[i].drawoffset - (Matrix21<double>)player[i].input->mouse.getleft().getPushpos();
					}
					else if( player[i].pushtime_right==1){
						player[i].drawoffset_b = player[i].drawoffset - (Matrix21<double>)player[i].input->mouse.getright().getPushpos();
					}
					else if( player[i].pushtime_middle==1){
						player[i].drawoffset_b = player[i].drawoffset - (Matrix21<double>)player[i].input->mouse.getmiddle().getPushpos();
					}
					//それ以外
					else{
						player[i].drawoffset = player[i].drawoffset_b + (Matrix21<double>)player[i].input->mouse.getpos();
					}
				}

			}

			//=======================
			//オブジェクト全体の処理
			//=======================

			//move全部実行
			_it = objects.begin();
			for(;_it!=objects.end();_it++) (*_it)->move();
			
			//collision全部実行
			Object** opp;
			int num=CollisionAllObjects(&opp);
			for(int i=0;i<num;i+=2){
				(opp[i]->get_class_id() < opp[i+1]->get_class_id()) ?
				opp[i+1]->collision(opp[i]) : opp[i]->collision(opp[i+1]);
			}

			//after_collision全部実行
			_it = objects.begin();
			for(;_it!=objects.end();_it++) (*_it)->after_collision();

		}

		//選択したオブジェクトから不要なものを削除
		for(unsigned int i=0;i<player.size();i++){
			_it = player[i].selected.begin();
			while(_it!=player[i].selected.end()){
				if((*_it)->isNeed==false) _it=player[i].selected.erase(_it);
				else _it++;
			}
		}

		//本体からもどうやら必要ないっぽいものを削除します
		_it = objects.begin();
		while(_it!=objects.end()){
			if((*_it)->isNeed==false){
				delete *_it;
				_it=objects.erase(_it);
			}
			else _it++;
		}

		//全力でソート
		objects.sort(ObjectSortFuncByPos());
		//std::stable_sort(objects.begin(),objects.end(),ObjectSortFuncByPos());

		counter++;
	}

	void GameMain::draw()
	{
		//一番大事なもの
		std::list<Object*>::iterator _it= objects.begin();

		//===============//
		//===============//
		//SDLによる描画  //
		//===============//
		//===============//

		//================
		//ゲーム関係の描画
		//================
		char _str[256];
		//背景の背景
		{
			SDL_Rect _sr = {field_rect.x,field_rect.y,field_rect.w,field_rect.h};
			SDL_FillRect(systemmain->screen,NULL,0xff004488);
		}
		//背景
		SDL_Rect yr = {
			(Sint16)((double)field_rect.x+field_rect.w/2.0+player[player_ctrl].drawoffset.x)+10,
			(Sint16)((double)field_rect.y+field_rect.h/2.0+player[player_ctrl].drawoffset.y)-45,
			field_rect.w ,field_rect.h
		};
		SDLFILESRFS::inst().get("image/stage.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);

		//おける範囲表示
		for(_it=objects.begin();_it!=objects.end();_it++){
			if((*_it)->get_player_id()-1==player_ctrl && (*_it)->powerdist>=5.0){
				//範囲のこと
				SDL_Rect r = {
					(Sint16)((*_it)->fieldpos.x+(double)field_rect.x+player[player_ctrl].drawoffset.x),
					(Sint16)((*_it)->fieldpos.y+(double)field_rect.y+player[player_ctrl].drawoffset.y),
					0,0
				};
				aacircleRGBA(systemmain->screen,r.x,systemmain->screen->h-r.y,(Sint16)((*_it)->powerdist),128,64,192,255);
				filledCircleRGBA(systemmain->screen,r.x,systemmain->screen->h-r.y,(Sint16)((*_it)->powerdist),128,64,192,32);
			}
		}

		//全部描画(2D)
		_it= objects.begin();
		for(;_it!=objects.end();_it++)
		{
			//通常描画
			(*_it)->draw2D();
		}

		//選択したオブジェクトの描画
		{
			_it=player[player_ctrl].selected.begin();
			for(;_it!=player[player_ctrl].selected.end();_it++){
				//位置
				SDL_Rect yr = {
					(Sint16)((*_it)->fieldpos.x+(double)field_rect.x+player[player_ctrl].drawoffset.x+(*_it)->maincircle.d/2.0+5.0),
					(Sint16)((*_it)->fieldpos.y+(double)field_rect.y+player[player_ctrl].drawoffset.y+(*_it)->maincircle.d/2.0+5.0),
					0,0
				};
				//描画
				SDLFILESRFS::inst().get("image/ordermark.gif").Draw(systemmain->screen,&yr,NULL,systemmain->screen->h);
			}
		}


		//選択枠の表示
		if( field_rect.Col_Dot(player[player_ctrl].input->mouse.getleft().getPushpos()) && player[player_ctrl].input->mouse.getleft().isPush() )
		{
			Matrix21<int> _psps = player[player_ctrl].input->mouse.getleft().getPushpos();
			Matrix21<int> _plps = player[player_ctrl].input->mouse.getleft().getPullpos();

			if(player[player_ctrl].input->mouse.getleft().isPush())
				_plps=player[player_ctrl].input->mouse.getpos();
			
			unsigned char rgba[4][4] = {
				{0xff,0x00,0x00,0xff},
				{0x00,0x00,0xff,0xff},
				{0x00,0xff,0x00,0xff},
				{0xff,0xff,0x00,0xff}
			};

			rectangleRGBA(systemmain->screen,
				      _psps.x, systemmain->screen->h-_psps.y, _plps.x, systemmain->screen->h-_plps.y,
				      rgba[player_ctrl][0], rgba[player_ctrl][1], rgba[player_ctrl][2], rgba[player_ctrl][3]);
		}
		//ゲーム画面の枠線
		//field_rect.Draw(systemmain->screen,0x44,0xff,0xff,0xff,systemmain->screen->h);

		//===================
		//他のゲーム関係描画
		//===================
		//全体の枠線
		{
			SDL_Rect temprect = {systemmain->screen->w/2,systemmain->screen->h/2,0,0};
			SDLFILESRFS::inst().get("image/mainframe.gif").Draw(systemmain->screen,&temprect,NULL,systemmain->screen->h);
		}

		//データ画面の枠線
		//data_rect.Draw(systemmain->screen,0x22,0xaa,0x22,0xff,systemmain->screen->h);
		//データ画面の背景
		{
			SDL_Rect temprect = {data_rect.x+data_rect.w/2-1,data_rect.y+data_rect.h/2+5,0,0};
			SDLFILESRFS::inst().get("image/data.gif").Draw(systemmain->screen,&temprect,NULL,systemmain->screen->h);
			//拠点によって画像をかえる
			SDL_Rect ra = {150,0,150,150};
			temprect.y -= 20;
			if(player[player_ctrl].typeofBase==BASETYPE_LOVE)
				SDLFILESRFS::inst().get("image/love.gif").Draw(systemmain->screen,&temprect,&ra,systemmain->screen->h);
			else if(player[player_ctrl].typeofBase==BASETYPE_MONEY)
				SDLFILESRFS::inst().get("image/moneybase.gif").Draw(systemmain->screen,&temprect,&ra,systemmain->screen->h);
			else if(player[player_ctrl].typeofBase==BASETYPE_POWER)
				SDLFILESRFS::inst().get("image/power.gif").Draw(systemmain->screen,&temprect,&ra,systemmain->screen->h);
		}
		//とりあえず選択オブジェクト数の表示
		{
			sprintf(_str,"(%d,%d)",player[player_ctrl].input->mouse.getpos().x,player[player_ctrl].input->mouse.getpos().y);
			//sprintf(_str,"selects = %d",player[player_ctrl].selected.size());
			//sprintf(_str,"item = %d",player[player_ctrl].selected_item);
			SDL_Rect _rect= {
				data_rect.x+8,
				data_rect.y+data_rect.h-25,
				0,0};
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//とりあえずオブジェクト数の表示
		{
			sprintf(_str,"objects = %d",objects.size());
			SDL_Rect _rect= {
				data_rect.x+8,
				data_rect.y+data_rect.h-40,
				0,0};
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//とりあえずプレイヤー番号
		{
			sprintf(_str,"ctrl_num = %d",player_ctrl);
			SDL_Rect _rect= {
				data_rect.x+8,
				data_rect.y+data_rect.h-55,
				0,0};
			systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}
		//説明表示
		{
			Object* _dscobj;
			Item temp = item[player[player_ctrl].selected_item];
			_dscobj = temp.obj;

			if(_dscobj){
				SDL_Rect _rect= { data_rect.x+8, data_rect.y+data_rect.h-85, 0,0};
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"[ITEM]",systemmain->screen->h);
				_rect.x = data_rect.x + 60;
				//説明表示
				_rect.y-=15; sprintf(_str,"COST:%d",_dscobj->price);
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
				_rect.y-=15; sprintf(_str,"LIFE:%d",_dscobj->hp);
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
				_rect.y-=15; sprintf(_str," DEF:%d",_dscobj->def);
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
				//アイコン表示
				temp.showpic->Draw(
					systemmain->screen,
					&(SDL_Rect)WinRect(data_rect.x+30,data_rect.y+100,0,0),
					&(SDL_Rect)temp.r,
					systemmain->screen->h);
			}
		}		

		//コントロール画面の枠線
		//control_rect.Draw(systemmain->screen,0x44,0xff,0x44,0xff,systemmain->screen->h);


		//選択されてないもののこと
		{
			//アイテムの種類表示するとこの画像の配列
			std::vector<const SDLSURFACE*> &sv = sortofitems_drawpict;
			int j=1;
			for(unsigned int i=0;i<sv.size();i++){
				//選択されてないのは飛ばす
				if(i == player[player_ctrl].sortofitems)continue;
	
				//コントロール画面の背景(選択していないもの
				SDL_Rect temprect = {control_rect.x+control_rect.w/2+j*3,control_rect.y+control_rect.h/2+j*7,0,0};
				SDL_Rect temprectsrc = {0,155,162,155};
				SDLFILESRFS::inst().get("image/board_bs.gif").Draw(systemmain->screen,&temprect,&temprectsrc,systemmain->screen->h);
				
				//コントロール画面のアイテムの種類(選択していないもの)
				SDL_Rect rc = {0,sv[i]->surface->h/2,sv[i]->surface->w,sv[i]->surface->h/2};
				SDL_Rect rd = {control_rect.x+rc.w/2-1+j*3,control_rect.y+control_rect.h-2+rc.h/2+j*7-8,0,0};
				sv[i]->Draw(systemmain->screen,&rd,&rc,systemmain->screen->h);
				
				j++;
			}
		}

		//コントロール画面の背景
		{
			SDL_Rect temprect = {control_rect.x+control_rect.w/2,control_rect.y+control_rect.h/2,0,0};
			SDL_Rect temprectsrc = {0,0,162,155};
			SDLFILESRFS::inst().get("image/board_bs.gif").Draw(systemmain->screen,&temprect,&temprectsrc,systemmain->screen->h);
		}
		//コントロール画面のアイテム描画
		for(int i=0;i<12;i++)
		{

			//適当に変換
			SDL_Rect r = {
				items_rect[i].x+items_rect[i].w/2,
				items_rect[i].y+items_rect[i].h/2,
				0,0
			};
			SDL_Rect ra = { 0,0,24,24 };
			SDL_Rect rb = { 0,36,36,36 };
			//アイテムの背景
			if( items_rect[i].Col_Dot( player[player_ctrl].input->mouse.getpos() ) ) rb.y = 0;
			if( player[player_ctrl].selected_item != 0 &&
				player[player_ctrl].selected_item == player[player_ctrl].shownitems[i])
			{ rb.y = 0;}

			SDLFILESRFS::inst().get("image/chipboard_red.gif").Draw(systemmain->screen,&r,&rb,systemmain->screen->h);
			//アイテム
			const SDLSURFACE * temp = item[player[player_ctrl].shownitems[i]].showpic;
			SDL_Rect rim = item[player[player_ctrl].shownitems[i]].r;
			temp->Draw(systemmain->screen,&r,&rim,systemmain->screen->h);
		}
		
		//コントロール画面のアイテムの種類(選択しているもの)
		{
			//アイテムの種類表示するとこの画像の配列
			std::vector<const SDLSURFACE*> &sv = sortofitems_drawpict;
			//今選ばれてるアイテムの描画
			{
				int now = player[player_ctrl].sortofitems;
				//普通の描画
				SDL_Rect rc = {0,0,sv[now]->surface->w,sv[now]->surface->h/2};
				SDL_Rect rd = {control_rect.x+rc.w/2-1,control_rect.y+control_rect.h-2+rc.h/2-8,0,0};
				sv[now]->Draw(systemmain->screen,&rd,&rc,systemmain->screen->h);
			}
		}
		
		//種類変更のとこにかざされた時
		{
			if( flippos.Col_Dot( player[player_ctrl].input->mouse.getpos() ) ){
				SDL_Rect temprect = {control_rect.x+control_rect.w-26,control_rect.y+control_rect.h+5,0,0};
				SDLFILESRFS::inst().get("image/circle_arrow.gif").Draw(systemmain->screen,&temprect,NULL,systemmain->screen->h);
			}
		}

		//お金画面
		//お金画面の枠線
		//money_rect.Draw(systemmain->screen,0xff,0x55,0x55,0xff,systemmain->screen->h);
		//お金画面の背景
		{
			SDL_Rect temprect = {money_rect.x+money_rect.w/2-1,money_rect.y+money_rect.h/2+5,0,0};
			SDLFILESRFS::inst().get("image/money.gif").Draw(systemmain->screen,&temprect,NULL,systemmain->screen->h);
		}
		//お金の描画
		{
			sprintf(_str,"%6d;:",player[player_ctrl].money);
			SDL_Rect _rect= {
				money_rect.x+9,
				money_rect.y+5,
				0,0};
			moneyfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
		}


		//つかまれてるアイテムがある時の描画
		if(item[player[player_ctrl].selected_item].obj)
		{
			//適当に変換
			SDL_Rect r = {
				player[player_ctrl].input->mouse.getpos().x,
				player[player_ctrl].input->mouse.getpos().y,
				0,0
			};
			//アイテム
			const SDLSURFACE * temp = item[player[player_ctrl].selected_item].showpic;
			SDL_Rect rim = item[player[player_ctrl].selected_item].r;
			temp->Draw(systemmain->screen,&r,&rim,systemmain->screen->h);
		}
		//アイテムはつかんでいないが、アイテム上にマウスをかざしている時。
		else{
			for(int i=0;i<12;i++){
				Object* shownobj = item[player[player_ctrl].shownitems[i]].obj;
				if(shownobj && items_rect[i].Col_Dot(player[player_ctrl].input->mouse.getpos())){
					SDL_Rect r = {
						items_rect[i].x - 40 + 30,
						items_rect[i].y+items_rect[i].h/2+items_rect[i].h - 5,
						0,0
					};
					char str[256];
					sprintf(str,"%4dG",shownobj->price);
					SDLFILESRFS::inst().get("image/popup_balloon.gif").Draw(systemmain->screen,&r,NULL,systemmain->screen->h);
					if(shownobj->price <= player[player_ctrl].money)
						systemmain->bmpfont.draw(systemmain->screen,r.x-30,r.y-2,str,systemmain->screen->h);
					else
						systemmain->bmpfontred.draw(systemmain->screen,r.x-30,r.y-2,str,systemmain->screen->h);
					break;
				}
			}
		}

		//各プレイヤーのステータス画面の枠線
		//states_rect.Draw(systemmain->screen,0xff,0xff,0x44,0xff,systemmain->screen->h);
		//各オブジェクト数カウント
		std::vector<int> ienum(player.size(),0);
		std::vector<int> peoplenum(player.size(),0);
		_it= objects.begin();
		for(;_it!=objects.end();_it++)
		{
			if((*_it)->get_class_id() == Object::PEOPLE)
				peoplenum[(*_it)->get_player_id()-1]++;
			else if ((*_it)->get_class_id() == Object::BUILDING)
				ienum[(*_it)->get_player_id()-1]++;
		}
		//ステータス描画
		for(unsigned int i=0;i<player.size();i++){
			//ゲージバー
			{
				SDL_Rect _rect= {
					states_rect.x+111*i+58-(int)(100.0-(double)player[i].base->hp/(double)PlayerBase().hp*100.0)/2,
					states_rect.y+23,
					0,0};
				SDL_Rect ra= {
					0,
					8*i,
					(int)((double)player[i].base->hp/(double)PlayerBase().hp*100.0),8};
				SDLFILESRFS::inst().get("image/life_bar.gif").Draw(systemmain->screen,&_rect,&ra,systemmain->screen->h);
			}
			//ゲージ枠
			{
				SDL_Rect _rect= {
					states_rect.x+111*i+54,
					states_rect.y+23,
					0,0};
				SDLFILESRFS::inst().get("image/life_frame.gif").Draw(systemmain->screen,&_rect,NULL,systemmain->screen->h);
			}
			//拠点HPの表示
			{
				sprintf(_str,"%d",player[i].base->hp);
				SDL_Rect _rect= {
					states_rect.x+111*i+40,
					states_rect.y+19,
					0,0};
				statusfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
			}
			//プレイヤー
			{
				SDL_Rect _rect= {
					states_rect.x+111*i+13,
					states_rect.y+states_rect.h,
					0,0};
				SDL_Rect ra= {
					0,
					20*i,
					34,20};
				SDLFILESRFS::inst().get("image/player.gif").Draw(systemmain->screen,&_rect,&ra,systemmain->screen->h);
			}
			//家のアイコンと表示
			{
				SDL_Rect _rect= {
					states_rect.x+111*i+10,
					states_rect.y+9,
					0,0};
				SDL_Rect ra= {
					0,
					16*i,
					16,16};
				SDLFILESRFS::inst().get("image/ie_icon_all.gif").Draw(systemmain->screen,&_rect,&ra,systemmain->screen->h);
				sprintf(_str,"%3d",ienum[i]);
				_rect.x += 10;
				_rect.y -= 4;
				statusfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
			}
			//兵士のアイコンと表示
			{
				SDL_Rect _rect= {
					states_rect.x+111*i+61,
					states_rect.y+9,
					0,0};
				SDL_Rect ra= {
					0,
					16*i,
					16,16};
				SDLFILESRFS::inst().get("image/heishi_icon_all.gif").Draw(systemmain->screen,&_rect,&ra,systemmain->screen->h);
				sprintf(_str,"%3d",peoplenum[i]);
				_rect.x += 10;
				_rect.y -= 4;
				statusfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
			}

		}

		//画面全体に関すること
		//どまんなか
		//全員終わって試合を終えるときに次の指示を書く。
		if(deadcount + 1 >= player.size()){
			//それぞれ勝敗が付いてる時に結果を書く
			//全員死んでるとき
			if(deadcount == player.size()){
				//引き分け
				SDL_Rect _rect= {systemmain->screen->w/2,systemmain->screen->h/2,0,0};
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,"draw",systemmain->screen->h);
			}
			//自分は死んでるとき
			else if(player[player_ctrl].base->hp <= 0){
				//負け
				SDL_Rect _rect= {systemmain->screen->w/2,systemmain->screen->h/2,0,0};
				SDLFILESRFS::inst().get("image/looser.gif").Draw(systemmain->screen,&_rect,NULL,systemmain->screen->h);
			}
			//他が全部死んでるとき
			else if(deadcount + 1 == player.size()){
				//勝ち
				SDL_Rect _rect= {systemmain->screen->w/2,systemmain->screen->h/2,0,0};
				SDLFILESRFS::inst().get("image/winner.gif").Draw(systemmain->screen,&_rect,NULL,systemmain->screen->h);
			}
			//共通の指示
			if(counter%52 < 40)
			{
				sprintf(_str,"Please wait...");
				SDL_Rect _rect= {
					systemmain->screen->w/2-(strlen(_str)*11)/2,
					systemmain->screen->h/2-100,
					0,0};
				systemmain->bmpfont.draw(systemmain->screen,_rect.x,_rect.y,_str,systemmain->screen->h);
			}
		}
		//最初のとき
		else if(counter < 200){
			SDL_Rect r = {systemmain->screen->w/2,systemmain->screen->h/2,0,0};
			SDL_Rect ra= {0,0,640,0};
			int temp = counter/10;
			if(counter<140){
				ra.y = 82*(14-temp);
				ra.h = 82;
				r.x += xor128()%(temp+1)-(temp+1)/2;
				r.y += xor128()%(temp+1)-(temp+1)/2;
				SDLFILESRFS::inst().get("image/areyouready.gif").Draw(systemmain->screen,&r,&ra,systemmain->screen->h);
			}
			else if(counter<150){
				r.y = (counter-140)*systemmain->screen->h/10 - systemmain->screen->h/2;
				ra.y=0; ra.h=120;
				SDLFILESRFS::inst().get("image/start.gif").Draw(systemmain->screen,&r,&ra,systemmain->screen->h);
				r.y += systemmain->screen->h/2 + systemmain->screen->h/2;
				 ra.y=0; ra.h=82;
				SDLFILESRFS::inst().get("image/areyouready.gif").Draw(systemmain->screen,&r,&ra,systemmain->screen->h);
			}
			else if(counter%4<2){
				ra.y=0; ra.h=120;
				SDLFILESRFS::inst().get("image/start.gif").Draw(systemmain->screen,&r,&ra,systemmain->screen->h);
			}
		}



	}

}
