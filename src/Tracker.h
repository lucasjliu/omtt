/*
* by Jiahuan.Liu
* 04/14/2016
* @SCUT
*/
#ifndef TRACKER_H_
#define TRACKER_H_

#include "VideoBase.h"
#include "Discriminator.h"
#include "Detector.h"
#include "Target.h"
#include "util.h"
#include "Recorder.h"
#include "Scene.h"

struct TrackerParam
{
	float ttSimTh;
	float tpSimTh;
	float tlSimTh;

	float newWeightRatio;
	float lostWeightRatio;

	int initFrameTh1;//frame num
	int termFrameTh1;
	int initFrameTh2;
	int termFrameTh2;
};

struct TrackerRet
{
	TrackerRet() :detAssign(NULL){}
	TrackerRet(int detNum)
	{
		detAssign = new int[detNum];
		for (size_t i = 0; i < detNum; i++)
			detAssign[i] = -1;
	}
	~TrackerRet()
	{
		if (detAssign != NULL)
			delete[]detAssign;
	}

	int* detAssign;
};

class Tracker
{
public:
	Tracker();
	Tracker(TrackerParam& param) :_param(param){}
	//~Tracker();
	void processFrame(unsigned int& _frameNum, Dets* dets,
		Scene* scene, Recorder* _recorder, TrackerRet& ret);
	int gettNum() { return _tracked.size(); }
	int getpNum() { return _potential.size(); }
	int getlNum() { return _lost.size(); }
private:
	//data association. 1: traditional. 2: with source and dummy nodes
	void _associate1();
	void _associate2();

	//perform assignment with the association result
	void _handleAssign(); //weight update

	//remove detections that are assigned. the rest are potential
	void _updateDets();

	//update targets state after assignment
	void _updateState();	//init&term, exclusion

	//call data association algorithm
	void _hungarian(float* distMat, int rowSize, int columnSize, int*& result);

	//call recorder to add a tracked record, as output result
	bool _addRecord(Target& target);

	float _newWeight(Detection& detection);
	float _lostWeight(Target& target);
	float _simWithScene(float sim, float newWeight, float lostWeight);

	//transition function between distance and similarity in data association
	float _sim2dist(float sim) const { return 1 - sim; }
	float _dist2sim(float dist) const { return 1 - dist; }
	//void _buildNonExcluTable(Matrix<float>& table);
private:
	Dets*						 _dets;
	Scene*						 _scene;
	Recorder*					 _recorder;
	unsigned int				 _frameNum;

	//containers for the three types of targets
	TrackerParam				 _param;
	std::vector<TrackedTarget>	 _tracked;
	std::vector<PotentialTarget> _potential;
	std::vector<LostTarget>		 _lost;

	//data association result, intermediate variable
	int*						 _assign;
	float*						 _score;
	//float*					 _sim;
};

#endif