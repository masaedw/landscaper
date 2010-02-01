#ifndef _COLLISION_H_
#define _COLLISION_H_

//山のような2次元当たり判定の面々
#include "common.h"
#include "matrixobj.h"

namespace space
{

//当たり判定用IDの型
typedef unsigned char IDCOL2D;
//IDの定義
const IDCOL2D COL2D_NULL = 0;
const IDCOL2D COL2D_DOT = 1;
const IDCOL2D COL2D_LINE = 2;
const IDCOL2D COL2D_TRIANGLE = 3;
const IDCOL2D COL2D_CIRCLE = 4;
//最大値
const IDCOL2D COL2D_MAX = 255;

//アブストラクト
//２次元当たり判定用のAdapter
class COL2DOBJ
{
protected:
	IDCOL2D id;
public:
	COL2DOBJ(IDCOL2D _id=COL2D_NULL):id(_id){}
	virtual ~COL2DOBJ(){}
	IDCOL2D get_id() const { return id; }
	//Adapter
	virtual bool getCollision(const COL2DOBJ* obj) const = 0;
};
//当たり判定用
//点
class Dot2_COL2D : public COL2DOBJ
{
public:
	const Matrix21<double> *dot;
public:
	Dot2_COL2D() : COL2DOBJ(COL2D_DOT),dot(){}
	Dot2_COL2D(const Matrix21<double> *_obj) : COL2DOBJ(COL2D_DOT),dot(_obj){}
	virtual ~Dot2_COL2D(){}
	
	virtual bool getCollision(const COL2DOBJ* obj) const
	{
		IDCOL2D temp = obj->get_id();
		if(temp>id) return obj->getCollision(this);
		switch(temp)
		{
		case COL2D_DOT:
			if( dot->Col_Dot(*const_pointer_cast<const Dot2_COL2D*>(obj)->dot) )return true; break;
		}
		return false;
	}

};
//線
class Line2_COL2D : public COL2DOBJ
{
public:
	const Line2<double> *line;
public:
	Line2_COL2D() : COL2DOBJ(COL2D_LINE),line(){}
	Line2_COL2D(const Line2<double> *_obj) : COL2DOBJ(COL2D_LINE),line(_obj){}
	virtual ~Line2_COL2D(){}
	
	virtual bool getCollision(const COL2DOBJ* obj) const
	{
		IDCOL2D temp = obj->get_id();
		if(temp>id) return obj->getCollision(this);
		switch(temp)
		{
		case COL2D_DOT:
			if( line->Col_Dot(*const_pointer_cast<const Dot2_COL2D*>(obj)->dot) )return true; break;
		case COL2D_LINE:
			if( line->Col_Line(*const_pointer_cast<const Line2_COL2D*>(obj)->line) )return true; break;
		}
		return false;
	}
};
//三角形
class Triangle2_COL2D : public COL2DOBJ
{
public:
	const Triangle2<double> *triangle;
public:
	Triangle2_COL2D() : COL2DOBJ(COL2D_TRIANGLE),triangle(){}
	Triangle2_COL2D(const Triangle2<double> *_obj) : COL2DOBJ(COL2D_TRIANGLE),triangle(_obj){}
	virtual ~Triangle2_COL2D(){}
	
	virtual bool getCollision(const COL2DOBJ* obj) const
	{
		IDCOL2D temp = obj->get_id();
		if(temp>id) return obj->getCollision(this);
		switch(temp)
		{
		case COL2D_DOT:
			if( triangle->Col_Dot(*const_pointer_cast<const Dot2_COL2D*>(obj)->dot) )return true; break;
		case COL2D_LINE:
			if( triangle->Col_Line(*const_pointer_cast<const Line2_COL2D*>(obj)->line) )return true; break;
		case COL2D_TRIANGLE:
			if( triangle->Col_Tri(*const_pointer_cast<const Triangle2_COL2D*>(obj)->triangle) )return true; break;
		}
		return false;
	}
};
//円
class Circle2_COL2D : public COL2DOBJ
{
public:
	const Circle2<double> *circle;
public:
	Circle2_COL2D() : COL2DOBJ(COL2D_CIRCLE),circle(){}
	Circle2_COL2D(const Circle2<double> *_obj) : COL2DOBJ(COL2D_CIRCLE),circle(_obj){}
	virtual ~Circle2_COL2D(){}
	
	virtual bool getCollision(const COL2DOBJ* obj) const
	{
		IDCOL2D temp = obj->get_id();
		if(temp>id) return obj->getCollision(this);
		switch(temp)
		{
		case COL2D_DOT:
			if( circle->Col_Dot(*const_pointer_cast<const Dot2_COL2D*>(obj)->dot) )return true; break;
		case COL2D_LINE:
			if( circle->Col_Line(*const_pointer_cast<const Line2_COL2D*>(obj)->line) )return true; break;
		case COL2D_TRIANGLE:
			if( circle->Col_Tri(*const_pointer_cast<const Triangle2_COL2D*>(obj)->triangle) )return true; break;
		case COL2D_CIRCLE:
			if( circle->Col_Circle(*const_pointer_cast<const Circle2_COL2D*>(obj)->circle) )return true; break;
		default:
			throw "(collision.h) Circle2 collision detection error!!";
		}
		return false;
	}
};

//二つ以上の複合判定
class CollisionDetective2D : public COL2DOBJ
{
public:
	std::list<const COL2DOBJ*> objs;
public:
	CollisionDetective2D() : COL2DOBJ(COL2D_MAX){ objs.clear(); }
	virtual ~CollisionDetective2D(){}

	virtual bool getCollision(const COL2DOBJ* obj) const
	{
		std::list<const COL2DOBJ*>::const_iterator it=objs.begin();
		for(;it!=objs.end();it++) if((*it)->getCollision(obj)) return true;
		return false;
	}

};

}

#endif
