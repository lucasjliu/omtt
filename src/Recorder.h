/*
* by Jiahuan.Liu
* 05/21/2016
* @SCUT
*/
#ifndef RECORDER_H_
#define RECORDER_H_

#include "tinyxml.h"
#include "tinystr.h"
#include "util.h"

#include <map>
#include <vector>

const int MAX_TARGET_NUM = 10000;

struct Record
{
	Record(unsigned int frameNum, float px, float py, float width, float height)
		:frameNum(frameNum), px(px), py(py), width(width), height(height){}
	unsigned int frameNum;
	float px;
	float py;
	float width;
	float height;
};

struct Track
{
	Track()
		:startFrame(0), endFrame(0), records() {}
	Track(unsigned int start)
		:startFrame(start), endFrame(start), records() {}
	unsigned int startFrame;
	unsigned int endFrame;
	std::vector<Record> records;
};

class XmlHandler
{
public:
	XmlHandler();
	~XmlHandler() {}
	void initialize(const char* fname);
	void terminate(int endFrame);
	void saveTrack(unsigned int id, Track track);
	void saveFile(const char* filename);
private:
	void _saveRecord(Record record);
	typedef std::vector<Record>::iterator RecordItr;
private:
	TiXmlDocument _doc;
	TiXmlElement *_video;
	TiXmlElement *_track;
	TiXmlElement *_frame;
};

class Recorder
{
public:
	Recorder();
	~Recorder();
	void	initialize(std::string videoName);
	void	terminate(unsigned int endFrame);
	void	saveXml(std::string filepath);
	int		addTrack(unsigned int frameNum);
	bool	endTrack(unsigned int id, unsigned int frameNum);
	bool	rmTrack(unsigned int id);
	bool	addRecord(unsigned int id, float px, float py, 
				float width, float height, unsigned int frameNum);
	bool	mergeTrack(unsigned int id1, unsigned int id2);
	int		exclusionCount(unsigned int id1, unsigned int id2);
	int		getStartFrame(unsigned int id);
	int		getEndFrame(unsigned int id);
private:
	void	_sortRecords(unsigned int id);
private:
	std::map<unsigned int, Track>	_tracks;
	bool							_isUsed[MAX_TARGET_NUM];
	XmlHandler						_xml;
};

#endif