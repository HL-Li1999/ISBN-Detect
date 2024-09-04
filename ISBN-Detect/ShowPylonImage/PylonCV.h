

#ifndef __PYLONCV_H
#define __PYLONCV_H

#include <vector>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <pylon/PylonGUI.h>
#include <pylon/PylonImage.h>

using namespace std;
using namespace cv;
using namespace Pylon;

extern int deviceNum;
struct PylonCameraInfo
{
public:
	string ID;
	string modelName;
	int width;
	int height;
};

extern vector<PylonCameraInfo> camInfos;

void SearchPylonCam();

void CameraStart(int index);
bool GetPicFlag(int index);
void ShowImage(int* index_ptr, int flag);
Mat GetImgNow();
void SaveImage();

//void GETPIC(int index);

#endif