/*
* by Jiahuan.Liu
* 05/01/2016
* @SCUT
*/
#include "Recorder.h"

XmlHandler::XmlHandler()
	:_doc()
{
}

void XmlHandler::initialize(const char* fname)
{
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	_doc.LinkEndChild(decl);
	_video = new TiXmlElement("Video");
	_doc.LinkEndChild(_video);
	_video->SetAttribute("fname", fname);
	_video->SetAttribute("start_frame", 0);
}

void XmlHandler::terminate(int endFrame)
{
	_video->SetAttribute("end_frame", endFrame);
}

void XmlHandler::saveTrack(unsigned int id, Track track)
{
	_track = new TiXmlElement("Trajectory");
	_video->LinkEndChild(_track);
	_track->SetAttribute("obj_id", id);
	_track->SetAttribute("start_frame", track.startFrame);
	_track->SetAttribute("end_frame", track.endFrame);
	RecordItr recordItr = track.records.begin();
	for (; recordItr != track.records.end(); ++recordItr)
	{
		this->_saveRecord(*recordItr);
	}
}

void XmlHandler::_saveRecord(Record record)
{
	_frame = new TiXmlElement("Frame");
	_frame->LinkEndChild(new TiXmlText(""));
	_track->LinkEndChild(_frame);
	_frame->SetAttribute("frame_no", record.frameNum);
	_frame->SetAttribute("x", record.px);
	_frame->SetAttribute("y", record.py);
	_frame->SetAttribute("width", record.width);
	_frame->SetAttribute("height", record.height);
	_frame->SetAttribute("observation", "1");
}

void XmlHandler::saveFile(const char* filename)
{
	_doc.SaveFile(filename);
}

Recorder::Recorder() 
{
	_tracks = std::map<unsigned int, Track>();
	memset(_isUsed, 0, MAX_TARGET_NUM);
	_xml = XmlHandler();
}

Recorder::~Recorder()
{
	std::map<unsigned int, Track>::iterator tracksItr = _tracks.begin();
	for (; tracksItr != _tracks.end(); ++tracksItr)
	{
		tracksItr->second.records.clear();
	}
	_tracks.clear();
}

void Recorder::initialize(std::string videoName)
{
	_xml.initialize(videoName.c_str());
}

void Recorder::terminate(unsigned int endFrame)
{
	_xml.terminate(endFrame);
}

void Recorder::saveXml(std::string filepath)
{
	for (size_t i = 0; i < MAX_TARGET_NUM; ++i)
	{
		if (_isUsed[i])
		{
			_xml.saveTrack(i, _tracks[i]);
		}
	}
	_xml.saveFile(filepath.c_str());
}

int Recorder::addTrack(unsigned int frameNum)
{
	for (size_t i = 0; i < MAX_TARGET_NUM; ++i)
	{
		if (!_isUsed[i])
		{
			_isUsed[i] = true;
			_tracks[i] = Track(frameNum);
			return i;
		}
	}
	return -1;
}

bool Recorder::endTrack(unsigned int id, unsigned int frameNum)
{
	if (!_isUsed[id])
		return false;
	_tracks[id].endFrame = frameNum;
	return true;
}

bool Recorder::rmTrack(unsigned int id)
{
	if (!_isUsed[id])
		return false;
	_isUsed[id] = false;
	_tracks[id].records.clear();
	_tracks.erase(id);
	return true;
}

bool Recorder::addRecord(unsigned int id, float px, float py,
	float width, float height, unsigned int frameNum)
{
	if (!_isUsed[id])
		return false;
	_tracks[id].records.push_back(Record(frameNum, px, py, width, height));
	return true;
}

bool Recorder::mergeTrack(unsigned int id1, unsigned int id2)
{
	if (!_isUsed[id1] || !_isUsed[id2])
		return false;

	return true;
}

int Recorder::exclusionCount(unsigned int id1, unsigned int id2)
{
	if (!_isUsed[id1] || !_isUsed[id2])
		return -1;
	unsigned int start = Min(_tracks[id1].startFrame, _tracks[id2].startFrame);
	unsigned int end = Max(_tracks[id1].endFrame, _tracks[id2].endFrame);
	bool *flag = new bool[start - end + 1];
	memset(flag, 0, start - end + 1);
	std::vector<Record>::iterator record = _tracks[id1].records.begin();
	for (; record != _tracks[id1].records.end(); ++record)
	{
		flag[record->frameNum] = true;
	}
	int count = 0;
	for (; record != _tracks[id2].records.end(); ++record)
	{
		if (flag[record->frameNum])
			++count;
	}
	delete []flag;
	return count;
}

int Recorder::getStartFrame(unsigned int id)
{
	if (!_isUsed[id])
		return -1;
	return _tracks[id].startFrame;
}

int Recorder::getEndFrame(unsigned int id)
{
	if (!_isUsed[id])
		return -1;
	return _tracks[id].endFrame;
}
