/*
* by Jiahuan.Liu
* 04/14/2016
* @SCUT
*/
#include "Tracker.h"

#include <iostream>

void Tracker::processFrame(unsigned int& frameNum, Dets* dets,
	Scene* scene, Recorder* recorder, TrackerRet& ret)
{
	_dets = dets;
	_scene = scene;
	_recorder = recorder;
	_frameNum = frameNum;

	size_t targetNum = _tracked.size() + _potential.size() + _lost.size();
	_assign = new int[targetNum];
	_score = new float[targetNum];
	//_sim = new float[targetNum];
	memset(_assign, 0, sizeof(int)*targetNum);
	memset(_score, 0, sizeof(float)*targetNum);
	//memset(_sim, 0, sizeof(float)*targetNum);

	_associate1();
	_handleAssign();
	_updateState();

	//write ret and draw tracking result in outer logic
	for (size_t i = 0; i < targetNum; i++)
	{
		if (_assign[i] >= 0)
			ret.detAssign[_assign[i]] = i;
	}

	delete[]_score;
	delete[]_assign;
}

void Tracker::_associate1()
{
	float sim;
	size_t targetNum = _tracked.size() + _potential.size() + _lost.size();
	size_t detNum = _dets->size();
	Matrix<float> distMat = Matrix<float>(targetNum, detNum);

	for (std::size_t j = 0; j < detNum; j++)
	{
		for (std::size_t i = 0; i < _tracked.size(); i++)
		{
			sim = Discriminator::similarity(_tracked[i], (*_dets)[j], 1);
			sim = _simWithScene(sim, _newWeight((*_dets)[j]), _lostWeight(_tracked[i]));
			distMat.set(i, j, (sim < _param.ttSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
		for (std::size_t i = 0; i < _potential.size(); i++)
		{
			sim = Discriminator::similarity(_potential[i], (*_dets)[j], _potential[i].getConsecMissedCount());
			sim = _simWithScene(sim, _newWeight((*_dets)[j]), _lostWeight(_potential[i]));
			distMat.set(i + _tracked.size(), j,
				(sim < _param.tpSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
		for (std::size_t i = 0; i < _lost.size(); i++)
		{
			sim = Discriminator::similarity(_lost[i], (*_dets)[j], _lost[i].getConsecMissedCount());
			sim = _simWithScene(sim, _newWeight((*_dets)[j]), _lostWeight(_lost[i]));
			distMat.set(i + _tracked.size() + _potential.size(), j,
				(sim < _param.tlSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
	}
	this->_hungarian(distMat.getDataPtr(), distMat.getSize(1), distMat.getSize(2), _assign);
}

void Tracker::_associate2()
{
	float sim;
	size_t targetNum = _tracked.size() + _potential.size() + _lost.size();
	size_t detNum = (*_dets).size();
	Matrix<float> distMat = Matrix<float>(targetNum + detNum, detNum);

	for (std::size_t j = 0; j < detNum; j++)
	{
		for (std::size_t i = 0; i < _tracked.size(); i++)
		{
			sim = Discriminator::similarity(_tracked[i], (*_dets)[j], 1);
			distMat.set(i, j, (sim < _param.ttSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
		for (std::size_t i = 0; i < _potential.size(); i++)
		{
			sim = Discriminator::similarity(_potential[i], (*_dets)[j], _potential[i].getConsecMissedCount());
			distMat.set(i + _tracked.size(), j, (sim < _param.tpSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
		for (std::size_t i = 0; i < _lost.size(); i++)
		{
			sim = Discriminator::similarity(_lost[i], (*_dets)[j], _lost[i].getConsecMissedCount());
			distMat.set(i + _tracked.size() + _potential.size(), j, (sim < _param.tlSimTh ? PLUS_INFINITY : _sim2dist(sim)));
		}
	}

	float tempMinDist;
	for (std::size_t j = 0; j < detNum; j++)
	{
		tempMinDist = distMat.min(j, 2, 0, targetNum);
		for (std::size_t i = targetNum; i < targetNum + detNum; i++)
		{
			distMat.set(i, j, tempMinDist);
		}
	}
	//distMat.print();

	delete[]_assign;
	_assign = new int[targetNum + detNum];
	this->_hungarian(distMat.getDataPtr(), distMat.getSize(1), distMat.getSize(2), _assign);
}

void Tracker::_handleAssign()
{
	int* ttAssign = _assign;
	int* tpAssign = &_assign[_tracked.size()];
	int* tlAssign = &_assign[_tracked.size() + _potential.size()];
	std::vector<TrackedTarget> tTargets;
	TrackedTarget tTarget;
	PotentialTarget pTarget;
	LostTarget lTarget;

	for (size_t i = 0; i < _tracked.size(); ++i)
	{
		if (ttAssign[i] == -1 || ttAssign[i] >= (*_dets).size())
		{
			//lTarget = LostTarget();
			lTarget = _tracked[i];
			lTarget.updateCount(false);
			lTarget.updateLostWeight(*_scene);
			lTarget.setLostPos(_tracked[i].getPos()); //////////////////
			_lost.push_back(lTarget); ///////////////////////////////
		}
		else
		{
			_tracked[i].copy((*_dets)[ttAssign[i]]);
			_tracked[i].updateSpeed((*_dets)[ttAssign[i]]);
			_tracked[i].updateCount(true);
			tTargets.push_back(_tracked[i]);
			this->_addRecord(_tracked[i]);
		}
	}
	_tracked.clear();
	_tracked.swap(tTargets);
	for (size_t i = 0; i < _potential.size(); ++i)
	{
		if (tpAssign[i] != -1 && tpAssign[i] < (*_dets).size())
		{
			_potential[i].copy((*_dets)[tpAssign[i]]);
			_potential[i].updateSpeed((*_dets)[tpAssign[i]]);
			_potential[i].updateCount(true);
			this->_addRecord(_potential[i]);
		}
		else
		{
			_potential[i].updateCount(false);
		}
	}
	for (size_t i = 0; i < _lost.size(); ++i)
	{
		if (tlAssign[i] != -1 && tlAssign[i] < (*_dets).size())
		{
			_lost[i].copy((*_dets)[tlAssign[i]]);
			_lost[i].updateSpeed((*_dets)[tlAssign[i]]);
			_lost[i].updateCount(true);
			
		}
		else
		{
			_lost[i].updateCount(false);
		}
	}
	_updateDets();
	for (DetItr detItr = (*_dets).begin(); detItr != (*_dets).end(); ++detItr)
	{
		pTarget = PotentialTarget();
		pTarget.copy(*detItr);
		pTarget.setId(_recorder->addTrack(_frameNum));
		pTarget.updateCount(true);
		pTarget.updateInitWeight(*_scene);
		pTarget.setInitPos(detItr->getPos()); ////////////////////////
		this->_addRecord(pTarget);
		_potential.push_back(pTarget); ///////////////////////////////
	}
}

void Tracker::_updateState()
{
	std::vector<PotentialTarget> pTargets;
	std::vector<LostTarget>	lTargets;
	TrackedTarget tTarget;
	for (size_t i = 0; i < _potential.size(); ++i)
	{
		if (_potential[i].getConsecAssignedCount() >=
			(_potential[i].calInitConf() > 0.9 ? 
			_param.initFrameTh2 : _param.initFrameTh1))
		{//initialization
			tTarget = _potential[i];
			_tracked.push_back(tTarget);
			//_testscene.handleFeedback(Feedback(FB_TYPE::EN, _potential[i]));
			//_scene.handleFeedback(Feedback(FB_TYPE::EN, _potential[i]));
		}
		else if (_potential[i].getStateCount() <= 5)
		{//stay potential state
			pTargets.push_back(_potential[i]);
		}
		else
		{//false detection
			_recorder->rmTrack(_potential[i].getId());
			int alert = 1;
		}
	}
	_potential.clear();
	_potential.swap(pTargets);
	for (size_t i = 0; i < _lost.size(); ++i)
	{
		if (_lost[i].getConsecAssignedCount() >= 2)
		{//re-identification. lost by occlusion
			tTarget = _lost[i];
			this->_addRecord(tTarget);
			_tracked.push_back(tTarget);
		}
		else if (_lost[i].getStateCount() >
			(_lost[i].calTermConf() > 0.9 ? 
			_param.termFrameTh2 : _param.termFrameTh1))
		{//termination
			_recorder->endTrack(_lost[i].getId(),
				_frameNum - _lost[i].getConsecMissedCount());
		}
		else
		{//stay lost state
			lTargets.push_back(_lost[i]);
		}
	}
	_lost.clear();
	_lost.swap(lTargets);
	for (size_t i = 0; i < _tracked.size(); ++i)
	{
		_tracked[i].updateSceneConf(*_scene);
	}
	for (size_t i = 0; i < _potential.size(); ++i)
	{
		_potential[i].updateSceneConf(*_scene);
	}
	for (size_t i = 0; i < _lost.size(); ++i)
	{
		_lost[i].updateSceneConf(*_scene);
	}
}

void Tracker::_hungarian(float* distMat, int rowNum, int columnNum, int*& result)
{
	float* _assign = new float[rowNum];
	float cost = 0;
	//solve assignment problem
	assignmentoptimal(_assign, &cost, distMat, rowNum, columnNum, PLUS_INFINITY);
	for (size_t i = 0; i < rowNum; ++i)
	{
		result[i] = round(_assign[i]);
	}
	delete[]_assign;
}

bool Tracker::_addRecord(Target& target)
{
	return _recorder->addRecord(target.getId(), target.getPos().x, target.getPos().y,
		target.getSize().width, target.getSize().height, _frameNum);
}


void Tracker::_updateDets()
{
	unsigned int targetNum = _tracked.size() + _potential.size() + _lost.size();
	bool *isAssigned = new bool[(*_dets).size()];
	memset(isAssigned, 0, (*_dets).size());
	for (size_t i = 0; i < targetNum; ++i)
	{
		if (_assign[i] >= 0 && _assign[i] < (*_dets).size())
		{
			isAssigned[_assign[i]] = true;
		}
	}
	std::vector<Detection> dets;
	for (size_t i = 0; i < (*_dets).size(); ++i)
	{
		if (!isAssigned[i])
		{
			dets.push_back((*_dets)[i]);
		}
	}
	(*_dets).clear();
	(*_dets).swap(dets);
	delete[]isAssigned;
}

float Tracker::_newWeight(Detection& detection)
{
	return _scene->getEn(detection.getStandPos().x,
		detection.getStandPos().y);
}


float Tracker::_lostWeight(Target& target)
{
	return target.getTermWeight();
}

float Tracker::_simWithScene(float sim, float newWeight, float lostWeight)
{
	return sim * (1 - newWeight * _param.newWeightRatio - lostWeight * _param.lostWeightRatio);
}