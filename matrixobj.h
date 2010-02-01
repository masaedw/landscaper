#ifndef _MATRIXOBJ_H_
#define _MATRIXOBJ_H_

#include <math.h>
#include <float.h>

namespace space
{

//‚«‚í‚ß‚Ä•’Ê‚È3sˆê—ñ‚Ìs—ñ‚Æ‚à‚¢‚¦‚È‚­‚Í‚È‚¢‰½‚©
//“_‚Ì\‘¢‘Ì‚Æ‚¢‚¦‚È‚­‚à‚È‚¢
template <class T=double>
struct Matrix31
{
	T x;
	T y;
	T z;

	Matrix31();
	Matrix31(const Matrix31<T> &_t);
	Matrix31(T _x,T _y,T _z);

	//‘«‚µZ
	inline Matrix31<T> operator+(const Matrix31<T> &_t);
	//ˆø‚«Z
	inline Matrix31<T> operator-(const Matrix31<T> &_t);
	//‘ã“ü
	inline Matrix31<T>& operator=(const Matrix31<T> &_t);
	//“™‚µ‚¢
	inline bool operator==(const Matrix31<T> &_t) const;

	//’è”‚Æ‚Ì‘«‚µZ
	inline Matrix31<T> operator+(T _c);
	//’è”‚Æ‚Ìˆø‚«Z
	inline Matrix31<T> operator-(T _c);
	//’è”‚Æ‚ÌŠ|‚¯Z
	inline Matrix31<T> operator*(T _c);
	//’è”‚Æ‚ÌŠ„‚èZ
	inline Matrix31<T> operator/(T _c);

	//â‘Î’l
	inline T Abs2();
	inline T Abs();

	//“àÏ
	inline T DotMul(const Matrix31<T> &_t);
	//ŠOÏ
	inline Matrix31<T> CrsMul(const Matrix31<T> &_t);

};


//‘O•ûéŒ¾
template <class T> struct Matrix21;
template <class T> struct Line2;
template <class T> struct Triangle2;
template <class T> struct Circle2;

//‚«‚í‚ß‚Ä•’Ê‚È2sˆê—ñ‚Ìs—ñ‚Æ‚à‚¢‚¦‚È‚­‚Í‚È‚¢‰½‚©
//“_‚Ì\‘¢‘Ì‚Æ‚¢‚¦‚È‚­‚à‚È‚¢
template <class T=double>
struct Matrix21
{
	T x;
	T y;

	Matrix21();
	Matrix21(const Matrix21<T> &_t);
	Matrix21(T _x,T _y);

	//‘«‚µZ
	inline Matrix21<T> operator+(const Matrix21<T> &_t) const;
	//ˆø‚«Z
	inline Matrix21<T> operator-(const Matrix21<T> &_t) const;

	//‘ã“ü
	inline Matrix21<T>& operator=(const Matrix21<T> &_t);
	//‘«‚µ‚Ä‘ã“ü
	inline Matrix21<T>& operator+=(const Matrix21<T> &_t);
	//ˆø‚¢‚Ä‘ã“ü
	inline Matrix21<T>& operator-=(const Matrix21<T> &_t);
	
	//“™‚µ‚¢
	inline bool operator==(const Matrix21<T> &_t) const;
	//“™‚µ‚­‚È‚¢
	inline bool operator!=(const Matrix21<T> &_t) const;

	//’è”‚Æ‚Ì‘«‚µZ
	inline Matrix21<T> operator+(T _c) const;
	//’è”‚Æ‚Ìˆø‚«Z
	inline Matrix21<T> operator-(T _c) const;
	//’è”‚Æ‚ÌŠ|‚¯Z
	inline Matrix21<T> operator*(T _c) const;
	//’è”‚Æ‚ÌŠ„‚èZ
	inline Matrix21<T> operator/(T _c) const;
	//‘«‚µ‚Ä‘ã“ü
	inline Matrix21<T>& operator+=(T _c) ;
	//ˆø‚¢‚Ä‘ã“ü
	inline Matrix21<T>& operator-=(T _c) ;
	//Š|‚¯‚Ä‘ã“ü
	inline Matrix21<T>& operator*=(T _c) ;
	//Š„‚Á‚Ä‘ã“ü
	inline Matrix21<T>& operator/=(T _c) ;

	//â‘Î’l
	inline T Abs2() const;
	inline T Abs() const;

	//2ŸŒ³‚Ì“àÏ
	inline T DotMul(const Matrix21<T> &_t) const;
	//2ŸŒ³‚ÌŠOÏ
	inline T CrsMul(const Matrix21<T> &_t) const;

	//ƒLƒƒƒXƒg
	template <class S> inline operator Matrix21<S>() const{ return Matrix21<S>((S)x,(S)y); }

	//‚¨‚È‚¶‚¾‚Á‚½‚çB
	inline bool Col_Dot (const Matrix21 &_mt) const;
	//‚»‚Ì‘¼
	inline bool Col_Line (const Line2<T> &_mt) const;
	inline bool Col_Tri (const Triangle2<T> &_mt) const;
	inline bool Col_Circle (const Circle2<T> &_mt) const;
};


//==============
//“ñŸŒ³‚ÈlX
//==============
//’¼ü
template <class T=double>
struct Line2
{
	//“_2ŒÂ
	Matrix21<T> p1,p2;

	Line2();
	Line2(const Line2 &_l);
	Line2(const Matrix21<T> &_p1, const Matrix21<T> &_p2);
	Line2(T _x1,T _y1,T _x2,T _y2);

	//‘ã“ü
	inline Line2<T>& operator=(const Line2<T> &_l);
	//“™‚µ‚¢
	inline bool operator==(const Line2<T> &_l) const;

	//’·‚³‚Ì‚Qæ
	inline T length2();
	inline T length();

	//2’¼ü‚Ì“–‚½‚è”»’è
	//{p1(x,y)¨p2(x,y)}‚Æ{_l.p1(x,y)¨_l.p2(x,y)}
	//“–‚½‚ê‚Îtrue
	inline bool Col_Line (Line2 _l) const;
	//double‚Å‚Í–â‘è‚Ì‚ ‚é’è‹`B
	inline bool Col_Dot(const Matrix21<T> &_mt) const;
	//‚»‚Ì‘¼
	inline bool Col_Tri (const Triangle2<T> &_mt) const;
	inline bool Col_Circle (const Circle2<T> &_mt) const;
	
	
	// “ñ‚Â‚Ìü•ª‚ÌŒğ“_‚ğ‹‚ß‚éB–ß‚è’ltrue‚ÅŒğ‚í‚Á‚Ä‚¢‚é.
	inline bool CrossPoint(const Line2<T> &_l, Matrix21<T> &_p) const;
	//‚ ‚é“_‚Ì‚ü‚Æ’¼Œğ‚·‚é’n“_‚ğ‹‚ß‚éB’¼s’n“_‚ªüã‚È‚çtrue.
	inline bool OrthoPoint(const Matrix21<T> &_p, Matrix21<T> &_h) const;
	//‚ ‚é“_‚ÌAü‘ÎÌ‚ÈˆÊ’u‚ğ‹‚ß‚éD‚»‚ÌˆÊ’u‚©‚ç‚ü‚ğˆø‚¢‚Ä‘ÎÛüã‚È‚ç‚Îtrue.
	inline bool LineSymmetryPoint(const Matrix21<T> &_p, Matrix21<T> &_q) const;

};
	
//OŠpŒ`
template <class T=double>
struct Triangle2
{
	//“_‚R‚±
	Matrix21<T> p1,p2,p3;

	Triangle2();
	Triangle2(const Triangle2<T> &_t);
	Triangle2(const Line2<T> &_l1, const Line2<T> &_l2, const Line2<T> &_l3);
	Triangle2(const Matrix21<T> &_p1, const Matrix21<T> &_p2, const Matrix21<T> &_p3);
	
	Triangle2(T _x1,T _y1,T _x2,T _y2,T _x3,T _y3);
	
	//“™‚µ‚¢
	inline bool operator==(const Triangle2<T> &_t) const;
	//‘ã“ü
	inline Triangle2<T>& operator=(const Triangle2<T> &_t);

	//OŠpŒ`‚Æ“_‚Ì”»’è
	//{0(x,y)¨1(x,y)¨2(x,y)},‚¨‚æ‚Ñp(x,y)
	//“–‚½‚ê‚Îtrue
	inline bool Col_Dot(const Matrix21<T> &_p) const;
	//OŠpŒ`‚Æü‚Ì”»’è
	inline bool Col_Line(const Line2<T> &_l) const ;
	//OŠpŒ`‚ÆOŠpŒ`‚Ì”»’è
	inline bool Col_Tri(const Triangle2<T> &_t) const ;
	//‚»‚Ì‘¼
	inline bool Col_Circle (const Circle2<T> &_mt) const;

};


//‰~
template <class T=double>
struct Circle2
{
	Matrix21<T> p;	//’†S“_
	T d;	//’¼Œa

	Circle2();
	Circle2(const Circle2<T> &_c);
	Circle2(const Matrix21<T> &_p,T &_d);
	Circle2(T _x, T _y, T _d);
	//‘ã“ü
	inline Circle2<T>& operator=(const Circle2<T> &_c);

	//‰~‚Æ“_‚Ì“–‚½‚è”»’è
	//d‚Í‰~‚Ì’¼Œa
	//“–‚½‚Á‚½‚çtrue
	inline bool Col_Dot(const Matrix21<T> &_p) const;
	//‰~‚Æ‰~
	//“–‚½‚Á‚½‚çtrue
	inline bool Col_Circle (const Circle2<T> &_c) const;
	//ü‚Æ‰~‚Ì“–‚½‚è”»’è
	//‚±‚ê‚Í‚Â‚Ü‚è,“_‚Æ’¼ü‚Ì‹——£‚ğ‹‚ß‚Ä‚»‚ê‚ğ”»’è‚·‚é‚±‚Æ‚É“™‚µ‚¢
	//’¼ü‚ÌÀ•W‚ğ2‚ÂA‰~‚Ì’†SÀ•W‚ğˆê‚ÂAÅŒã‚É‚Í‰~‚Ì’¼Œa‚ğ“ü‚ê‚æ‚¤
	//“–‚½‚Á‚½‚çtrue
	inline bool Col_Line (const Line2<T> &_l) const;
	//OŠpŒ`‚Æ‰~‚Ì“–‚½‚è”»’è
	//‚ ‚½‚ê‚Îtrue
	inline bool Col_Tri(const Triangle2<T> &_t) const;

};



//===========================
//‚«‚í‚ß‚Ä•’Ê‚È3sˆê—ñ‚Ìs—ñ
//===========================
template <class T> Matrix31<T>::Matrix31(){}
template <class T> Matrix31<T>::Matrix31(const Matrix31<T> &_t){ x=_t.x; y=_t.y; z=_t.z;}
template <class T> Matrix31<T>::Matrix31(T _x,T _y,T _z){ x=_x;y=_y; z=_z;}

//‘«‚µZ
template <class T> inline Matrix31<T> Matrix31<T>::operator+(const Matrix31<T> &_t)
{ return Matrix31<T>(x+_t.x,y+_t.y,z+_t.z); }
//ˆø‚«Z
template <class T> inline Matrix31<T> Matrix31<T>::operator-(const Matrix31<T> &_t)
{ return Matrix31<T>(x-_t.x,y-_t.y,z-_t.z); }
//‘ã“ü
template <class T> inline Matrix31<T>& Matrix31<T>::operator=(const Matrix31<T> &_t)
{
	x=_t.x; y=_t.y; z=_t.z;
	return *this;
}
//“™‚µ‚¢
template <class T> inline bool Matrix31<T>::operator==(const Matrix31<T> &_t) const
{ return (x==_t.x&&y==_t.y&&z==_t.z);}

//’è”‚Æ‚Ì‘«‚µZ
template <class T> inline Matrix31<T> Matrix31<T>::operator+(T _c)
{ return Matrix31(x+_c,y+_c,z+_c); }
//’è”‚Æ‚Ìˆø‚«Z
template <class T> inline Matrix31<T> Matrix31<T>::operator-(T _c)
{ return Matrix31(x-_c,y-_c,z-_c); }
//’è”‚Æ‚ÌŠ|‚¯Z
template <class T> inline Matrix31<T> Matrix31<T>::operator*(T _c)
{ return Matrix31(x*_c,y*_c,z*_c); }
//’è”‚Æ‚ÌŠ„‚èZ
template <class T> inline Matrix31<T> Matrix31<T>::operator/(T _c)
{ return Matrix31(x/_c,y/_c,z/_c); }

//â‘Î’l
template <class T> inline T Matrix31<T>::Abs2() {return x*x+y*y+z*z;}
template <class T> inline T Matrix31<T>::Abs() {return sqrt(Abs2());}

//“àÏ
template <class T> inline T Matrix31<T>::DotMul(const Matrix31<T> &_t)
{ return (_t.x*x + _t.y*y + _t.z*z); }
//ŠOÏ
template <class T> inline Matrix31<T> Matrix31<T>::CrsMul(const Matrix31<T> &_t)
{ return Matrix31(y*_t.z-z*_t.y ,z*_t.x-x*_t.z, x*_t.y-y*_t.x); }




//===========================
//‚«‚í‚ß‚Ä•’Ê‚È2sˆê—ñ‚Ìs—ñ
//===========================
template <class T> Matrix21<T>::Matrix21(){}
template <class T> Matrix21<T>::Matrix21(const Matrix21<T> &_t){ x=_t.x; y=_t.y; }
template <class T> Matrix21<T>::Matrix21(T _x,T _y){ x=_x;y=_y; }

//‘«‚µZ
template <class T> inline Matrix21<T> Matrix21<T>::operator+(const Matrix21<T> &_t) const
{ return Matrix21<T>(x+_t.x,y+_t.y); }
//ˆø‚«Z
template <class T> inline Matrix21<T> Matrix21<T>::operator-(const Matrix21<T> &_t) const 
{ return Matrix21<T>(x-_t.x,y-_t.y); }

//‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator=(const Matrix21<T> &_t)
{ x=_t.x; y=_t.y; return *this; }
//‘«‚µ‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator+=(const Matrix21<T> &_t)
{ x+=_t.x; y+=_t.y; return *this; }
//ˆø‚¢‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator-=(const Matrix21<T> &_t)
{ x-=_t.x; y-=_t.y; return *this; }

//“™‚µ‚¢
template <class T> inline bool Matrix21<T>::operator==(const Matrix21<T> &_t) const { return (x==_t.x&&y==_t.y); }
//“™‚µ‚­‚È‚¢
template <class T> inline bool Matrix21<T>::operator!=(const Matrix21<T> &_t) const { return (x!=_t.x||y!=_t.y); }

//’è”‚Æ‚Ì‘«‚µZ
template <class T> inline Matrix21<T> Matrix21<T>::operator+(T _c) const { return Matrix21(x+_c,y+_c); }
//’è”‚Æ‚Ìˆø‚«Z
template <class T> inline Matrix21<T> Matrix21<T>::operator-(T _c) const { return Matrix21(x-_c,y-_c); }
//’è”‚Æ‚ÌŠ|‚¯Z
template <class T> inline Matrix21<T> Matrix21<T>::operator*(T _c) const { return Matrix21(x*_c,y*_c); }
//’è”‚Æ‚ÌŠ„‚èZ
template <class T> inline Matrix21<T> Matrix21<T>::operator/(T _c) const { return Matrix21(x/_c,y/_c); }
//‘«‚µ‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator+=(T _c) { x+=_c; y+=_c; return *this; }
//ˆø‚¢‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator-=(T _c) { x-=_c; y-=_c; return *this; }
//Š|‚¯‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator*=(T _c) { x*=_c; y*=_c; return *this; }
//Š„‚Á‚Ä‘ã“ü
template <class T> inline Matrix21<T>& Matrix21<T>::operator/=(T _c) { x/=_c; y/=_c; return *this; }

//â‘Î’l
template <class T> inline T Matrix21<T>::Abs2() const {return x*x+y*y;}
template <class T> inline T Matrix21<T>::Abs() const {return sqrt(Abs2());}

//2ŸŒ³‚Ì“àÏ
template <class T> inline T Matrix21<T>::DotMul(const Matrix21<T> &_t) const { return (x*_t.x + y*_t.y); }
//2ŸŒ³‚ÌŠOÏ
template <class T> inline T Matrix21<T>::CrsMul(const Matrix21<T> &_t) const { return (x*_t.y - y*_t.x); }

//‚¨‚È‚¶‚¾‚Á‚½‚çB
template <class T> inline bool Matrix21<T>::Col_Dot (const Matrix21<T> &_mt) const { return _mt==*this; }
//‚»‚Ì‘¼
template <class T> inline bool Matrix21<T>::Col_Line (const Line2<T> &_mt) const { return _mt.Col_Dot(*this); }
template <class T> inline bool Matrix21<T>::Col_Tri (const Triangle2<T> &_mt) const { return _mt.Col_Dot(*this); }
template <class T> inline bool Matrix21<T>::Col_Circle (const Circle2<T> &_mt) const { return _mt.Col_Dot(*this); }



//==============
//’¼ü
//=============
template <class T> inline Line2<T>::Line2(){}
template <class T> inline Line2<T>::Line2(const Line2 &_l) {p1=_l.p1; p2=_l.p2;}
template <class T> inline Line2<T>::Line2(const Matrix21<T> &_p1, const Matrix21<T> &_p2){ p1=_p1; p2=_p2; }
template <class T> inline Line2<T>::Line2(T _x1,T _y1,T _x2,T _y2)
{
	p1=Matrix21<T>(_x1,_y1);
	p2=Matrix21<T>(_x2,_y2);
}

//‘ã“ü
template <class T> inline Line2<T>& Line2<T>::operator=(const Line2<T> &_l)
{
	p1=_l.p1; p2=_l.p2;
	return *this;
}
//“™‚µ‚¢
template <class T> inline bool Line2<T>::operator==(const Line2<T> &_l) const { return (p1==_l.p1&&p2==_l.p2);}

//’·‚³‚Ì‚Qæ
template <class T> inline T Line2<T>::length2(){ return (p1-p2).Abs2(); }
template <class T> inline T Line2<T>::length() { return sqrt( length2() ); }

//2’¼ü‚Ì“–‚½‚è”»’è
//{p1(x,y)¨p2(x,y)}‚Æ{_l.p1(x,y)¨_l.p2(x,y)}
//“–‚½‚ê‚Îtrue
template <class T> inline bool Line2<T>::Col_Line (Line2<T> _l) const {
  double mulres[2];
  mulres[0] = (p1-p2).CrsMul(p1-_l.p1) * (p1-p2).CrsMul(p1-_l.p2);
  mulres[1] = (_l.p1-_l.p2).CrsMul(_l.p1-p1) * (_l.p1-_l.p2).CrsMul(_l.p1-p2);

  if ( mulres[0] < 0.0 && mulres[1] < 0.0 ) return true;
  return false;
}

//double‚Å‚Í–â‘è‚Ì‚ ‚é’è‹`B
template <class T> inline bool Line2<T>::Col_Dot(const Matrix21<T> &_mt) const
{
	return (_mt.x-p1.x/p2.x-p1.x==_mt.y-p1.y/p2.y-p1.y);
}

//‚»‚Ì‘¼
template <class T> inline bool Line2<T>::Col_Tri (const Triangle2<T> &_mt) const { return _mt.Col_Line(*this); }
template <class T> inline bool Line2<T>::Col_Circle (const Circle2<T> &_mt) const { return _mt.Col_Line(*this); }

// “ñ‚Â‚Ìü•ª‚ÌŒğ“_‚ğ‹‚ß‚éB–ß‚è’ltrue‚ÅŒğ‚í‚Á‚Ä‚¢‚é.
template <class T> inline bool Line2<T>::CrossPoint(const Line2<T> &_l, Matrix21<T> &_p) const
{
  double A,B,C,D,E,F,det;

  A = p2.x - p1.x;
  B = p2.y - p1.y;

  C = _l.p2.x - _l.p1.x;
  D = _l.p2.y - _l.p1.y;

  det = D*A-B*C;

  if( det*det < DBL_EPSILON){
	_p = Matrix21<T>(0.0,0.0);
    return false;
  }

  E = B*   p1.x - A*   p1.y;
  F = D*_l.p1.x - C*_l.p1.y;

  _p.x = (A*F-E*C)/det;
  _p.y = (B*F-D*E)/det;
  return Col_Line(_l);
}

//‚ ‚é“_‚Ì‚ü‚Æ’¼Œğ‚·‚é’n“_‚ğ‹‚ß‚éB’¼s’n“_‚ªüã‚È‚çtrue.
template <class T> inline bool Line2<T>::OrthoPoint(const Matrix21<T> &_p, Matrix21<T> &_h) const
{
	T k;
	Matrix21<T> s = p2-p1;
	k = s.DotMul(_p)/s.Abs2();
	_h = s*k + p1;
	return (0.0<=k&&k<=1.0);
}

//‚ ‚é“_‚ÌAü‘ÎÌ‚ÈˆÊ’u‚ğ‹‚ß‚éD‚»‚ÌˆÊ’u‚©‚ç‚ü‚ğˆø‚¢‚Ä‘ÎÛüã‚È‚ç‚Îtrue.
template <class T> inline bool Line2<T>::LineSymmetryPoint(const Matrix21<T> &_p, Matrix21<T> &_q) const
{
	Matrix21<T> _h;
	bool ret = OrthoPoint(_p,_h);
	_q = _h*2 - _p;
	return ret;
}



//==========
//OŠpŒ`
//============
template<class T> Triangle2<T>::Triangle2(){}
template<class T> Triangle2<T>::Triangle2(const Triangle2<T> &_t){ p1 = _t.p1; p2 = _t.p2; p3 = _t.p3; }
template<class T> Triangle2<T>::Triangle2(const Line2<T> &_l1, const Line2<T> &_l2, const Line2<T> &_l3)
{
	p1=_l1.p1; p2=_l2.p1; p3=_l3.p1;
}
template<class T> Triangle2<T>::Triangle2(const Matrix21<T> &_p1, const Matrix21<T> &_p2, const Matrix21<T> &_p3)
{
	p1=_p1; p2=_p2; p3=_p3;
}

template<class T> Triangle2<T>::Triangle2(T _x1,T _y1,T _x2,T _y2,T _x3,T _y3)
{
	p1 = Matrix21<T>(_x1,_y1);
	p2 = Matrix21<T>(_x2,_y2);
	p3 = Matrix21<T>(_x3,_y3);
}

//“™‚µ‚¢
template<class T> inline bool Triangle2<T>::operator==(const Triangle2<T> &_t) const { return (p1==_t.p1&&p2==_t.p2&&p3==_t.p3);}
//‘ã“ü
template<class T> inline Triangle2<T>& Triangle2<T>::operator=(const Triangle2<T> &_t)
{
	p1=_t.p1; p2=_t.p2; p3=_t.p3;
	return *this;
}

//OŠpŒ`‚Æ“_‚Ì”»’è
//{0(x,y)¨1(x,y)¨2(x,y)},‚¨‚æ‚Ñp(x,y)
//“–‚½‚ê‚Îtrue
template<class T> inline bool Triangle2<T>::Col_Dot(const Matrix21<T> &_p) const {
  double mulres[3];
  mulres[0] = (p1-p2).CrsMul(Matrix21<T>(p1-_p));
  mulres[1] = (p2-p3).CrsMul(Matrix21<T>(p2-_p));
  mulres[2] = (p3-p1).CrsMul(Matrix21<T>(p3-_p));

  if(mulres[0]>0.0 && mulres[1]>0.0 && mulres[2]>0.0) return true;
  if(mulres[0]<0.0 && mulres[1]<0.0 && mulres[2]<0.0) return true;
  return false;
}

//OŠpŒ`‚Æü‚Ì”»’è
template<class T> inline bool Triangle2<T>::Col_Line(const Line2<T> &_l) const 
{
	if( (Line2<T>(p1,p2)).Col_Line(_l) ) return true;
	if( (Line2<T>(p2,p3)).Col_Line(_l) ) return true;
	if( (Line2<T>(p3,p1)).Col_Line(_l) ) return true;
	//ÅŒã‚É“à•””»’è
	return Col_Dot(_l.p1);
}

//OŠpŒ`‚ÆOŠpŒ`‚Ì”»’è
template<class T> inline bool Triangle2<T>::Col_Tri(const Triangle2<T> &_t) const 
{
	if( Col_Line(Line2<T>(_t.p1,_t.p2)) ) return true;
	if( Col_Line(Line2<T>(_t.p2,_t.p3)) ) return true;
	if( Col_Line(Line2<T>(_t.p3,_t.p1)) ) return true;
	//ÅŒã‚É“à•””»’è
	//OŠpŒ`‚ÍŒİ‚¢‚É“à‘¤‚É‚¢‚é‰Â”\«‚ğl‚¦‚é
	if( Col_Dot(_t.p1) ) return true;
	return _t.Col_Dot(p1);
}

//‚»‚Ì‘¼
template <class T> inline bool Triangle2<T>::Col_Circle (const Circle2<T> &_mt) const { return _mt.Col_Tri(*this); }

//====
//‰~
//====
template<class T> Circle2<T>::Circle2(){}
template<class T> Circle2<T>::Circle2(const Circle2<T> &_c){ p=_c.p; d=_c.d;}
template<class T> Circle2<T>::Circle2(const Matrix21<T> &_p,T &_d){ p=_p; d=_d; }
template<class T> Circle2<T>::Circle2(T _x, T _y, T _d){ p=Matrix21<T>(_x,_y); d=_d; }
//‘ã“ü
template<class T> Circle2<T>& Circle2<T>::operator=(const Circle2<T> &_c)
{
	p=_c.p; d=_c.d;
	return *this;
}

//‰~‚Æ“_‚Ì“–‚½‚è”»’è
//d‚Í‰~‚Ì’¼Œa
//“–‚½‚Á‚½‚çtrue
template<class T> inline bool Circle2<T>::Col_Dot(const Matrix21<T> &_p) const
{
	if( d*d/4.0 > (Line2<T>(p,_p)).length2() ) return true;
  return false;
}

//‰~‚Æ‰~
//“–‚½‚Á‚½‚çtrue
template<class T> inline bool Circle2<T>::Col_Circle (const Circle2<T> &_c) const
{
	if( (d+_c.d)*(d+_c.d)/4.0 > (Line2<T>(p,_c.p)).length2() ) return true;
	return false;
}

//ü‚Æ‰~‚Ì“–‚½‚è”»’è
//‚±‚ê‚Í‚Â‚Ü‚è,“_‚Æ’¼ü‚Ì‹——£‚ğ‹‚ß‚Ä‚»‚ê‚ğ”»’è‚·‚é‚±‚Æ‚É“™‚µ‚¢
//’¼ü‚ÌÀ•W‚ğ2‚ÂA‰~‚Ì’†SÀ•W‚ğˆê‚ÂAÅŒã‚É‚Í‰~‚Ì’¼Œa‚ğ“ü‚ê‚æ‚¤
//“–‚½‚Á‚½‚çtrue
template<class T> inline bool Circle2<T>::Col_Line (const Line2<T> &_l) const
{

  double A,B,C,D,E,F,G,dist;
  A = _l.p2.x - _l.p1.x;
  B = _l.p2.y - _l.p1.y;
  C = B*_l.p1.x - A*_l.p1.y;
  F = A*A+B*B;
  dist = fabs(B*p.x-A*p.y-C)/sqrt(F);

  D = (Line2<T>(p,_l.p1)).length2() ;
  E = (Line2<T>(p,_l.p2)).length2() ;
  G = (D>E) ? D : E;

  if(dist <= d/2 && G*G-dist*dist <= F*F)return true;

  //’[‚Ì“_‚Æ‰~‚Ì“–‚½‚è”»’è‚ğs‚¤
  //‚±‚ê‚ª‚È‚¢‚Æ³Šm‚¶‚á‚È‚¢
  if(Col_Dot(_l.p1))return true;
  if(Col_Dot(_l.p2))return true;

  return false;
}

//OŠpŒ`‚Æ‰~‚Ì“–‚½‚è”»’è
//‚ ‚½‚ê‚Îtrue
template<class T> inline bool Circle2<T>::Col_Tri(const Triangle2<T> &_t) const
{
	// ‰~‚Æ3‚Â‚Ì’¼ü‚Æ‚Ì“–‚½‚è”»’è
	if( Col_Line(Line2<T>(_t.p1,_t.p2)) ) return true;
	if( Col_Line(Line2<T>(_t.p2,_t.p3)) ) return true;
	if( Col_Line(Line2<T>(_t.p3,_t.p1)) ) return true;

	//’¼ü‚ÆŒğ‚í‚ç‚È‚­‚Ä‚àAOŠpŒ`‚Ì“à•”‚É‰~‚ª‚ ‚éê‡‚Í‚ ‚½‚Á‚½‚±‚Æ‚É‚È‚é
	return _t.Col_Dot(p);
}



}

#endif
