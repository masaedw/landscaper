#ifndef _GAMEMAINOBJECT_H_
#define _GAMEMAINOBJECT_H_

#include "gamemain.h"
#include "ColTrees.h"

namespace space
{
	//前方宣言
	class GameMain;

	//============================
	//アブストラクト
	//GameMain::オブジェクトに関するもの
	//============================
	class Object
	{
	//無駄に多い定義
	public:
		//適当にtypedef
		typedef short STATUS;
		typedef unsigned short PLAYERID;
		//enumとか定数とか
		enum CLASSID {
			NONE = 0,
			PEOPLE,
			BULLET,
			BUILDING,
			IMPORTANCE,
			FIELD,
			EFFECT
		};
		enum ID {
			PEOPLE_NULL = 0,
			PEOPLE_PAWN,
			PEOPLE_GUNNER,
			BULLET_NULL = 0,
			BULLET_NORMAL,
			BULLET_BOMBFIRE,
			BULLET_METEO,
			BULLET_BOMB,
			BULLET_GROUNDBOMB,
			BUILDING_NULL = 0,
			BUILDING_NORMAL,
			BUILDING_FORTRESS,
			BUILDING_SHOP,
			BUILDING_SMITH,
			IMPORTANCE_NULL = 0,
			IMPORTANCE_GOD,
			IMPORTANCE_BASE,
			FIELD_NULL = 0,
			FIELD_VOLCANO,
			FIELD_ROCK,
			FIELD_TREE,
			FIELD_SABOTEN,
			EFFECT_NULL = 0,
			EFFECT_EXPLOSION,
			EFFECT_METEOPOINT,
			EFFECT_WARNING
		};
		static const PLAYERID PLAYER_NULL;

	//見せておかないと不便すぎるパラメータ
	public:
		//====================
		//共通パラメータ
		//====================
		//親
		GameMain *gamemain;
		//当たり判定用
		IKD::OBJECT_FOR_TREE<Object> formorton;

		//必要なくなったのかどうなのか、というフラグ
		bool isNeed;

		//各種パラメータ
		//重要なのは、これらのパラメータは使われる場合もあれば
		//使われない場合も圧倒的にあるということ。
		STATUS atk;	//攻撃
		STATUS def;	//防御
		double rpd;	//移動速度
		STATUS luk;	//運の強さ
		STATUS hp;		//ヒットポイント
		STATUS maketime;//作る時間(フレーム)
		STATUS price;	//値段

		double powerdist; //勢力距離

		//フィールド上での現在位置
		Matrix21<double> fieldpos;
		//フィールド上での速度
		Matrix21<double> fieldspd;
		//表示レイヤー(デフォルトは0)
		int zpos;

		//当たり判定用のオブジェクト
		CollisionDetective2D colobj;
		//基本的に円なのでここにおいておく。
		Circle2<double> maincircle;
		//当たり判定用。
		Circle2_COL2D colcircle;
		//モートン用
		Matrix21<double> mrect_lt,mrect_rb;
		//円のこといろいろ
		void revisionPosByCircle(Object *p);

		//アニメ用と決ってるわけではないが。
		int animecount;

	//隠蔽パラメータ
	protected:
		//どのクラスのオブジェクトなのかを指し示す
		CLASSID class_id;
		//オブジェクトが何なのかを指し示す
		ID object_id;
		//どのプレイヤーに属するオブジェクトなのかを示す
		PLAYERID player_id;

	public:

		//コンストラクタ
		Object();
		Object(GameMain *p, CLASSID _id = NONE);
		virtual ~Object() { formorton.Remove(); }

		//ID_get
		inline Object::CLASSID get_class_id(){ return class_id; }
		inline Object::ID get_object_id(){ return object_id; }
		inline Object::PLAYERID get_player_id(){ return player_id; }
		//攻撃する
		virtual void attackTo(Object* src);

		//デフォルトは用意するけど確実に実装してください。
		virtual void draw2D(){}
		virtual void draw3D(){}

		//手動で動かす時に使います。というか最終的には何時もこれで動かすように。
		virtual void move_manual();
		virtual void move(){animecount++;move_manual();}

		//当たり判定
		virtual void collision(Object* obj){}
		virtual void collision(std::list<Object*> &list){
			std::list<Object*>::iterator it = list.begin();
			for(;it!=list.end();it++){if(*it!=this){collision(*it);}}
		}

		//参照関係の解消に使えたりします。
		//まあ使いたければ使うでいいんじゃないでしょうか。
		virtual void after_collision(){}

		//パラメータのセット(実体を作るクラス全てで宣言すること)
		virtual void setParam() = 0;

		//実行順序
		//move -> collision -> after_collision -> draw2D,draw3D

	};

	//位置ソート用の関数オブジェクト
	class ObjectSortFuncByPos{
	public:
		bool operator()(const Object* lt, const Object* rt) const{
			return (
				//まずはzpos
				(lt->zpos < rt->zpos) ||
				//zposが同値ならｙで判断
				(lt->zpos==rt->zpos && lt->fieldpos.y > rt->fieldpos.y)
			);
		}
	};

	//======================
	//人間のアブストラクト
	//======================
	//まだそんなに定義はないわけだが
	class People  : public Object
	{
	public:
		Matrix21<double> dstpos;	//目的地
		int state;	//今の状態
	public:
		People(){}
		People(GameMain* p,ID _oid = PEOPLE_NULL);
		virtual ~People(){}
		
		//衝突判定
		virtual void collision(Object* obj);
		virtual void after_collision(){ if(hp<=0){ isNeed=false; hp=0; } }
		//動
		virtual void move(void){Object::move();}

	};


	//歩兵
	class PawnPeople : public People
	{
	public:

		PawnPeople();
		PawnPeople(GameMain* p,PLAYERID _pid,double _x,double _y);

		//2D描画
		inline void draw2D(void);

		//動く(updateみたいなもの、毎回実行するもの)
		inline void move(void);

		//パラメータセット
		inline void setParam();
	};

	//銃兵
	class Gunner : public People
	{
	public:
		int range;

	public:
		Gunner();
		Gunner(GameMain* p,PLAYERID _pid,double _x,double _y);

		//2D描画
		inline void draw2D(void);

		//動く(updateみたいなもの、毎回実行するもの)
		inline void move(void);

		//パラメータセット
		inline void setParam();
	};

	//=====================
	//弾丸のアブストラクト
	//======================
	//砂糖菓子ではない
	class Bullet : public Object{
	public:
		Bullet(){}
		Bullet(GameMain* p,ID _oid = BULLET_NULL);
		virtual ~Bullet();

		virtual void collision(Object* op);
		virtual void after_collision();
	};

	//普通の弾
	class NormalBullet : public Bullet
	{
	public:
		double distance;
		Matrix21<double> startpos;
	public:

		NormalBullet();
		NormalBullet(GameMain* p,PLAYERID _pid,double _x,double _y,double vx,double vy);

		virtual void draw2D(void);
		virtual void move(void);
		virtual void after_collision(void);

		//パラメータセット
		virtual void setParam(void);
	};

	//爆風
	class BombFire : public Bullet
	{
	public:
		unsigned int damageframe;
	public:

		BombFire();
		BombFire(GameMain* p,PLAYERID _pid,double _x,double _y,double _d,unsigned int frame);

		virtual void draw2D(void);
		virtual void move(void);
		virtual void after_collision(void);

		//パラメータセット
		virtual void setParam(void);
	};

	//メテオ
	class Meteo : public Bullet
	{
	public:
		Matrix21<double> dstpos;
		Matrix21<double> startpos;
		double PBa,PBb,PBc;
		Object* meteopointeffect;
	public:

		Meteo();
		Meteo(GameMain* p,PLAYERID _pid,double _x,double _y,double dx,double dy);

		virtual void draw2D(void);
		virtual void move(void);
		
		virtual void after_collision(void);

		//パラメータセット
		virtual void setParam(void);
	};

	//爆弾
	class Bomb : public Bullet
	{
	public:
		Bomb();
		Bomb(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);

		virtual void after_collision();


		//パラメータセット
		virtual void setParam();
	};

	//地雷
	class GroundBomb : public Bullet
	{
	public:
		GroundBomb();
		GroundBomb(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);

		virtual void after_collision();

		//パラメータセット
		virtual void setParam();
	};


	//=====================
	//建物のこと
	//======================
	class Building : public Object
	{
	public:
		int poolnum; //残ってる人の数

	public:
		Building(){}
		Building(GameMain* p,ID _oid = BUILDING_NULL);
		virtual ~Building(){}

		virtual void collision(Object* op);
		virtual void after_collision();

	};

	//普通の建物
	class NormalBuilding : public Building
	{
	public:
		NormalBuilding();
		NormalBuilding(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);
		virtual void move(void);
		//パラメータセット
		virtual void setParam();
	};

	//要塞
	class Fortress : public Building
	{
	public:
		Fortress();
		Fortress(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);
		virtual void move(void);
		//パラメータセット
		virtual void setParam();
	};

	//店
	class Shop : public Building
	{
	public:
		Shop();
		Shop(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);
		virtual void move(void);
		//パラメータセット
		virtual void setParam();
	};

	//鍛冶屋
	class Smith : public Building
	{
	public:
		Smith();
		Smith(GameMain* p,PLAYERID _pid,double _x,double _y);
		virtual void draw2D(void);
		virtual void move(void);
		//パラメータセット
		virtual void setParam();
	};

	//=============================
	//重要なもの
	//=============================
	//アブストラクト
	class Importance : public Object
	{
	public:
		Importance(){}
		Importance(GameMain* p,ID _oid = IMPORTANCE_NULL);
		virtual ~Importance(){}


	};


	//拠点
	class PlayerBase : public Importance
	{
	public:

		PlayerBase();
		PlayerBase(GameMain* p,PLAYERID _pid,double _x,double _y);

		virtual void draw2D(void);

		virtual void move(void);
		virtual void collision(Object* op);
		virtual void after_collision();

		//パラメータセット
		void setParam();
	};



	//===============================================
	//フィールドにある物体、または全体のことに関して。
	//===============================================
	class Field : public Object
	{
	public:
		Field(){}
		Field(GameMain* p,ID _oid = FIELD_NULL);
		virtual ~Field();
		
		virtual void after_collision(){
			if(hp<=0){isNeed=false;hp=0;}
		}
		virtual void collision(Object* op);
	};

	//火山
	class FieldVolcano : public Field
	{
	public:
		int distance;
		int warningfinish_cnt;
	public:
		FieldVolcano();
		FieldVolcano(GameMain* p,double _x,double _y);
		virtual void draw2D(void);
		virtual void move(void);

		virtual void collision(Object* op);
		virtual void after_collision(){
			if(hp<=0){hp=0;}
		}


		//パラメータセット
		virtual void setParam();
	};


	//岩
	class FieldRock : public Field
	{
	public:
		FieldRock();
		FieldRock(GameMain* p,double _x,double _y);
		virtual void draw2D(void);


		//パラメータセット
		virtual void setParam();
	};

	
	//木
	class FieldTree : public Field
	{
	public:
		FieldTree();
		FieldTree(GameMain* p,double _x,double _y);
		virtual void draw2D(void);

		//パラメータセット
		virtual void setParam();
	};

	//さぼてん
	class FieldSaboten : public Field
	{
	public:
		FieldSaboten();
		FieldSaboten(GameMain* p,double _x,double _y);
		virtual void draw2D(void);

		//パラメータセット
		virtual void setParam();
	};


	//==========
	//エフェクト
	//==========
	class Effect : public Object
	{
	public:
		Effect(){}
		Effect(GameMain* p,ID _oid) : Object(p,EFFECT){ object_id = _oid; player_id = PLAYER_NULL; }
		virtual ~Effect(){}
		virtual void move_manual(){Object::move_manual();formorton.Remove();}
	};

	//爆発エフェクト
	class ExplosionEffect : public Effect
	{
	public:
		ExplosionEffect();
		ExplosionEffect(GameMain* p,double _x,double _y);

		virtual void draw2D(void);
		virtual void move(void);

		//パラメータセット
		virtual void setParam();
	};

	//落下店表示エフェクト
	class MeteoPointEffect : public Effect
	{
	public:
		MeteoPointEffect();
		MeteoPointEffect(GameMain* p,double _x,double _y);

		virtual void draw2D(void);

		//パラメータセット
		virtual void setParam();
	};

	//ワーニングエフェクト
	class WarningEffect : public Effect
	{
	public:
		int display_cnt;
	public:
		WarningEffect();
		WarningEffect(GameMain* p,int cnt);

		virtual void draw2D(void);
		virtual void move(void);

		//パラメータセット
		virtual void setParam();
	};
}

#endif
