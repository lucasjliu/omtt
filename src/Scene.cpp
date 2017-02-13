/*
* by Jiahuan.Liu
* 05/08/2016
* @SCUT
*/
#include "Scene.h"
#include "VideoBase.h"
#include "Target.h"
#include "Motion.h"

Scene::Scene(unsigned int width, unsigned int height)
	:_width(width), _height(height), _gridDim1(0), _gridDim2(0), 
	_en(width, height), _ex(width, height), _oc(width, height)
{
}

Scene::Scene(unsigned int width, unsigned int height, unsigned int gridDim1, unsigned int gridDim2)
	: _width(width), _height(height), _gridDim1(gridDim1), _gridDim2(gridDim2)
	, _en(width, height), _ex(width, height), _oc(width, height)
	, _priEnP(gridDim1, gridDim2), _priExP(gridDim1, gridDim2), _priOcP(gridDim1, gridDim2)
	, _priNeg(gridDim1, gridDim2), _sumEnP(0), _sumExP(0), _sumOcP(0), _sumNeg(0)
{
}

bool Scene::load(std::string enPicPath, std::string exPicPath, std::string ocPicPath)
{
	Frame *enFrame = cvLoadImage(enPicPath.c_str(), CV_LOAD_IMAGE_COLOR);
	Frame *exFrame = cvLoadImage(exPicPath.c_str(), CV_LOAD_IMAGE_COLOR);
	Frame *ocFrame = cvLoadImage(ocPicPath.c_str(), CV_LOAD_IMAGE_COLOR);
	assert(enFrame != NULL);
	assert(exFrame != NULL);
	assert(ocFrame != NULL);
	_height = enFrame->height;
	_width = enFrame->width;
	_en = Matrix<float>(_width, _height);
	_ex = Matrix<float>(_width, _height);
	_oc = Matrix<float>(_width, _height);
	if (!enFrame || !exFrame || !ocFrame)
		return false;
	for (int i = 0; i < enFrame->height; i++)
	{
		for (int j = 0; j < enFrame->width; j++)
		{
			CvScalar bgr = cvGet2D(enFrame, i, j);
			if (bgr.val[0] == 255 && bgr.val[1] == 0 && bgr.val[2] == 0)
			{
				_en.set(j, i, 1);
			}
		}
	}
	for (int i = 0; i < exFrame->height; i++)
	{
		for (int j = 0; j < exFrame->width; j++)
		{
			CvScalar bgr = cvGet2D(exFrame, i, j);
			if (bgr.val[0] == 0 && bgr.val[1] == 255 && bgr.val[2] == 0)
			{
				_ex.set(j, i, 1);
			}
		}
	}
	for (int i = 0; i < ocFrame->height; i++)
	{
		for (int j = 0; j < ocFrame->width; j++)
		{
			CvScalar bgr = cvGet2D(ocFrame, i, j);
			if (bgr.val[0] == 0 && bgr.val[1] == 0 && bgr.val[2] == 255)
			{
				_oc.set(j, i, 1);
			}
		}
	}
	return true;
}

float Scene::getEn(unsigned int x, unsigned int y)
{
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return 1;
	return _en.get(x, y);
}

float Scene::getEx(unsigned int x, unsigned int y)
{
	if (x < 0 || y < 0 || x >= _width || y >= _height)
		return 1;
	return _ex.get(x, y);
}

float Scene::getOc(unsigned int x, unsigned int y)
{
	if (x >= _width)
		x = _width - 1;
	if (x < 0)
		x = 0;
	if (y >= _height)
		y = _height - 1;
	if (y < 0)
		y = 0;
	return _oc.get(x, y);
}

Feedback::Feedback(FB_TYPE t, Target& target)
{
	type = t;
	pos = Motion::predict(target, 1); //should be outer
	pos.x = pos.x + target.getSize().width / 2;
	pos.y = pos.y + target.getSize().height;
	std::cout << "Event " << t << ": target " << target.getId() << " at "
		<< target.getPos().x << " " << target.getPos().y << std::endl;
}

Feedback::Feedback(FB_TYPE t, PotentialTarget& target)
{
	type = t;
	pos = target.getInitPos();
	pos.x = pos.x + target.getSize().width / 2;
	pos.y = pos.y + target.getSize().height;
	std::cout << "Event " << t << ": target " << target.getId() << " at "
		<< target.getPos().x << " " << target.getPos().y << std::endl;
}

void Scene::handleFeedback(Feedback& fb)
{
	fb.pos.x = (fb.pos.x >= _width ? _width - 1 : (fb.pos.x < 0 ? 0 : fb.pos.x));
	fb.pos.y = (fb.pos.y >= _height ? _height - 1 : (fb.pos.y < 0 ? 0 : fb.pos.y));
	Position pos = _pos2Grid(fb.pos);
	switch (fb.type)
	{
	case FB_TYPE::EN:
		_priEnP.set(pos, _priEnP.get(pos) + 1);
		_sumEnP++;
		_en.set(fb.pos, _priEnP.get(pos) / (_sumEnP + 0.01));
		break;
	case FB_TYPE::EX:
		_priExP.set(pos, _priExP.get(pos) + 1);
		_sumExP++;
		_ex.set(fb.pos, _priExP.get(pos) / (_sumExP + 0.01));
		break;
	case FB_TYPE::OC:
		_priOcP.set(pos, _priOcP.get(pos) + 1);
		_sumOcP++;
		_oc.set(fb.pos, _priOcP.get(pos) / (_sumOcP + 0.01));
		break;
	case FB_TYPE::PA:
		_priNeg.set(pos.x, pos.y, _priNeg.get(pos) + 1);
		_sumNeg++;
		_en.set(fb.pos, _priEnP.get(pos) / (_sumEnP + 0.01));
		_ex.set(fb.pos, _priExP.get(pos) / (_sumExP + 0.01));
		_oc.set(fb.pos, _priOcP.get(pos) / (_sumOcP + 0.01));
		break;
	default:
		break;
	}
}

void SceneTest::initialize()
{
	_ttargets.swap(std::vector<TrackedTarget>());
	_gts.swap(std::vector<Target>());
	_gtList.swap(std::vector<std::vector<Target>>());
	_scene = Scene(384, 288, 38, 28); //////////////////////////
	_frameNum = 0;
}

bool SceneTest::loadgt_MOT()
{//////////////////////////////////////////////// TODO: Loader
	std::string gtFilePath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\gt\\gt.txt";
	FILE* in;
	fopen_s(&in, gtFilePath.c_str(), "r");
	if (!in)
		return false;
	Target target;
	std::vector<Target> frame;
	unsigned int nk = 1, nk1 = 1;
	while (_loadgt_MOT(in, nk1, target) == 6)
	{
		while (nk1 > nk)
		{
			_gtList.push_back(frame);
			frame.clear();
			++nk;
		}
		frame.push_back(target);
	}
	_gtList.push_back(frame);
	fclose(in);
	return true;
}

int SceneTest::_loadgt_MOT(FILE*& file, unsigned int& frameNum, Target& target)
{
	Position tempPos;
	Size tempSize;
	int id, ret;
	ret = fscanf_s(file, "%d, %d, %f, %f, %f, %f, 1, -1, -1, -1\n",
		&frameNum, &id, &tempPos.x, &tempPos.y, &tempSize.width, &tempSize.height);
	target = Object(tempPos, tempSize);
	target.setId(id);
	return ret;
}

bool SceneTest::loadgt_CAVIAR()
{
	return true;
}

bool SceneTest::fetchNextFrame()
{
	if (_frameNum >= _gtList.size())
		return false;
	_gts.swap(std::vector<Target>());
	_gts = _gtList[_frameNum]; ////////////////////////////////?????
	_frameNum++;
	return true;
}

void SceneTest::processFrame()
{
	std::vector<TrackedTarget> targets;
	TrackedTarget target;
	bool gtIsFound;
	bool *tIsLost = new bool[_ttargets.size()];
	memset(tIsLost, 1, _ttargets.size());
	for (size_t i = 0; i < _gts.size(); ++i)
	{
		target = _gts[i];
		_scene.handleFeedback(Feedback(FB_TYPE::PA, _gts[i]));
		gtIsFound = false;
		for (size_t j = 0; j < _ttargets.size(); ++j)
		{
			if (target.getId() == _ttargets[j].getId())
			{
				target = _ttargets[j];
				target.updateSpeed(_gts[i]);
				gtIsFound = true;
				tIsLost[j] = false;
				break;
			}
		}
		if (!gtIsFound)
		{
			_scene.handleFeedback(Feedback(FB_TYPE::EN, target));
		}
		targets.push_back(target);
	}
	// check oc
	for (size_t i = 0; i < _ltargets.size(); ++i)
	{
		for (size_t j = 0; j < _ttargets.size(); ++j)
		{
			if (tIsLost[j] && _ltargets[i].getId() == _ttargets[j].getId())
			{
				targets.push_back(_ltargets[i]);
				_scene.handleFeedback(Feedback(FB_TYPE::OC, _ltargets[i]));
				_ltargets.erase(_ltargets.begin() + i);
				tIsLost[j] = false;
			}
		}
	}
	// add lost targets
	for (size_t i = 0; i < _ttargets.size(); ++i)
	{
		if (tIsLost[i])
		{
			_ltargets.push_back(_ttargets[i]);
			std::cout << "Lost" << ": target " << _ttargets[i].getId() << " at "
				<< _ttargets[i].getPos().x << " " << _ttargets[i].getPos().y << std::endl;
		}
	}
	_ttargets.clear();
	_ttargets.swap(targets);
}

void SceneTest::terminate()
{
	for (size_t i = 0; i < _ltargets.size(); ++i)
		_scene.handleFeedback(Feedback(FB_TYPE::EX, _ltargets[i]));
	_scene.testPrint("D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\img1\\000001.jpg",
		"D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\scene_test_en.bmp");
}

int  SceneTest::getFrameNum()
{
	return _frameNum;
}

void Scene::testPrint(std::string srcpath, std::string respath)
{
	/*std::cout << std::endl;
	_priEnP.print();
	std::cout << std::endl;
	_priExP.print();
	std::cout << std::endl;
	_priOcP.print();
	std::cout << std::endl;
	_priNeg.print();
	std::cout << std::endl;*/

	Frame* res;
	res = cvLoadImage(srcpath.c_str(), CV_LOAD_IMAGE_COLOR);
	
	for (int i = 0; i < res->width; i++)
	{
		for (int j = 0; j < res->height; j++)
		{
			if (_priEnP.get(_x2grid(i), _y2grid(j)) > 0)
			{
				res->imageData[j * res->widthStep + i * res->nChannels] = 255;
				res->imageData[j * res->widthStep + i * res->nChannels + 1] = 0;
				res->imageData[j * res->widthStep + i * res->nChannels + 2] = 0;
			}
		}
	}

	cvSaveImage(respath.c_str(), res);
	cvReleaseImage(&res);
}

bool Scene::save(std::string filepath)
{
	FILE* out = NULL;
	fopen_s(&out, filepath.c_str(), "wb");
	if (out == NULL)
		return false;
	return fwrite(this, sizeof(Scene), 1, out);
}

bool Scene::load_memory(std::string filepath)
{
	FILE* in = NULL;
	fopen_s(&in, filepath.c_str(), "rb");
	if (in == NULL)
		return false;
	return fread(this, sizeof(Scene), 1, in);
}
