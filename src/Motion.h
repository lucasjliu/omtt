/*
* by Jiahuan.Liu
* 04/18/2016
* @SCUT
*/
#ifndef MOTION_H_
#define MOTION_H_

#include "Target.h"
#include "Detection.h"
#include "util.h"

class Motion
{
public:
	static Position predict(Target& t, unsigned int interval)
	{
		return Position(t.getPos().x + t.getSpeed().x,
			t.getPos().y + t.getSpeed().y);
	}
	static Speed calSpeed(Target& t, Object& d, unsigned int interval)
	{
		return Speed((t.getPos().x - d.getPos().x) / interval,
			(t.getPos().y - d.getPos().y) / interval);
	}
	static float calOverlapArea(Target& t, Object& d, Position predict)
	{
		int left = Max(t.getPos().x, d.getPos().x);
		int right = Min(t.getPos().x + t.getSize().width, d.getPos().x + d.getSize().width);
		int top = Max(t.getPos().y, d.getPos().y);
		int bottom = Min(t.getPos().y + t.getSize().height, d.getPos().y + d.getSize().height);
		if (left >= right || top >= bottom)
			return 0;
		return (right - left) * (bottom - top);
	}
};

#endif
