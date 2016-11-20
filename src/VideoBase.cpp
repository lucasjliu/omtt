/*
* by Jiahuan.Liu
* 04/17/2016
* @SCUT
*/
#include "VideoBase.h"

CvFont VideoBase::_labelFont = CvFont();

const int RECT_THICKNESS = 2;

void VideoBase::initialize()
{
	cvInitFont(&_labelFont, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 1, 2, 8);
}

void VideoBase::showFrame(const char* name, Frame* frame)
{
	cvShowImage(name, frame);
}

void VideoBase::rectangle(Frame* frame, Object& object,
	CvScalar rectColor)
{
	cvRectangle(frame, CvPoint(object.getPos().x, object.getPos().y),
		CvPoint(object.getPos().x + object.getSize().width,
		object.getPos().y + object.getSize().height), rectColor, RECT_THICKNESS);
}

void VideoBase::cutFrame(Frame* frame, Position& p, Size& s, Frame*& patch)
{
	cvSetImageROI(frame, cvRect(p.x, p.y, s.width, s.height));
	patch = cvCreateImage(CvSize(frame->roi->width, frame->roi->height), 8, 3);
	cvCopy(frame, patch, 0);
	cvResetImageROI(frame);
}

void VideoBase::extractColorHist(Frame* frame, Position& p, Size& s, 
	float*& histVal, const int binSize, bool isRGB)
{
	int histSize[] = { binSize, binSize, binSize };
	float range[] = { 0, 255 };
	float* ranges[] = { range, range, range };
	CvHistogram* hist = cvCreateHist(3, histSize, CV_HIST_ARRAY, ranges, 1);

	Frame* patch = NULL;
	cutFrame(frame, p, s, patch);

	IplImage* plane1 = cvCreateImage(cvGetSize(patch), 8, 1);
	IplImage* plane2 = cvCreateImage(cvGetSize(patch), 8, 1);
	IplImage* plane3 = cvCreateImage(cvGetSize(patch), 8, 1);
	IplImage* planes[] = { plane1, plane2, plane3 };
	if (!isRGB)
		cvCvtColor(patch, patch, CV_BGR2HSV);
	cvSplit(patch, plane1, plane2, plane3, 0);
	cvCalcHist(planes, hist, 0, 0);

	for (int dim1 = 0; dim1 < binSize; dim1++)
	{
		for (int dim2 = 0; dim2 < binSize; dim2++)
		{
			for (int dim3 = 0; dim3 < binSize; dim3++)
			{
				histVal[dim1*binSize*binSize + dim2*binSize + dim3] =
					cvGetReal3D(hist->bins, dim1, dim2, dim3);
			}
		}
	}

	cvReleaseImage(&patch);
}

ImageInput::ImageInput(std::string path)
	: _path(path) {}

bool ImageInput::get(unsigned int frameNum, Frame*& frame)
{
	char filename[10];
	sprintf_s(filename, "%06d", frameNum + 1);
	std::string filepath = _path + std::string(filename) + ".jpg";
	frame = cvLoadImage(filepath.c_str(), CV_LOAD_IMAGE_COLOR);
	return (bool)frame;
}

ImageOutput::ImageOutput(std::string path)
	: _path(path) {}

bool ImageOutput::put(unsigned int frameNum, Frame* frame)
{
	char filename[11];
	sprintf_s(filename, "%06d", frameNum + 1);
	std::string filepath = _path + std::string(filename) + ".jpg";
	return cvSaveImage(filepath.c_str(), frame);
}