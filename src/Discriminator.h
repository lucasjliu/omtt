/*
* by Jiahuan.Liu
* 04/15/2016
* @SCUT
*/
#ifndef DISCRIMINATOR_H_
#define DISCRIMINATOR_H_

#include "object.h"
#include "Target.h"
#include "VideoBase.h"

#include <math.h>

class Discriminator
{
public:
	static void extractColor(Object& t, Frame* frame);
	static float similarity(Object& t1, Object& t2);
	static float similarity(Target& t1, Object& t2, int interval);
private:
	static float Bhattacharyya(float hist1[], float hist2[], int pNum1, int pNum2);
};

#endif