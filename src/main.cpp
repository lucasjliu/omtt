#include "Tracker.h"
#include "Detector.h"
#include "Scene.h"
#include "Procedure.h"

#include <iostream>
#include <string>

void test1()
{
	//define tracker
	TrackerParam tParam;
	tParam.ttSimTh = 0.6;
	tParam.tpSimTh = 0.6;
	tParam.tlSimTh = 0.6;
	tParam.newWeightRatio = 0;
	tParam.lostWeightRatio = 0;
	tParam.initFrameTh1 = 3;
	tParam.termFrameTh1 = 20;
	tParam.initFrameTh2 = 2;
	tParam.termFrameTh2 = 20;
	Tracker tracker(tParam);
	Scene scene;
	Detector publicDetector;
	
	//define an experiment case
	TrackingParam testParam;
	testParam.workPath = ".\\";
	testParam.dataPath = "";
	testParam.videoName = "test1";
	testParam.imgInputPath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\img1\\";
	Tracking testcase(testParam);

	std::string enPicFPath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\ADL-en1.bmp";
	std::string exPicFPath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\ADL-ex.bmp";
	std::string ocPicFPath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\ADL-oc.bmp";
	scene.load(enPicFPath, exPicFPath, ocPicFPath);

	std::string detFPath = "D:\\Document\\Study\\CV\\OMTT\\Test\\ADL-Rundle-6\\det\\det.txt";
	float detConfTh = 40; 
	publicDetector.load(detFPath, detConfTh);

	//main logic
	testcase.run(&tracker, &scene, &publicDetector);

	//clean up or terminate components
	publicDetector.terminate();
}

int main(int argc, char** argv)
{

	/*for (int i = 3; (i < argc) && ((argv[i])[0] == '-'); i++)
	{
		switch ((argv[i])[1])
		{
		case 'e':
			i++;
			tracker.set_std_noise(atof(argv[i]));
			break;
		default:
			printf("\nUnrecognized option %s!\n\n", argv[i]);
			exit(0);
		}
	}*/
	test1();
	return 0;
}