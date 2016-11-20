/*
* by Jiahuan.Liu
* 04/13/2016
* @SCUT
*/
#ifndef VIDEOBASE_H_
#define VIDEOBASE_H_

#include "Object.h"

#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <vector>
#include <fstream>

typedef IplImage Frame;

class VideoBase
{
public:
	static void initialize();
	static void showFrame(const char* name, Frame* frame);
	static void rectangle(Frame* frame, Object& object, CvScalar rectColor = CV_RGB(0, 0, 255));
	static void cutFrame(Frame* frame, Position& p, Size& s, Frame*& dest);
	static void extractColorHist(Frame* frame, Position& p, Size& s, 
		float*& histVal, const int binSize, bool isRGB = 1);
private:
	static CvFont _labelFont;
public:
	template <typename T>
	static void rectangle(Frame* frame, Object& object, T label,
		CvScalar rectColor = CV_RGB(0, 0, 255),
		CvScalar labelColor = CV_RGB(255, 0, 0))
	{
		rectangle(frame, object, rectColor);
		putText(frame, object.getPos(), label, labelColor);
	}
	template <typename T>
	static void putText(Frame*& frame, Position& p, T text,
		CvScalar textColor = CV_RGB(0, 0, 255))
	{
		std::ostringstream ss;
		ss << text;
		cvPutText(frame, ss.str().c_str(), CvPoint(p.x, p.y),
			&_labelFont, textColor);
	}
};

class ImageInput
{
public:
	ImageInput(std::string path = "");
	bool get(unsigned int frameNum, Frame*& frame);
private:
	std::string	_path;
};

class ImageOutput
{
public:
	ImageOutput(std::string path = "");
	bool put(unsigned int FrameNum, Frame* frame);
private:
	std::string	_path;
};

struct VideoParam
{
	std::string _inputPath;
	std::string _outputPath;
};

#endif