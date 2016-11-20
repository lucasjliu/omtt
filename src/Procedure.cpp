/*
* by Jiahuan.Liu
* 08/04/2016
* @SCUT
*/
#include "Procedure.h"
#include "Discriminator.h"

#include <iostream>
#include <string>

void Procedure::run(Tracker* tracker, Scene* scene, Detector* detector)
{
	_tracker = tracker;
	_scene = scene;
	_detector = detector;
	_initialize();
	while (_fetchFrame())
	{
		_processFrame();
		_printLog();
	}
	_report();
	_terminate();
}

void Procedure::_printLog() const
{
	std::cout << _timeStamp()
		<< _frameNum << ": "
		<< _tracker->gettNum() << " "
		<< _tracker->getpNum() << " "
		<< _tracker->getlNum() << std::endl;
}

#include "Detection.h"

bool Tracking::_fetchFrame()
{
	return _imgInput.get(_frameNum, _currFrame); //frameNum begin with 1
												//warning: image format
}

void Tracking::_initialize()
{
	Procedure::_initialize();
	VideoBase::initialize();
	_recorder.initialize(_param.videoName);
	_imgInput = ImageInput(_param.imgInputPath);
	//_imgOutput = ImageOutput(_param.workPath + "result\\img\\");
	//_testScene = Scene(_scene->getSize());//TODO: getSize()
	//_video = cvCreateVideoWriter((_param.workPath + "result\\" + _param.videoName).c_str(),
		//CV_FOURCC('M', 'J', 'P', 'G'), 25, cvSize(384, 288), 1);
	//if (_tracker.fail() || _detector->fail() || _scene.isEmpty())
		//exit(1);//check input components
}

void Tracking::_terminate()
{
	Procedure::_terminate();
	//_testscene.testPrint();
	_recorder.terminate(_frameNum);
	_recorder.saveXml(_param.workPath + "result\\" + _param.videoName + ".tracking.xml");
	//cvReleaseVideoWriter(&_video);
}

void Tracking::_processFrame()
{
	std::vector<Detection> dets;
	_detector->get(_frameNum, dets); //get detection result
	for (DetItr detItr = dets.begin(); detItr != dets.end(); detItr++)
	{ //extract appearance features of detections in the current frame
		Discriminator::extractColor(*detItr, _currFrame);
	}
	TrackerRet ret(dets.size());
	std::vector<Detection> tempDets = dets;

	_tracker->processFrame(_frameNum, &dets, _scene, &_recorder, ret);

	//draw and output result
	for (size_t i = 0; i < tempDets.size(); i ++)
	{
		VideoBase::rectangle(_currFrame, tempDets[i]);
		if (ret.detAssign[i] >= 0)
			VideoBase::putText(_currFrame, tempDets[i].getPos(), ret.detAssign[i], CV_RGB(0, 255, 0));
	}
	VideoBase::putText(_currFrame, Position(10, 1000), _frameNum, CV_RGB(255, 255, 255));
	//VideoBase::showFrame("scene", _currFrame);
	cvWaitKey();
	_imgOutput.put(_frameNum, _currFrame);
	//cvWriteFrame(_video, _currFrame);

	cvReleaseImage(&_currFrame);
	_frameNum++;
}