#include "gamemainobject.h"

namespace space
{
	//大切な定義
	const Object::PLAYERID Object::PLAYER_NULL = 0;
	//GameMain* Object::gamemain = NULL;

	//コンストラクタ
	Object::Object(){}
	Object::Object(GameMain *p, CLASSID _id)
	{
		gamemain = p;
		class_id = _id;
		isNeed=true;
		colcircle.circle = &maincircle;
		colobj.objs.push_back(&colcircle);
		animecount = 0;
		powerdist = 0.0;
		zpos = 0;

		// OFTに登録
		formorton.m_pObject = this;
	}

	inline void Object::move_manual(){
			fieldpos += fieldspd;
			maincircle.p = fieldpos;
			formorton.Remove();
			gamemain->mortontree.Regist(fieldpos.x-maincircle.d,fieldpos.y+maincircle.d,fieldpos.x+maincircle.d,fieldpos.y-maincircle.d,&formorton);
	}

	//攻撃する
	void Object::attackTo(Object* src){
		//hpが0のやつは放置することにする
		if(src->hp<=0)return;

		//ダメージ計算式
		//候補
		int dmg = (int)( (7.0*atk - 2.0*src->def) / 5.0 );

		//ダメージ補正
		//拠点とかで補正が入ったりするのかも
		//力拠点ボーナス
		if(player_id>0 && gamemain->player[player_id-1].typeofBase==BASETYPE_POWER)dmg+=std::max(2.0,dmg*0.6);
		if(src->get_player_id()>0 && gamemain->player[src->get_player_id()-1].typeofBase==BASETYPE_POWER)dmg-=std::max(1.0,dmg*0.12);

		//最後にこれ
		dmg = (dmg<=0) ? 1 : dmg;

		//その他特殊判定
		//普通の弾だけは同じチーム同士ならダメージを放置する
		if( ( (src->get_class_id()==BULLET && src->get_object_id()==BULLET_NORMAL) ||
			(         class_id  ==BULLET &&          object_id  ==BULLET_NORMAL) )
			&& src->get_player_id()==player_id){
				dmg = 0;
		}

		//普通にダメージを与える
		src->hp -= dmg;

		//ダメージがあるとき
		if(dmg>0){
			//弾は攻撃に回れば壊す。
			if(class_id==BULLET){hp=0;}
			//弾は攻撃されても壊す
			if(src->get_class_id()==BULLET){src->hp=0;}
		}

		//なにかしら倒したらのお話
		if(src->hp<=0 && player_id!=0){
			int addmoney=0;

			if(src->get_player_id()>0){gamemain->player[src->get_player_id()-1].bekilled_cnt++;}
			if(player_id>0){gamemain->player[player_id-1].killed_cnt++;}

			//人の場合
			if(src->get_class_id()==PEOPLE){
				addmoney += rand()%5 + 6;
			}
			//弾は壊してもなにもないよ
			else if(src->get_class_id()==BULLET){}
			//建物の場合
			else if(src->get_class_id()==BUILDING){
				addmoney += ((rand()%5 + 9)*pointer_cast<Building*>(src)->poolnum+rand()%10+21);
			}
			//拠点破壊
			else if(src->get_class_id()==IMPORTANCE && src->get_object_id()==IMPORTANCE_BASE){
				addmoney += 250;
			}

			//お金拠点ボーナス	
			if(player_id>0 && gamemain->player[player_id-1].typeofBase==BASETYPE_MONEY) addmoney+=std::max(1.0,addmoney*0.05);
			gamemain->player[player_id-1].money += addmoney;
		}
	}

	//当たり判定補正
	void Object::revisionPosByCircle(Object *p){
		//円の奴
		double theta = atan2(
			p->fieldpos.y - fieldpos.y,
			p->fieldpos.x - fieldpos.x);
		p->fieldpos.x = fieldpos.x + (maincircle.d/2+p->maincircle.d/2)*cos(theta);
		p->fieldpos.y = fieldpos.y + (maincircle.d/2+p->maincircle.d/2)*sin(theta);
		Matrix21<double> temp = p->fieldspd;
		p->fieldspd.x = 0.0; p->fieldspd.y = 0.0;
		p->move_manual();
		p->fieldspd = temp;
	}

	//実行順序
	//move -> collision -> after_collision -> draw2D,draw3D


	//人間のアブストラクト
	People::People(GameMain* p,ID _oid) : Object(p,PEOPLE)
	{
		object_id = _oid;
	}
	


	//衝突判定
	void People::collision(Object* op){
		//if(op==this)return;

		switch( op->get_class_id() )
		{
		//人と人
		case PEOPLE:
			if( colobj.getCollision(&op->colobj) )
			{
				//あたったらどかす。
				revisionPosByCircle(op);
			}
			break;
		}
	}



	//歩兵
	
	//パラメータセット	
	void PawnPeople::setParam(){
		atk = 13; def = 1; luk = 10; hp = 10; rpd = 1.0;
		maketime = 150; price = 10;
		maincircle.d = 10;
	}

	//コンストラクタ
	PawnPeople::PawnPeople() : People(NULL,PEOPLE_PAWN){setParam();}
	PawnPeople::PawnPeople(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : People(p,PEOPLE_PAWN)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		dstpos = fieldpos;
		fieldspd.x=0;fieldspd.y=0;

		player_id = _pid;
		maincircle.p = fieldpos;

		state = 0;

	}

	//2D描画
	void PawnPeople::draw2D(void)
	{
		//===================
		//SDLの描画
		//===================
		
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra= {0,0,24,24};
		ra.x = 24 * ((animecount/30)%2);
		ra.y = 24 * (player_id-1);
		SDLFILESRFS::inst().get("image/heishi_front_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	//アニメのためだけに作ったわけではない。
	void PawnPeople::move(){

		//衝突したら攻撃する
		Object* nearobj=NULL;
		for(std::list<Object*>::iterator it=gamemain->objects.begin();it!=gamemain->objects.end();it++){
			//ID
			CLASSID cid=(*it)->get_class_id();
			ID id=(*it)->get_object_id();
			PLAYERID pid=(*it)->get_player_id();
			//攻撃する人を選択
			bool foratk=false;
			switch(cid){
				//敵で、人か建物なら
				case PEOPLE: foratk=true;break;
				case BUILDING: foratk=true;break;
				case IMPORTANCE: if(id==IMPORTANCE_BASE){foratk=true;} break;
				default: break;
			}
			//攻撃する人に向けて送る処理。
			if(foratk){
				if(player_id!=pid && colobj.getCollision(&(*it)->colobj)){
					//攻撃タイミングなら攻撃
					if((animecount+1)%(unsigned int)(90.0/rpd) == 0){attackTo(*it);}
					//そうでなくてもあたったから、目標を変更する
					dstpos = (*it)->fieldpos;
				}
			}
		}

		//ただの移動時
		if(state == 0){
			//5フレ見る
			if(animecount % 5 == 0){
				//行きたい方に行く
				double theta = atan2(dstpos.y-fieldpos.y,dstpos.x-fieldpos.x) + (rand()%120-60)/180.0*M_PI;
				fieldspd.x=0.9*cos(theta); fieldspd.y=0.9*sin(theta);
			}
		}

		//動かす
		People::move();
	}


	//銃兵
	
	//パラメータセット	
	void Gunner::setParam(){
		atk = 8; def = 1; luk = 10; hp = 5; rpd = 1.0;
		maketime = 490; price = 40;
		maincircle.d = 10;
		range = 70;
	}

	//コンストラクタ
	Gunner::Gunner() : People(NULL,PEOPLE_GUNNER){setParam();}
	Gunner::Gunner(GameMain* p,PLAYERID _pid, double _x,double _y) : People(p,PEOPLE_GUNNER)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		dstpos = fieldpos;
		fieldspd.x=0;fieldspd.y=0;

		player_id = _pid;
		maincircle.p = fieldpos;

		state = 0;

	}

	//2D描画
	void Gunner::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra= {0,0,24,24};
		ra.x = 24 * ((animecount/30)%2);
		ra.y = 24 * (player_id-1);
		SDLFILESRFS::inst().get("image/zyuhei_front_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	//アニメのためだけに作ったわけではない。
	void Gunner::move(){

		//範囲内なら攻撃する
		if((animecount+1)%(unsigned int)(200.0/rpd)==0){
			//一番近いの。
			Object* nearobj=NULL;
			//範囲内にいる人を確認する
			for(std::list<Object*>::iterator it=gamemain->objects.begin();it!=gamemain->objects.end();it++){
				//ID
				CLASSID cid=(*it)->get_class_id();
				ID id=(*it)->get_object_id();
				PLAYERID pid=(*it)->get_player_id();
				
				//player_idが同じなら無視。
				if(player_id==pid)continue;

				//攻撃する人を選択
				bool foratk=false;
				switch(cid){
					//敵で、人か建物なら
					case PEOPLE: foratk=true;break;
					case BUILDING: foratk=true;break;
					case IMPORTANCE: if(id==IMPORTANCE_BASE){foratk=true;} break;
					default: break;
				}

				//攻撃対象のとき
				if(foratk){
					//もし最初かつレンジの範囲内か、
					//すでに選んだやつより近くなら選ぶ。
					if( ( nearobj==NULL && ((*it)->fieldpos-fieldpos).Abs2() < range*range ) ||
						( nearobj!=NULL && ((*it)->fieldpos-fieldpos).Abs2() < (nearobj->fieldpos-fieldpos).Abs2() ) ){
						//いれかえ
						nearobj = (*it);
					}
				}
			}

			//攻撃する人に向けて攻撃
			if(nearobj){
				Matrix21<double> v;
				v = nearobj->fieldpos-fieldpos;
				v *= (0.8/v.Abs());
				NormalBullet* p = new NormalBullet(gamemain,player_id,fieldpos.x,fieldpos.y,v.x,v.y);
				p->distance = range;
				gamemain->objects.push_back(p);
			
			}
			
		}

		//ただの移動時,5フレ見る
		if(animecount % 5 == 0){
			//行きたい方に行く
			double theta = atan2(dstpos.y-fieldpos.y,dstpos.x-fieldpos.x) + (rand()%120-60)/180.0*M_PI;
			fieldspd.x=0.8*cos(theta); fieldspd.y=0.8*sin(theta);
		}

		//動かす
		People::move();
	}



	//=========================
	//弾丸のアブストラクト
	//=======================
	Bullet::Bullet(GameMain* p,ID _oid) : Object(p,BULLET){
		object_id = _oid;
	}
	Bullet::~Bullet(){}

	void Bullet::collision(Object* op){
			//if(op==this)return;
			switch( op->get_class_id() )
			{
			case PEOPLE:
				if(colobj.getCollision(&op->colobj) ){
					attackTo(op);
					op->attackTo(this);
				}
				break;
			case BULLET:
				if(colobj.getCollision(&op->colobj) ){
					attackTo(op);
					op->attackTo(this);
				}
				break;
			default:
				break;
			}
	}
	//さいごに
	void Bullet::after_collision()
	{
		if(hp<=0){
			isNeed=false;
			hp = 0;
		}
	}



	//普通の弾

	//パラメータセット	
	void NormalBullet::setParam(){
		atk = 20; def = 1; luk = 0; hp = 1; rpd = 3.5;
		maketime = 120; price = 0;
		maincircle.d = 8;
		distance = 100;
	}

	//コンストラクタ
	NormalBullet::NormalBullet() : Bullet(NULL,BULLET_NORMAL){setParam();}
	NormalBullet::NormalBullet(GameMain* p,PLAYERID _pid,
			   double _x,double _y,double vx,double vy) : Bullet(p,BULLET_NORMAL)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=vx; fieldspd.y=vy;

		player_id = _pid;
		maincircle.p = fieldpos;
		startpos = fieldpos;
	}

	void NormalBullet::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra= {0,0,12,12};
		ra.x= 12 * ((animecount/9)%4);
		ra.y= 12 * (player_id-1);
		SDLFILESRFS::inst().get("image/Bullet_all_tamesi.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	void NormalBullet::move(void){
		if( (startpos-fieldpos).Abs2() >= distance*distance ) isNeed = false;
		Bullet::move();
	}

	void NormalBullet::after_collision(void){
		Bullet::after_collision();
		//死んだら爆風。
		if(hp<=0) gamemain->objects.push_back(new ExplosionEffect(gamemain,fieldpos.x,fieldpos.y));
	}



	//爆風

	//パラメータセット	
	void BombFire::setParam(){
		atk = 8; def = 100; luk = 0; hp = 0; rpd = 0;
		maketime = 0; price = 0;
		maincircle.d = 0.0;//何も言わなければ0です。
	}

	//コンストラクタ
	BombFire::BombFire() : Bullet(NULL,BULLET_BOMBFIRE){setParam();}
	BombFire::BombFire(GameMain* p,PLAYERID _pid,
			   double _x,double _y,double d,unsigned int frame) : Bullet(p,BULLET_BOMBFIRE)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=.0; fieldspd.y=.0;

		player_id = _pid;
		maincircle.p = fieldpos;
		maincircle.d = d;

		damageframe = frame;
	}

	void BombFire::draw2D(void)
	{
	}

	void BombFire::move(void){
		//自前が一番。だが面倒。
		//というわけで半分自前。
		if(animecount%15 -1 == 0){
			gamemain->objects.push_back(new ExplosionEffect(gamemain,fieldpos.x,fieldpos.y));
		}

		//カウントに到達したら終わる。
		if(  animecount >= (int)damageframe ) isNeed = false;
		Bullet::move();
	}

	void BombFire::after_collision(){
		//こいつは死なない。カウントまでは。
		if(hp<=0){hp=0;}
	}

	//火山弾

	//パラメータセット	
	void Meteo::setParam(){
		atk = 200; def = 200; luk = 0; hp = 200; rpd = 30.0;
		maketime = 60; price = 0;
		maincircle.d = 48;
	}

	//コンストラクタ
	Meteo::Meteo() : Bullet(NULL,BULLET_METEO){setParam();}
	Meteo::Meteo(GameMain* gp,PLAYERID _pid,
			   double _x,double _y,double dx,double dy) : Bullet(gp,BULLET_METEO)
	{
		setParam();

		if(fabs(_x-dx)<0.001)dx=_x+0.001;

		fieldpos.x=_x; fieldpos.y=_y;
		dstpos.x = dx; dstpos.y = dy;


		player_id = _pid;
		maincircle.p = fieldpos;
		startpos = fieldpos;


		//表示位置は上。
		zpos = 127;
		//横方向速度は割りとゆっくり目に。
		fieldspd.x = std::min( fabs(dstpos.x-startpos.x)/240.0, 0.5 );
		fieldspd.x *= (dstpos.x>startpos.x)?1.0:-1.0;
		//放物線のいろいろ。
		Matrix21<double> p;
		p.y = std::max(dstpos.y,startpos.y)+250.0;
		p.x = startpos.x+(dstpos.x-startpos.x)*0.5;
		double A,B,C,D,E,F,G;
		A=startpos.x*startpos.x-dstpos.x*dstpos.x;
		B=startpos.x-dstpos.x;
		C=startpos.y-dstpos.y;
		D=startpos.x*startpos.x-p.x*p.x;
		E=startpos.x-p.x;
		F=startpos.y-p.y;
		G=B/E;
		//式計算
		PBa = (C-G*F)/(A-G*D);
		PBb = (C-A*PBa)/B;
		PBc = startpos.y-PBa*startpos.x*startpos.x-PBb*startpos.x;

		//最初は当たり判定をなくす
		colobj.objs.clear();

		//落下点表示
		meteopointeffect = new MeteoPointEffect(gamemain,dstpos.x,dstpos.y);
		gamemain->objects.push_back(meteopointeffect);

	}

	void Meteo::draw2D(void)
	{
		//位置
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		//エフェクト
		static SDL_Rect ra = {0,0,184,184};
		ra.x= ( animecount%5 )*ra.w;
		if(fieldspd.y>0.0){
			if(fieldspd.y<0.5){
				ra.y= ( (animecount/5)%1   )*ra.h;
			}
			else if(fieldspd.y<1.0){
				ra.y= ( (animecount/5)%1+1 )*ra.h;
			}
			else if(fieldspd.y<1.5){
				ra.y= ( (animecount/5)%1+2 )*ra.h;
			}
			else{
				ra.y= ( (animecount/5)%2+3 )*ra.h;
			}
			r.y -= 50;
			SDLFILESRFS::inst().get("image/meteo_fire_back.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
			r.y += 50;
		}
		else{
			if(fieldspd.y>-0.5){
				ra.y= ( (animecount/5)%1   )*ra.h;
			}
			else if(fieldspd.y>-1.0){
				ra.y= ( (animecount/5)%1+1 )*ra.h;
			}
			else if(fieldspd.y>-1.5){
				ra.y= ( (animecount/5)%1+2 )*ra.h;
			}
			else{
				ra.y= ( (animecount/5)%2+3 )*ra.h;
			}
			r.y += 50;
			SDLFILESRFS::inst().get("image/meteo_fire.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
			r.y -= 50;
		}
		//画像本体
		static SDL_Rect rb = {0,0,48,48};
		SDLFILESRFS::inst().get("image/meteo.gif").Draw(gamemain->systemmain->screen,&r,&rb,gamemain->systemmain->screen->h);
	}

	void Meteo::move(void){
		//計算した位置から速度を逆算する。
		Matrix21<double> nextpos;
		nextpos.x = fieldpos.x+fieldspd.x;
		nextpos.y = PBa*nextpos.x*nextpos.x+PBb*nextpos.x+PBc;
		fieldspd=nextpos-fieldpos;
		

		//あとはいつもどおり。
		Bullet::move();
		//目標地点に到達したようなら当たり判定を復活させる。
		if( (dstpos-fieldpos).Abs2() < 1.0 ) colobj.objs.push_back(&colcircle);
	}

	void Meteo::after_collision(void){
		Bullet::after_collision();
		//当たり判定を出現させたそのターンに必ず破壊する。
		//死んだら爆風。
		if(colobj.objs.size()){
			hp=0;
			isNeed=false;
			gamemain->objects.push_back(new ExplosionEffect(gamemain,fieldpos.x,fieldpos.y));
			//落下点消去
			meteopointeffect->isNeed = false;
		}
	}


	//爆弾
	
	//パラメタ
	void Bomb::setParam(void){
		atk = 0; def = 0; luk = 0; hp = 1; rpd = 0.0;
		maketime = 0; price = 120;
		maincircle.d = 20;
	}

	//コンストラクタ
	Bomb::Bomb() : Bullet(NULL,BULLET_BOMB){setParam();}
	Bomb::Bomb(GameMain* p,PLAYERID _pid,double _x,double _y)
		: Bullet(p,BULLET_BOMB)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = _pid;

	}

	void Bomb::draw2D(void){
		//===================
		//SDLの描画
		//===================
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect rs= {0,0,32,32};
		rs.x = 32*((animecount/6)%3);
		SDLFILESRFS::inst().get("image/bomb.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}

	//さいごに
	void Bomb::after_collision()
	{
		if(hp<=0){
			//滅びて
			isNeed=false;
			hp = 0;
			//爆風
			gamemain->objects.push_back(new BombFire(gamemain,player_id,fieldpos.x,fieldpos.y,32,15));
		}
	}

	//地雷。
	//つまりそれは見えない爆弾

	//パラメタ
	void GroundBomb::setParam(void){
		atk = 0; def = 0; luk = 0; hp = 1; rpd = 0.0;
		maketime = 0; price = 140;
		maincircle.d = 20;
	}

	//コンストラクタ
	GroundBomb::GroundBomb() : Bullet(NULL,BULLET_GROUNDBOMB){setParam();}
	GroundBomb::GroundBomb(GameMain* p,PLAYERID _pid,double _x,double _y)
		: Bullet(p,BULLET_GROUNDBOMB)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = _pid;

	}

	void GroundBomb::draw2D(void){
		//自分以外見えないよ
		if(gamemain->player_ctrl==(player_id-1)){
			static SDL_Rect r = {0,0,0,0};
			r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
			r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
			static SDL_Rect rs ={0,0,32,32};
			rs.y= 32*(player_id-1);
			SDLFILESRFS::inst().get("image/groundbomb.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
		}
	}

	//さいごに
	void GroundBomb::after_collision()
	{
		if(hp<=0){
			//滅びて
			isNeed=false;
			hp = 0;
			//爆風
			gamemain->objects.push_back(new BombFire(gamemain,player_id,fieldpos.x,fieldpos.y,30,14));
		}
	}



	//=======================
	//建物のこと
	//=======================
	Building::Building(GameMain* p,ID _oid) : Object(p,BUILDING){
		object_id = _oid;
	}

	void Building::collision(Object* op){
			//if(op==this)return;

			switch( op->get_class_id() )
			{
			case PEOPLE:
				//人と建物
				if( colobj.getCollision(&op->colobj) ){
					//円前提でどかす
					People *p = pointer_cast<People*>(op);
					revisionPosByCircle(p);
				}
				break;

			case BULLET:
				//弾はダメージ判定あり
				if( colobj.getCollision(&op->colobj) ){
					//家はダメージ
					op->attackTo(this);
				}
				break;

			case BUILDING:
				//他のやつは移動判定しかない。
				if( colobj.getCollision(&op->colobj) ){
					//円の奴
					revisionPosByCircle(op);
				}
				break;
			default:
				break;
			}
	}


	//さいごに
	void Building::after_collision()
	{
		if(hp<=0){
			isNeed=false;
			hp = 0;
		}
	}


	//普通の建物

	//パラメータセット	
	void NormalBuilding::setParam(){
		atk = 0; def = 10; luk = 0; hp = 100; rpd = 1.0;
		maketime = 10; price = 80;
		maincircle.d = 20;

		poolnum = 8;
		powerdist = 60.0;
	}

	//コンストラクタ
	NormalBuilding::NormalBuilding() : Building(NULL,BUILDING_NORMAL){setParam();}
	NormalBuilding::NormalBuilding(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : Building(p,BUILDING_NORMAL)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x= 0; fieldspd.y= 0;

		player_id = _pid;
		maincircle.p = fieldpos;

		//愛の拠点ボーナス
		if(gamemain->player[player_id-1].typeofBase==BASETYPE_LOVE){poolnum*=1.5;}
	}

	//出すよ！
	void NormalBuilding::move(void){
		//人を出します
		if((animecount+1)%PawnPeople().maketime == 0){
			if(poolnum > 0){ gamemain->setPawn(player_id,fieldpos.x,fieldpos.y-maincircle.d/2); poolnum--;}
		}
		//ほかのこと
		Building::move();
	}

	//2D
	void NormalBuilding::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra = {0,0,32,32};
		ra.y= 32 * (player_id-1);
		SDLFILESRFS::inst().get("image/ie_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}


	//要塞

	//パラメータセット	
	void Fortress::setParam(){
		atk = 3; def = 23; luk = 10; hp = 85; rpd = 1.0;
		maketime = 10; price = 200;
		maincircle.d = 20;

		poolnum = 15;
		powerdist = 80.0;
	}

	//コンストラクタ
	Fortress::Fortress() : Building(NULL,BUILDING_FORTRESS){setParam();}
	Fortress::Fortress(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : Building(p,BUILDING_FORTRESS)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x= 0; fieldspd.y= 0;

		player_id = _pid;
		maincircle.p = fieldpos;

		//愛の拠点ボーナス
		if(gamemain->player[player_id-1].typeofBase==BASETYPE_LOVE){poolnum*=1.5;}
	}

	//2D
	void Fortress::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra ={0,0,32,32};
		ra.y= 32 * (player_id-1);
		SDLFILESRFS::inst().get("image/yousai_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	//弾打ったりしますからまぁ。
	void Fortress::move(){
		//打つ処理
		if((animecount+1)%(unsigned int)(120.0/rpd) == 0){
			//狙える範囲で。
			Object* nearobj=NULL;
			for(std::list<Object*>::iterator it=gamemain->objects.begin();it!=gamemain->objects.end();it++){
				double dist = ((*it)->fieldpos-fieldpos).Abs();
				//距離が近く、誰よりも近く、それが敵であるなら狙う。
				if(
					(*it)->get_player_id()!=player_id && (*it)->get_player_id()!=PLAYER_NULL &&
					dist<=120.0 &&
					(!nearobj || dist<=(nearobj->fieldpos-fieldpos).Abs())
					){
					nearobj=(*it);
				}
			}
			//打つ方向決定
			if(nearobj){
				Matrix21<double> dir=nearobj->fieldpos-fieldpos;
				dir /= dir.Abs();
				dir *= 0.6;
				//出す
				gamemain->setNormalBullet(player_id,fieldpos.x,fieldpos.y,dir.x,dir.y);
			}
		}
		//人を出します
		if((animecount+1)%PawnPeople().maketime == 0){
			if(poolnum > 0){ gamemain->setPawn(player_id,fieldpos.x,fieldpos.y-maincircle.d/2); poolnum--;}
		}
		//その他の処理
		Building::move();
	}


	//お店

	//パラメータセット	
	void Shop::setParam(){
		atk = 0; def = 11; luk = 10; hp = 50; rpd = 0.0;
		maketime = 10; price = 50;
		maincircle.d = 20;

		poolnum = 2;
		powerdist = 20.0;
	}

	//コンストラクタ
	Shop::Shop() : Building(NULL,BUILDING_SHOP){setParam();}
	Shop::Shop(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : Building(p,BUILDING_SHOP)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x= 0; fieldspd.y= 0;

		player_id = _pid;
		maincircle.p = fieldpos;

		//愛の拠点ボーナス
		if(gamemain->player[player_id-1].typeofBase==BASETYPE_LOVE){poolnum*=1.5;}
	}

	//2D
	void Shop::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra = {0,0,32,32};
		ra.y= 32 * (player_id-1);
		SDLFILESRFS::inst().get("image/shop_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	//こいつはおかねがふえます
	void Shop::move(){
		//お金が増えるよ
		if((animecount+1)%720 == 0){
			gamemain->player[player_id-1].money += rand()%8+8;
			//金の拠点ボーナス
			if(gamemain->player[player_id-1].typeofBase==BASETYPE_MONEY){gamemain->player[player_id-1].money += rand()%1+1;}
		}
		//人を出します
		if((animecount+1)%PawnPeople().maketime == 0){
			if(poolnum > 0){ gamemain->setPawn(player_id,fieldpos.x,fieldpos.y-maincircle.d/2); poolnum--;}
		}
		//ほかのこと
		Building::move();
	}


	//鍛冶屋

	//パラメータセット	
	void Smith::setParam(){
		atk = 0; def = 10; luk = 10; hp = 100; rpd = 0.0;
		maketime = 10; price = 270;
		maincircle.d = 20;

		poolnum = 10;
		powerdist = 20.0;
	}

	//コンストラクタ
	Smith::Smith() : Building(NULL,BUILDING_SMITH){setParam();}
	Smith::Smith(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : Building(p,BUILDING_SMITH)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x= 0; fieldspd.y= 0;

		player_id = _pid;
		maincircle.p = fieldpos;

		//愛の拠点ボーナス
		if(gamemain->player[player_id-1].typeofBase==BASETYPE_LOVE){poolnum*=1.5;}
	}

	//2D
	void Smith::draw2D(void)
	{
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect ra = {0,0,32,32};
		ra.x= 32*((animecount/30)%2);
		ra.y= 32 * (player_id-1);
		SDLFILESRFS::inst().get("image/smith_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
	}

	//銃兵が出ます
	void Smith::move(){
		//人を出します
		if((animecount+1)%Gunner().maketime == 0){
			if(poolnum > 0){ gamemain->objects.push_back(new Gunner(gamemain,player_id,fieldpos.x,fieldpos.y-maincircle.d/2)); poolnum--;}
		}
		//ほかのこと
		Building::move();
	}


	//=====================
	//重要なもの
	//=====================
	Importance::Importance(GameMain* p,ID _oid) : Object(p,IMPORTANCE){
		object_id = _oid;
	}


	//拠点について
	//パラメタ設定
	void PlayerBase::setParam(){
		atk = 0; def = 20; luk = 100; hp = 500; rpd = 0.1;
		maketime = 0; price = 0;
		maincircle.d = 28;
		powerdist = 140.0;
	}

	//コンストラクタ
	PlayerBase::PlayerBase() : Importance(NULL,IMPORTANCE_BASE){setParam();}
	PlayerBase::PlayerBase(GameMain* p,PLAYERID _pid,
			   double _x,double _y) : Importance(p,IMPORTANCE_BASE)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		player_id = _pid;
		maincircle.p = fieldpos;
	}

	void PlayerBase::collision(Object* op){
			//if(op==this)return;
			switch( op->get_class_id() )
			{
			//人はとりあえずどかす。
			case PEOPLE:
				if( colobj.getCollision(&op->colobj) ){
					revisionPosByCircle(op);
				}
				break;
			//弾は攻撃して死ぬ
			case BULLET:
				if( colobj.getCollision(&op->colobj) ){
					op->attackTo(this);
					attackTo(op);
				}
				break;
			//建物は補正
			case BUILDING:
				if( colobj.getCollision(&op->colobj) ){
					revisionPosByCircle(op);
				}
				break;
			//その他スルー
			case IMPORTANCE:
				break;
			default:
				break;
			}
	}
	//さいごに
	void PlayerBase::after_collision()
	{
		if(hp<=0){
			hp = 0;
			colobj.objs.clear();
		}
	}

	void PlayerBase::move(void)
	{
		//生きてるとき
		if(hp>0){
			//お金が増えるよ
			if((animecount+1)%600 == 0){
				gamemain->player[player_id-1].money += rand()%3+8;
			//金の拠点ボーナス
			if(gamemain->player[player_id-1].typeofBase==BASETYPE_MONEY){gamemain->player[player_id-1].money += rand()%2+1;}
			}
		}
		//あとはいつもどおり。
		Object::move();
	}

	void PlayerBase::draw2D(void)
	{
		if(hp<=0)return;

		//===================
		//SDLの描画
		//===================
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y-4);
		static SDL_Rect ra = {64,64,64,64};
		static int ax;
		ax = animecount%256 - 128;
		ax = ( (ax*ax*ax)/(128*128) + 128 )/16;
		ra.x= 64 * ax;
		ra.y= 64 * (player_id-1);
		//拠点の画像表示
		SDLFILESRFS::inst().get("image/bace_all.gif").Draw(gamemain->systemmain->screen,&r,&ra,gamemain->systemmain->screen->h);
		//名前表示の場所
		r.x -= gamemain->systemmain->bmpfont.getW()*gamemain->player[player_id-1].name.length()/2;
		r.y -= (gamemain->systemmain->bmpfont.getH()/2+16);
		gamemain->systemmain->bmpfont.draw(gamemain->systemmain->screen,r.x,r.y,gamemain->player[player_id-1].name.c_str(),gamemain->systemmain->screen->h);
	}



	//===============================================
	//フィールドにある物体、または全体のことに関して
	//==============================================
	Field::Field(GameMain* p,ID _oid) : Object(p,FIELD){
		object_id = _oid;
		player_id = PLAYER_NULL; //基本的にNULL
	}
	Field::~Field(){}

	void Field::collision(Object *op){

		switch(op->get_class_id()){
		//人はどける。建物もどける。
		case PEOPLE:
		case BUILDING:
			if( colobj.getCollision(&op->colobj) ){
				revisionPosByCircle(op);
			}
			break;
		//弾に当たったら攻撃を受ける
		case BULLET:
			if( colobj.getCollision(&op->colobj) ){
				op->attackTo(this);
			}
			break;
		//重要物は拠点だけ自分を補正
		case IMPORTANCE:
			if( op->get_object_id()==IMPORTANCE_BASE && colobj.getCollision(&op->colobj) ){
				op->revisionPosByCircle(this);
			}
			break;
		//フィールドオブジェクトは自分を補正
		case FIELD:
			if( colobj.getCollision(&op->colobj) ){
				op->revisionPosByCircle(this);
			}
			break;
		//その他スルー(いいのかよ)
		default:
			break;
		}
	}

	//火山

	//パラメタ
	void FieldVolcano::setParam(void){
		atk = 0; def = 30000; luk = 30000; hp = 30000; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 95;
	}

	//コンストラクタ
	FieldVolcano::FieldVolcano() : Field(NULL,FIELD_VOLCANO){setParam();}
	FieldVolcano::FieldVolcano(GameMain* p,double _x,double _y)
		: Field(p,FIELD_VOLCANO)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = PLAYER_NULL;
		distance = 250;
		warningfinish_cnt = -1;

	}

	void FieldVolcano::draw2D(void)
	{
		//位置
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y+20.0);
		static SDL_Rect rs = { 0,0,128,128 };
		rs.x= 128*((animecount/9)%6);
		SDLFILESRFS::inst().get("image/volcano_anim.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}

	void FieldVolcano::collision(Object* op){
		//if(op==this)return;

		switch(op->get_class_id()){
		//人は位置補正
		case PEOPLE:
			if( colobj.getCollision(&op->colobj) ){
				revisionPosByCircle(op);
			}
			break;
		//弾は破壊
		case BULLET:
			if(colobj.getCollision(&op->colobj)){
				attackTo(op);
			}
			break;
		//建物は補正
		case BUILDING:
			if( colobj.getCollision(&op->colobj) ){
				revisionPosByCircle(op);
			}
			break;
		//重要物はスルーだが
		case IMPORTANCE:
			//拠点だけ別。
			if( op->get_object_id()==IMPORTANCE_BASE && colobj.getCollision(&op->colobj) ){
				revisionPosByCircle(op);
			}
			break;
		//フィールドオブジェクトは補正
		case FIELD:
			if( colobj.getCollision(&op->colobj) ){
				revisionPosByCircle(op);
			}
			break;
		//その他スルー
		default:
			break;
		}
	}

	//火山弾を打つらしい
	void FieldVolcano::move(){
		Field::move();

		//やばい状況のとき
		if(animecount < warningfinish_cnt){
			//終了までのカウント
			int c = warningfinish_cnt - animecount;
			//打ちます
			if(c%30==0) gamemain->objects.push_back(new Meteo(gamemain,0,fieldpos.x,fieldpos.y+50,rand()%(distance*2)-distance+fieldpos.x,rand()%(distance*2)-distance+fieldpos.y) );
		}
		//やばくないとき
		else{
			//もしかしたらやばくなるかもしれない
			if(rand()%2100==0){
				warningfinish_cnt = animecount + 200;
				gamemain->objects.push_back(new WarningEffect(gamemain,200));
				gamemain->systemmain->sound_warning.play(5);
			}
		}
	}


	//石のような物体
	//パラメタ
	void FieldRock::setParam(void){
		atk = 0; def = 1000; luk = 0; hp = 1000; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 20;
	}

	//コンストラクタ
	FieldRock::FieldRock() : Field(NULL,FIELD_ROCK){setParam();}
	FieldRock::FieldRock(GameMain* p,double _x,double _y)
		: Field(p,FIELD_ROCK)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = 0;


	}

	void FieldRock::draw2D(void){
		//位置
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect rs = { 0,0,32,32 };
		SDLFILESRFS::inst().get("image/rock.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}


	//木
	//パラメタ
	void FieldTree::setParam(void){
		atk = 0; def = 1000; luk = 0; hp = 1000; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 20;
	}

	//コンストラクタ
	FieldTree::FieldTree() : Field(NULL,FIELD_TREE){setParam();}
	FieldTree::FieldTree(GameMain* p,double _x,double _y)
		: Field(p,FIELD_TREE)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = 0;

	}

	void FieldTree::draw2D(void){
		//位置
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect rs = { 0,0,32,32 };
		SDLFILESRFS::inst().get("image/tree.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}

	//サボテン
	//パラメタ
	void FieldSaboten::setParam(void){
		atk = 0; def = 1000; luk = 0; hp = 500; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 10;
	}

	//コンストラクタ
	FieldSaboten::FieldSaboten() : Field(NULL,FIELD_SABOTEN){setParam();}
	FieldSaboten::FieldSaboten(GameMain* p,double _x,double _y)
		: Field(p,FIELD_SABOTEN)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = 0;

	}

	void FieldSaboten::draw2D(void){
		//位置
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect rs = { 0,0,24,24 };
		SDLFILESRFS::inst().get("image/saboten.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}


	//===============================================
	//とってもエフェクト
	//==============================================

	//爆破エフェクト

	//パラメタ
	void ExplosionEffect::setParam(void){
		atk = 0; def = 0; luk = 0; hp = 0; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 0;
	}

	//コンストラクタ
	ExplosionEffect::ExplosionEffect() : Effect(NULL,EFFECT_EXPLOSION){setParam();}
	ExplosionEffect::ExplosionEffect(GameMain* p,double _x,double _y)
		: Effect(p,EFFECT_EXPLOSION)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;

		player_id = PLAYER_NULL;

	}

	void ExplosionEffect::draw2D(void){
		//===================
		//SDLの描画
		//===================
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static int c;
		static SDL_Rect rs = { 0,0,72,72 };
		c = (int)((double)animecount/1.5);
		rs.x = 72*(c%8);
		rs.y = 72*(c/8);
		SDLFILESRFS::inst().get("image/exp02.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}
	void ExplosionEffect::move(){
		Effect::move();
		if((double)animecount/1.5>=32.0) isNeed = false;
	}


	//落下店表示エフェクト

	//パラメタ
	void MeteoPointEffect::setParam(void){
		atk = 0; def = 0; luk = 0; hp = 0; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 0;
	}

	//コンストラクタ
	MeteoPointEffect::MeteoPointEffect() : Effect(NULL,EFFECT_METEOPOINT){setParam();}
	MeteoPointEffect::MeteoPointEffect(GameMain* p,double _x,double _y)
		: Effect(p,EFFECT_METEOPOINT)
	{
		setParam();

		fieldpos.x=_x; fieldpos.y=_y;
		fieldspd.x=0.0;fieldspd.y=0.0;

		maincircle.p = fieldpos;
		zpos = -256;

		player_id = PLAYER_NULL;

	}

	void MeteoPointEffect::draw2D(void){
		//===================
		//SDLの描画
		//===================
		static SDL_Rect r = {0,0,0,0};
		r.x = (Sint16)(fieldpos.x+(double)gamemain->field_rect.x+gamemain->player[gamemain->player_ctrl].drawoffset.x);
		r.y = (Sint16)(fieldpos.y+(double)gamemain->field_rect.y+gamemain->player[gamemain->player_ctrl].drawoffset.y);
		static SDL_Rect rs = { 0,0,96,96 };
		rs.x = 96*((animecount/6)%5);
		SDLFILESRFS::inst().get("image/meteo_point.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}


	//Warningエフェクト

	//パラメタ
	void WarningEffect::setParam(void){
		atk = 0; def = 0; luk = 0; hp = 0; rpd = 0.0;
		maketime = 0; price = 0;
		maincircle.d = 0;
	}

	//コンストラクタ
	WarningEffect::WarningEffect() : Effect(NULL,EFFECT_WARNING){setParam();}
	WarningEffect::WarningEffect(GameMain* p,int cnt)
		: Effect(p,EFFECT_WARNING)
	{
		setParam();

		fieldpos.x=0.0; fieldpos.y=0.0;
		fieldspd.x=0.0; fieldspd.y=0.0;

		maincircle.p = fieldpos;
		zpos = 255;

		player_id = PLAYER_NULL;
		display_cnt = cnt;

	}

	void WarningEffect::draw2D(void){
		//===================
		//SDLの描画
		//===================
		static SDL_Rect r = { 0,0,0,0};
		r.x = (Sint16)(gamemain->field_rect.w/2 + gamemain->field_rect.x);
		r.y = (Sint16)(gamemain->field_rect.h/2 + gamemain->field_rect.y);
		static SDL_Rect rs = { 0,0,89,78 };

		//時間的に2:1の割合で表示
		if(animecount%39<26)
			SDLFILESRFS::inst().get("image/warning.gif").Draw(gamemain->systemmain->screen,&r,&rs,gamemain->systemmain->screen->h);
	}

	void WarningEffect::move(){
		if(animecount > display_cnt)isNeed=false;
		Effect::move();
	}

}
