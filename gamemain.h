#ifndef _GAMEMAIN_H_
#define _GAMEMAIN_H_

#include "systemstate.h"
#include "gamemainobject.h"
#include "input.h"
#include "ColTrees.h"

namespace space
{
	class Object;
	class PawnPeople;
	class NormalBullet;
	class Fortress;
	class NormalBuilding;
	class Bomb;
	class GroundBomb;
	class Shop;
	class Smith;

	enum{
		BASETYPE_MONEY = 0,
		BASETYPE_LOVE,
		BASETYPE_POWER
	};

	//フィールドのクラス
	class GameMain : public SystemState
	{
	public:
		unsigned int deadcount;
	public:
		//プレイヤー
		struct Player{

			//==========
			//得点関連
			//==========
			unsigned int score;
			unsigned int killed_cnt;
			unsigned int bekilled_cnt;
			int spendmoney;

			Object* base; //拠点
			unsigned int typeofBase; //拠点タイプ

			//============
			//持ち物関連
			//============
			int money; //お金
			unsigned char items[64]; //配列番号=アイテムの番号 中身=アイテムの数

			//==========
			//入力関連
			//==========
			const Input *input;
			Matrix21<int> bmousepos;
			unsigned short pushtime_left;
			unsigned short pushtime_middle;
			unsigned short pushtime_right;
			bool ispull_left;
			bool ispull_middle;
			bool ispull_right;
			//key=ボタン番号 , value=押されたフレーム数
			std::map<unsigned short,unsigned short> pushtime_key;
			std::map<unsigned short,unsigned short> ispull_key; 

			//==========
			//選択関連
			//==========
			//いま、選択しているオブジェクト。
			//決してこの配列でdeleteしないように。
			std::list<Object*> selected;
			//いま、選択しているアイテムの番号。
			//0で選択していない。
			unsigned int selected_item;
			//表示されているアイテム
			unsigned short shownitems[12];
			//表示されているアイテムの種類
			unsigned short sortofitems;
			//表示オフセット
			Matrix21<double> drawoffset;
			Matrix21<double> drawoffset_b;
			//なまえ
			std::string name;

			//お金を使う
			bool spendMoney(int um){
				if(money >= um){
					money -= um;
					spendmoney += um;
					return true;
				}
				return false;
			}

		};

		//アイテム（設置できるもの全部まとめてこう言うことにする）
		struct Item{
			//アイテムの画像と切り取り位置
			const SDLSURFACE* showpic;
			WinRect r;
			//設置するオブジェクト
			Object* obj;

			Item(){
				showpic = &SDLFILESRFS::inst().get("image/default.gif");
				obj = NULL;
				r.x=0; r.y=0; r.w=32; r.h=32;
			}
		};

		//フィールド画面の大きさ
		WinRect field_rect;
		//コントロール画面の大きさ
		WinRect control_rect;
		WinRect items_rect[12];
		//情報表示画面の大きさ
		WinRect states_rect;
		//お金表示画面の大きさ
		WinRect money_rect;
		//データ表示ウインドウ
		WinRect data_rect;

		//お金用のフォント
		BMPFONT moneyfont;
		//ステータスのフォント
		BMPFONT statusfont;

		//アイテムの種類表示するとこの画像の配列
		std::vector<const SDLSURFACE*> sortofitems_drawpict;
		//種類変更するとこ。
		WinRect flippos;

		//オブジェクト全部
		std::list<Object*> objects;
		IKD::CLiner4TreeManager<Object> mortontree;
		IKD::CollisionList<Object> *mortoncollist;

		//乱数の種
		unsigned int randamseed;

		//playerに関するもの
		std::vector<Player> player;
		//自分の番号
		unsigned int player_ctrl;
		
		//リプレイファイル
		std::fstream replayfile;
		bool setrep;

		//itemに関するもの
		std::vector<Item> item;

		//おわり用のカウント
		unsigned int forEnd_cnt;


	public:

		PawnPeople* setPawn(unsigned short _pid, double _x, double _y);
		NormalBullet* setNormalBullet(unsigned short _pid, double _x, double _y, double vx, double vy);
		NormalBuilding* setNormalBuilding(unsigned short _pid, double _x,double _y);
		Fortress* setFortress(unsigned short _pid, double _x,double _y);
		Bomb* setBomb(unsigned short _pid, double _x,double _y);
		GroundBomb* setGroundBomb(unsigned short _pid, double _x,double _y);
		Shop* setShop(unsigned short _pid, double _x,double _y);
		Smith* setSmith(unsigned short _pid, double _x, double _y);

		void changePlayerCtrl(unsigned int);

		GameMain(ProgramSystemMain* _sys, std::vector<Player> _pl,unsigned int _ctrlpl, unsigned int _rands,bool _rep=true);
		GameMain(const GameMain &temp);
		~GameMain();

		void setting3D();

		void update();
		void draw();
		void DisplayReload();

		inline int CollisionAllObjects(Object*** oppp){
			int mortoncolnum = mortontree.GetAllCollisionList(&mortoncollist);
			*oppp = mortoncollist->getRootPtr();
			return mortoncolnum;
		}

	};
}

#endif
