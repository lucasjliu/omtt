/*
* by Jiahuan.Liu
* 04/17/2016
* @SCUT
*/
#ifndef OBJECT_H_
#define OBJECT_H_

#include <string>

struct Position
{
	Position()
		:x(0), y(0){}
	Position(float x, float y)
		:x(x), y(y){}
	float x;
	float y;
};

struct Speed
{
	Speed()
		:x(0), y(0){}
	Speed(float x, float y)
		:x(x), y(y){}
	float x;
	float y;
};

struct Size
{
	Size()
		:width(0), height(0){}
	Size(float w, float h)
		:width(w), height(h){}
	float width;
	float height;
};

struct Appearance
{
	static const int COLOR_BIN_SIZE = 16;
	static const int COLOR_HIST_SIZE = 
		COLOR_BIN_SIZE*COLOR_BIN_SIZE*COLOR_BIN_SIZE;
	float _histRGB[COLOR_HIST_SIZE];
	float _histHSV[COLOR_HIST_SIZE];
};

class Object
{
public:
	Object()
		:_position(), _size(), _appearance() {}
	Object(Position& p, Size& s)
		:_position(p), _size(s), _appearance() {}
	Object(Position& p, Size& s, Appearance& a)
		:_position(p), _size(s), _appearance(a) {}
	Position getPos() const { return _position; }
	Size getSize() const { return _size; }
	Appearance getAppearance() const { return _appearance; }
	void setRGB(float* hist) { memcpy(_appearance._histRGB, hist, Appearance::COLOR_HIST_SIZE*sizeof(float)); }
	void setHSV(float* hist) { memcpy(_appearance._histHSV, hist, Appearance::COLOR_HIST_SIZE*sizeof(float)); }
	Position getStandPos()
	{
		return Position(_position.x + _size.width / 2,
			_position.y + _size.height);
	}
protected:
	Position	_position;
	Size		_size;
	Appearance	_appearance;
};

#endif