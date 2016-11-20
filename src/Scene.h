/*
* by Jiahuan.Liu
* 05/08/2016
* @SCUT
*/
#ifndef SCENE_H_
#define SCENE_H_

#include "util.h"
#include "Object.h"

#include <iostream>

struct Feedback;
class SceneTest;
class Scene
{
public:
	friend class SceneTest;
	Scene()
		:_width(0), _height(0) {}
	Scene(unsigned int width, unsigned int height);
	Scene(unsigned int width, unsigned int height,
		unsigned int gridDim1, unsigned int gridDim2);
	bool load(std::string enPicPath, std::string exPicPath, std::string ocPicPath);
	float getEn(unsigned int x, unsigned int y);
	float getEx(unsigned int x, unsigned int y);
	float getOc(unsigned int x, unsigned int y);
	void handleFeedback(Feedback& fb);
	//void update();
	void testPrint(std::string srcpath, std::string respath);
	bool save(std::string filepath);
	bool load_memory(std::string filepath);
private:
	unsigned int _x2grid(float x) { return x / _width * _gridDim1; } //////???
	unsigned int _y2grid(float y) { return y / _height * _gridDim2; }
	Position _pos2Grid(Position pos) 
	{ return Position(_x2grid(pos.x), _y2grid(pos.y)); }
private:
	unsigned int _width;
	unsigned int _height;
	unsigned int _gridDim1;
	unsigned int _gridDim2;
	Matrix<float> _en;
	Matrix<float> _ex;
	Matrix<float> _oc;
	Matrix<int> _priEnP;
	Matrix<int> _priExP;
	Matrix<int> _priOcP;
	Matrix<int> _priNeg;
	size_t _sumEnP;
	size_t _sumExP;
	size_t _sumOcP;
	size_t _sumNeg;
};

class Target;
class PotentialTarget;
class Motion;
enum FB_TYPE {EN, EX, OC, PA};
struct Feedback
{
	Feedback() {}
	Feedback(FB_TYPE t, Target& target);
	Feedback(FB_TYPE t, PotentialTarget& target);
	FB_TYPE type;
	Position pos;
	//int targetid;
};

// SceneTest.h
class Target;                    //////////////////////////head file problem
class TrackedTarget;
class SceneTest
{
public:
	SceneTest() {}
	void initialize();
	bool loadgt_MOT();
	bool loadgt_CAVIAR();
	bool fetchNextFrame();
	void processFrame();
	void terminate();
	int  getFrameNum();
private:
	void _updateTargets();
	void _updateScene();
	int _loadgt_MOT(FILE*& file, unsigned int& frameNum, Target& target);
private:
	int _frameNum;
	Scene _scene;
	std::vector<TrackedTarget>		 _ttargets;
	std::vector<TrackedTarget>		 _ltargets;
	std::vector<Target>				 _gts;
	std::vector<std::vector<Target>> _gtList;
};

#endif