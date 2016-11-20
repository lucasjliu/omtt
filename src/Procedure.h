/*
* by Jiahuan.Liu
* 08/04/2016
* @SCUT
*/
#ifndef PROCEDURE_H_
#define PROCEDURE_H_

#include "Tracker.h"
#include "Detector.h"
#include "Scene.h"

#include <ctime>

class Procedure
{//basic procedure for a general tracking algorithm
public:
	Procedure() :_begin(0), _frameNum(1){}
	void		 run(Tracker* tracker, Scene* scene, Detector* detector);
protected:
	virtual void _initialize() { _startClock(); _frameNum = 0; }
	virtual void _terminate() { _stopClock(); }
	virtual void _printLog() const;
	virtual void _report() const {}
	virtual bool _fetchFrame() = 0;
	virtual void _processFrame() = 0;
private:
	//TODO:
	void		 _startClock() {}
	int			 _stopClock() { return 0; }
	std::string	 _timeStamp() const { return ""; }
	//end TODO
protected:
	Tracker*	 _tracker;
	Scene*		 _scene;
	Detector*	 _detector;
	unsigned int _frameNum;
private:
	time_t       _begin;
};

#include "VideoBase.h"
#include "Recorder.h"

struct TrackingParam
{
	std::string workPath;
	std::string dataPath;
	std::string videoName;
	std::string imgInputPath;
	//std::string videoInputPath;
};

class Tracking : public Procedure
{
public:
	Tracking() :_currFrame(NULL){}
	Tracking(TrackingParam param) :_param(param){}
	~Tracking() { cvReleaseImage(&_currFrame); }
	void _initialize();
	void _terminate();
	bool _fetchFrame();
	void _processFrame();
private:
	TrackingParam	_param;
	Frame*			_currFrame;
	Recorder		_recorder;
	//Scene			_testScene;
	ImageInput		_imgInput;//assert !fail
	ImageOutput		_imgOutput;//optional
	//CvVideoWriter*	_video;//optional, to be replaced
};

#endif