#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include "PylonCV.h"
#include <pylon/PylonImage.h>
#include <windows.h>

#include <iostream>
#include <string>

enum ErrorMsg {
	Default,		  //默认
	GrabFailed,   //未采集到图像

};

using namespace std;
using namespace Pylon;
using namespace cv;

int deviceNum;
vector<PylonCameraInfo> camInfos;
CInstantCameraArray camera;
CImageFormatConverter formatConverter;
CPylonImageWindow imageWindow;
HANDLE Thread_ShowImg;
CGrabResultPtr grabResult;
CPylonImage pylonImage;
ErrorMsg error_msg;

void IntialPylonCam()
{

}

void SearchPylonCam()
{
	PylonInitialize();

	CTlFactory& instance = CTlFactory::GetInstance();

	DeviceInfoList_t deviceList;
	deviceNum = instance.EnumerateDevices(deviceList);

	for (int i = 0; i < deviceNum; i++)
	{
		string ID = deviceList[i].GetDeviceID();
		string modelName = deviceList[i].GetModelName();
		PylonCameraInfo camInfo = { ID, modelName, 0, 0 };
		camInfos.push_back(camInfo);
	}
	
	camera.Initialize(deviceNum);

	for (int i = 0; i < deviceNum; i++)
	{
		IPylonDevice* pylonDevice = instance.CreateDevice(deviceList[i]);
		camera[i].Attach(pylonDevice);
	}
}

void CameraStart(int index)
{
	GenApi::INodeMap& nodemap = camera[index].GetNodeMap();
	camera[index].Open();
	GenApi::CIntegerPtr width = nodemap.GetNode("Width");
	GenApi::CIntegerPtr height = nodemap.GetNode("Height");
	camInfos[index].width = (int)width->GetValue();
	camInfos[index].height = (int)height->GetValue();
	
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;
	camera[index].StartGrabbing();

	imageWindow.Create(1, 0, 0, 1920, 1200);
}

bool GetPicFlag(int index)
{
	return camera[index].IsGrabbing();
}


DWORD WINAPI ThreadFun(LPVOID);

void ShowImage(int* index_ptr, int flag)
{
	if (flag == 0)
		Thread_ShowImg = CreateThread(NULL, 0, ThreadFun, (LPVOID)index_ptr, 0, NULL);
	else if (flag == 1)
		SuspendThread(Thread_ShowImg);
	else if (flag == 2)
		ResumeThread(Thread_ShowImg);
}

DWORD WINAPI ThreadFun(LPVOID index_ptr)
{
	int index = *((int*)index_ptr);
	while (camera[index].IsGrabbing())
	{
		camera[index].RetrieveResult(5000, grabResult);
		formatConverter.Convert(pylonImage, grabResult);
		imageWindow.SetImage(pylonImage);
		imageWindow.Show();
	}
	return 0;
}

Mat GetImgNow()
{
	uint8_t* buffer = (uint8_t*)pylonImage.GetBuffer();
	//if (!grabResult->GrabSucceeded())
	//	return Mat(0,0,Scalar(0));
	int width = grabResult->GetWidth();
	int height = grabResult->GetHeight();
	Mat img = Mat(height, width, CV_8UC3, buffer);

	return img;
}

void SaveImage()
{
	uint8_t* buffer = (uint8_t*)pylonImage.GetBuffer();
	int width = grabResult->GetWidth();
	int height = grabResult->GetHeight();
	Mat img = Mat(height, width, CV_8UC3, buffer);
	//Mat img = GetImgNow();
	
	SYSTEMTIME st;
	GetLocalTime(&st);
	char save_name[100];
	snprintf(save_name, 100, "img%d%d%d%d%d.jpg", st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	imwrite(string(save_name), img);
}


ErrorMsg ErrorMsgNow()
{
	if (!grabResult->GrabSucceeded()) 
		return GrabFailed;
	return Default;
}