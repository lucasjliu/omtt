/*
* by Jiahuan.Liu
* 04/13/2016
* @SCUT
*/
#ifndef DETECTION_H_
#define DETECTION_H_

#include "Object.h"

class Detection : public Object
{
public:
	Detection()
		:_detScore(0){}
	Detection(Position p, Size s, float score)
		:Object(p, s), _detScore(score) {}
	float getDetScore() const { return _detScore; }
protected:
	float _detScore;//detection confidence score
};

#endif