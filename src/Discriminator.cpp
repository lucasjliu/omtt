/*
* by Jiahuan.Liu
* 04/17/2016
* @SCUT
*/
#include "Discriminator.h"
#include "Motion.h"

void Discriminator::extractColor(Object& t, Frame* frame)
{
	float* histVal = new float[Appearance::COLOR_HIST_SIZE];
	VideoBase::extractColorHist(frame, t.getPos(), t.getSize(), histVal,
		Appearance::COLOR_BIN_SIZE, 1);
	t.setRGB(histVal);
	VideoBase::extractColorHist(frame, t.getPos(), t.getSize(), histVal,
		Appearance::COLOR_BIN_SIZE, 0);
	t.setHSV(histVal);
	delete[] histVal;
}

float Discriminator::Bhattacharyya(float hist1[], float hist2[], int pNum1, int pNum2)
{
	float sum = 0;
	for (int i = 0; i < Appearance::COLOR_HIST_SIZE; i++)
	{
		sum += sqrt((hist1[i] / pNum1)*(hist2[i] / pNum2));
	}
	return sum;
}

float Discriminator::similarity(Object& t1, Object& t2)
{
	return Bhattacharyya(t1.getAppearance()._histRGB, t2.getAppearance()._histRGB,
		(int)t1.getSize().width * (int)t1.getSize().height,
		(int)t2.getSize().width * (int)t2.getSize().height);
}

float Discriminator::similarity(Target& t1, Object& t2, int interval)
{
	float bhat = similarity((Object)t1, t2);
	float overlap = Motion::calOverlapArea(t1, t2, Motion::predict(t1, interval));
	float overlapRate = overlap * 2 /
		(t1.getSize().height * t1.getSize().width + t2.getSize().height * t2.getSize().width);
	return 0.8 * bhat + 0.2 * overlapRate;
}