/*
* by Jiahuan.Liu
* 04/16/2016
* @SCUT
*/
#include "Detector.h"
#include "Target.h"

#include <string>
#include <iostream>
#include <fstream>

Detector::Detector()
{
	_detList = std::vector<std::vector<Detection>>();
}

bool Detector::load(std::string& filepath, float confTh)
{
	fopen_s(&_inputFile, filepath.c_str(), "r");
	if (!_inputFile)
		return false;
	std::vector<Detection> detFrame;
	Detection det;
	unsigned int nk = 1, nk1 = 1;
	while (_loadDet(_inputFile, nk1, det) == 6)
	{
		while (nk1 > nk)
		{
			_detList.push_back(detFrame);
			detFrame.clear();
			++nk;
		}
		if (det.getDetScore() >= confTh)
			detFrame.push_back(det);
	}
	_detList.push_back(detFrame);
	fclose(_inputFile);
	_isDirty = new bool[_detList.size()];
	memset(_isDirty, 0, _detList.size());
	return true;
}

int Detector::_loadDet(FILE*& file, unsigned int& frameNum, Detection& det)
{
	Position tempPos;
	Size tempSize;
	float tempScore;
	int ret = fscanf_s(file, "%d, -1, %f, %f, %f, %f, %f, -1, -1, -1\n",
		&frameNum, &tempPos.x, &tempPos.y, &tempSize.width, &tempSize.height,
		&tempScore);
	det = Detection(tempPos, tempSize, tempScore);
	return ret;
}

bool Detector::get(unsigned int& frameNum, std::vector<Detection>& detResponses)
{
	if (frameNum >= _detList.size())
		return false;
	detResponses.swap(_detList[frameNum]);
	return !_isDirty[frameNum];
}

void Detector::terminate()
{
	std::vector<std::vector<Detection>>::iterator itr = _detList.begin();
	for (; itr != _detList.end(); ++itr)
	{
		itr->clear();
	}
	_detList.clear();
	delete []_isDirty;
}