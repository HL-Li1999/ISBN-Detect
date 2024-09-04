#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>
#include <opencv2/opencv.hpp>

public deviceNum; //相机数目


void SearchPylonCamera(PylonCamera cam)
{
	CTlFactory& instance = CTlFactory::GetInstance();
	DeviceInfoList_t deviceList;
	deviceNum = instance.EnumerateDevices(deviceList);

	for (int i = 0; i < deviceNum; i++)
	{
		cam.modelName = deviceList[i].Get
	}



}


public struct PylonCamera
{
	public string modelName;
	public string ID;
	public int width;
	public int height;
};