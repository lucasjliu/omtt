/*
* by Jiahuan.Liu
* 04/13/2016
* @SCUT
*/
#ifndef DETECTOR_H_
#define DETECTOR_H_

#include "Detection.h"

#include <vector>
#include <stdio.h>

typedef std::vector<Detection> Dets;
typedef std::vector<Detection>::iterator DetItr;

class Detector
{
public:
	Detector();
	bool load(std::string& filepath, float confTh);
	bool get(unsigned int& frameNum, std::vector<Detection>& detResponses);
	void terminate();
private:
	int _loadDet(FILE*& file, unsigned int& frameNum, Detection& det);
	std::vector<std::vector<Detection>> _detList;
	FILE*								_inputFile;
	bool*								_isDirty;
};

#endif