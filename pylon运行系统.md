# 安装和配置pylon驱动

​	在https://www2.baslerweb.cn/cn/downloads/software-downloads/software-pylon-7-5-0-windows/
可下载pylon相机软件套装，该套装包含易于使用的SDK、驱动程序和工具，可通过Windows PC来操作任意一款Basler相机。注意安装软件套装时应选择development开发者版本安装。

​	安装完毕后，可在Visual Studio中配置pylon驱动。主要步骤如下，请根据自己的安装路径进行调整：

>- 在环境变量中添加F:\Host\Driver\Basler\Development\Assemblies\Basler.Pylon\x64，F:\Host\Driver\Basler\Runtime\x64
>- 在外部包含目录中添加F:\Host\Driver\Basler\Development\include
>- 在库目录在添加F:\Host\Driver\Basler\Development\lib\x64

​	配置完毕后，可使用pylon相机SDK来操作相机，其中常见的API如下。

# 1.pylon运行系统

## 1.1.初始化pylon运行系统

```C++
void PylonInitialize(void)
```

初始化pylon运行系统，必须在调用任意其它pylon函数前调用该函数。

## 1.2.释放pylon运行系统资源

```C++
void PylonTerminate(bool ShutDownLogging =true)
```

释放由pylon运行时系统分配的资源。

在终止应用程序之前调用此函数。在调用PylonTerminate()之后不要使用任何pylon方法或pylon对象。

PylonInitialize/PylonTerminate是引用计数的。对于每次调用PylonInitialize，都需要调用PylonTerminate。

# 2.访问相机设备

## 2.1.CTlFactory类

Pylon::CTlFactory类提供访问相机设备的接口

`#include <pylon/TlFactory.h>`

### 2.1.1.初始化CTIFactory对象

```C++
static CTlFactory& GetInstance()
```

创建传输层单例对象。

### 2.1.2.检索相机设备并获取相机信息

```C++
virtual int EnumerateDevices(DeviceInfoList_t& list, bool addToList=false)
```

检索可用相机设备并将对应的相机信息填入DeviceInfoList_t列表（见3.2）。

**参数：**

- **list**：用返回检索到的相机设备列表，DeviceInfoList每个元素为一个CDeviceInfo对象。

- **addToList**：若为true则获将检索到的设备找到的设备将被添加到列表中，而不是删除列表。若为false，则会删除列表，新建一个包含检索到的设备的列表。一般用默认值即可。

**返回值：**检索到的设备数。

### 2.1.3.创建IPylonDevice对象

```C++
virtual IPylonDevice* CreateDevice(const CDeviceInfo& di)
```

从设备信息对象创建一个相机对象。

**参数**：

- **di**：设备信息对象，包含准确识别一个设备所需的所有信息。

**返回值**：pylon设备对象。

# 3.相机信息

Pylon::CDeviceInfo类提供访问和修改相机属性的接口。

`#include <pylon/DeviceInfo.h>`

## 3.1.CDeviceInfo类

### 3.1.1.获取/设置设备型号

```C++
String_t GetModelName() const
```

```C++
CDeviceInfo& SetModelName(const String_t & ModelNameValue)
```

### 3.1.2.获取/设置设备版本

```C++
String_t GetDeviceVersion() const
```

```C++
CDeviceInfo & SetDeviceVersion(const String_t & DeviceVersionValue)
```

### 3.1.3.获取/设置设备地址：IP+Port

```C++
String_t GetAddress() const
```

```C++
CDeviceInfo & SetAddress(const String_t & AddressValue)
```

## 3.2.DeviceInfoList类

Pylon::DeviceInfoList对象为Pylon::CDeviceInfo对象容器，提供了CDeviceInfo类迭代器，可通过[]运算符访问容器元素。

`#include <pylon/Container.h>`

# 4.相机采集图像

## 4.1.CInstantCamera类

Pylon::CInstantCamera类提供使用相机功能的接口，通过上一节创建的IPylonDevice对象来使用对应相机。

`#include <pylon/InstantCamera.h>`

### 4.1.1.关联IPylonDevice对象和CInstantCamera对象

```C++
virtual void Attach(IPylonDevice * pDevice,ECleanup cleanupProcedure=Cleanup_Delete)
```

关联IPylonDevice对象和CInstantCamera对象。

**参数：**

- **pDevice**：要关联的pylon设备。
- **cleanupProcedure**：设置为Cleanup_Delete时会在销毁该相机对象时销毁Pylon设备。

### 4.1.2.打开相机设备

```C++
virtual void Open()
```

打开CInstantCamera对象关联的相机设备。

### 4.1.3.相机启动/停止采集图像

```C++
virtual void StartGrabbing(EGrabStrategy strategy=GrabStrategy_OneByOne, EGrabLoop grabLoopType=GrabLoop_ProvidedByUser)
```

```C++
virtual void StopGrabbing()
```

关联的相机设备启动/停止采集图像。

### 4.1.4.获取相机采集图像

```C++
virtual bool RetrieveResult(
    unsigned int timeoutMs,
    CGrabResultPtr & grabResult,
    ETimeoutHandling timeoutHandling =TimeoutHandling_ThrowException
)
```

获取关联的相机设备采集到的图像，保存为CGrabResultPtr对象。

**参数**：

- **timeoutMs**：等待抓取结果的超时值(以毫秒为单位)。
- **grabResult**：用于返回采集的图像结果。
- **timeoutHandling**：设置为TimeoutHandling_ThrowException时会在等待超时后抛出超时异常。

**返回值**：采集成功则返回true，否则返回false。

## 4.2.CInstantCameraArray类

Pylon::CInstantCameraArray类提供多个相机操作的接口，相当于CInstantCamera对象的数列，可通过[]运算符访问数列元素。

`#include <pylon/InstantCameraArray.h>`

### 4.2.1.初始化

```C++
virtual void Initialize(size_t numberOfCameras)
```

初始化CInstantCameraArray对象。

**参数**：

- **numberOfCameras**：该数列可容纳的CInstantCamera对象的数目。

### 4.2.2.相机启动/停止采集图像

```C++
virtual void StartGrabbing(EGrabStrategy strategy=GrabStrategy_OneByOne, EGrabLoop grabLoopType=GrabLoop_ProvidedByUser)
```

```C++
virtual void StopGrabbing()
```

数列所有相机启动/停止采集图像。

# 5.采集的图像信息

## 5.1.CGrabResultData类

一个CGrabResultPtr对象为一个Pylon::CGrabResultData对象的指针，因此CGrabResultPtr对象可通过$\rightarrow$运算符来调用Pylon::CGrabResultData类成员函数。

`#include <pylon/GrabResultData.h>`

### 5.1.1.成功采集到图像

```C++
bool GrabSucceeded() const
```

**返回值**：如果成功抓取图像，则返回true；如果出现错误，则返回false。

### 5.1.2.获取相机上下文值

```C++
intptr_t GetCameraContext() const
```

**返回值**：分配给相机对象的上下文值，该值可用于后续显示采集到的图像。

### 5.1.3.获取图像高度、宽度

```C++
uint32_t GetHeight() const
```

**返回值**：图像的高度。

```C++
uint32_t GetWidth() const
```

**返回值**：图像的宽度。

### 5.1.4.获取图像缓冲区

```C++
void * GetBuffer() const
```

**返回值**：图像的缓冲区指针。

### 5.1.5.获取图像采集的错误信息

```C++
String_t GetErrorDescription() const
```

**返回值**：采集图像出现错误时，具体的错误信息。

# 6.转换图像格式

## 6.1.Pylon::CImageFormatConverter类

Pylon::CImageFormatConverter类通过将源图像转换为另一种格式来创建新图像。

`#include <pylon/ImageFormatConverter.h>`

### 6.1.1.设置输出图像属性

通过CImageFormatConverter的成员属性来设置转换器输出图像属性，主要的成员属性如下：

| 类型                                               | 名称                    | 功能                                                         |
| -------------------------------------------------- | ----------------------- | ------------------------------------------------------------ |
| Pylon::IIntegerEx &                                | **AdditionalLeftShift** | 用于转换单色图像的附加移位值-适用于:ImageFormatConverter。   |
| Pylon::IFloatEx &                                  | **Gamma**               | 用于转换单色图像的伽马值-适用于:ImageFormatConverter。       |
| Pylon::IEnumParameterT< OutputBitAlignmentEnums> & | **OutputBitAlignment**  | 设置目标像素类型中位的对齐方式-适用于:ImageFormatConverter。 |
| Pylon::IEnumParameterT< OutputOrientationEnums> &  | **OutputOrientation**   | 设置缓冲区中输出图像的垂直方向-适用于:ImageFormatConverter。 |
| Pylon::IIntegerEx &                                | **OutputPaddingX**      | 每行末尾的附加数据字节数-适用于:ImageFormatConverter。       |
| IOutputPixelFormatEnum &                           | **OutputPixelFormat**   | 输出图像的像素数据格式。                                     |

### 6.1.2.转换图像格式

```C++
virtual void Convert(IReusableImage & destinationImage, const IImage & sourceImage)
```

通过将图像转换为不同的格式来创建新图像。

**参数**：

- **destinationImage**：输出图像，例如CPylonImage或CPylonBitmapImage对象。当传递CPylonBitmapImage对象时，目标格式必须为CPylonBitmapImage类所支持。

- **sourceImage**：源图像，例如CPylonImage, CPylonBitmapImage，或CGrabResultPtr。

# 7.图像窗口

由于opencv不支持usb3.0协议，采用opencv api采集或显示图像可能会出现问题（比如显示延迟等）。

## 7.1.CPylonImageWindow类

Pylon::CPylonImageWindow类提供图像窗口的相关接口。

`#include <pylon/PylonGUI.h>`

### 7.1.1.创建一个窗口

```C++
void Create(
    size_t winIndex,
    int x =useDefault,
    int y =useDefault,
    int nWidth =useDefault,
    int nHeight =useDefault
)
```

根据给定属性创建一个窗口。

**参数**：

- **winIndex**：窗口索引，有效值为0~31。
- **x**：窗口左上角的x坐标(屏幕坐标)。
- **y**：窗口左上角的y坐标(屏幕坐标)。
- **nWidth**：窗口在屏幕坐标中的宽度。
- **nHeight**： 窗口在屏幕坐标中的高度。

### 7.1.2.获取窗口句柄

```C++
HWND GetWindowHandle() const
```

返回图像窗口的窗口句柄(HWND)。

### 7.1.3.设置显示图像

```C++
void SetImage(const Pylon::IImage & image)
```

设置图像窗口的显示内容。

**参数**：

- **image**：要在窗口中显示的内容，可为任意Pylon::IImage的子类对象，如CPylonImage或CPylonBitmapImage对象。

### 7.1.4.显示图像窗口

```C++
void Show(int nShow=showDefault) const
```

显示图像窗口。

# 8.例子

## 8.1.使用pylon窗口显示相机图像

```C++
#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>
#include <opencv2/opencv.hpp>
#include <pylon/PylonGUI.h>`

using namespace Pylon;
using namespace cv;

int main()
{
	//初始化pylon运行系统
	PylonInitialize();

	CTlFactory& instance = CTlFactory::GetInstance();

	//检索相机设备并获取相机信息
	DeviceInfoList_t deviceList;
	int deviceNum = instance.EnumerateDevices(deviceList);
	std::cout << "检测到相机数：" << deviceNum << std::endl;

	CInstantCameraArray camera;
	camera.Initialize(deviceNum);

	//打印相机信息
	for (int i = 0; i < deviceNum; i++)
	{
		std::cout << i+1 << std::endl;
		std::cout << "相机型号：" << deviceList[i].GetModelName() << std::endl;
		std::cout << "设备版本：" << deviceList[i].GetDeviceVersion() << std::endl;
		
		IPylonDevice* pylonDevice = instance.CreateDevice(deviceList[i]);
		camera[i].Attach(pylonDevice);
	}

	int index = 0;

	//打印图像信息

	GenApi::INodeMap& nodemap = camera[index].GetNodeMap();
	camera[index].Open();
	GenApi::CIntegerPtr width = nodemap.GetNode("Width");
	GenApi::CIntegerPtr height = nodemap.GetNode("Height");
	std::cout << "图像宽度为" << (int)width->GetValue() << std::endl;
	std::cout << "图像高度为" << (int)height->GetValue() << std::endl;

	CImageFormatConverter formatConverter;
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;
	CPylonImage pylonImage;

	camera[index].StartGrabbing();

	CPylonImageWindow imageWindow;
	imageWindow.Create(1, 0, 0, 1920, 1200);

	CGrabResultPtr grabResult;
	//相机采集图像并转换格式
	while (camera[index].IsGrabbing())
	{
		camera[index].RetrieveResult(5000, grabResult);

		if (grabResult->GrabSucceeded())
		{
			formatConverter.Convert(pylonImage, grabResult);
			imageWindow.SetImage(pylonImage);
			imageWindow.Show();
		}
	}

	//释放pylon运行系统资源
	camera[index].Close();
	PylonTerminate();	
}
```

![image-20240902180949388](https://raw.githubusercontent.com/HL-Li1999/CloudPic/master/img/image-20240902180949388.png)

<img src="https://raw.githubusercontent.com/HL-Li1999/CloudPic/master/img/image-20240902181515346.png" alt="image-20240902181513412" style="zoom:33%;" />

## 8.2.使用OpenCV显示相机图像

```C++
#include <pylon/PylonIncludes.h>
#include <pylon/TlFactory.h>
#include <opencv2/opencv.hpp>

using namespace Pylon;
using namespace cv;

int main()
{
	//初始化pylon运行系统
	PylonInitialize();

	CTlFactory& instance = CTlFactory::GetInstance();

	//检索相机设备并获取相机信息
	DeviceInfoList_t deviceList;
	int deviceNum = instance.EnumerateDevices(deviceList);
	std::cout << "检测到相机数：" << deviceNum << std::endl;

	CInstantCameraArray camera;
	camera.Initialize(deviceNum);

	//打印相机信息
	for (int i = 0; i < deviceNum; i++)
	{
		std::cout << i+1 << std::endl;
		std::cout << "相机型号：" << deviceList[i].GetModelName() << std::endl;
		std::cout << "设备版本：" << deviceList[i].GetDeviceVersion() << std::endl;
		
		IPylonDevice* pylonDevice = instance.CreateDevice(deviceList[i]);
		camera[i].Attach(pylonDevice);
	}

	int index = 0;
	CGrabResultPtr grabResult;

	//打印图像信息
    camera[index].Open();
	camera[index].StartGrabbing();
	if (camera[index].RetrieveResult(5000, grabResult))
	{
		std::cout << "图像宽度为" << grabResult->GetWidth() << std::endl;
		std::cout << "图像高度为" << grabResult->GetHeight() << std::endl;
	}
	
	CImageFormatConverter formatConverter;
	formatConverter.OutputPixelFormat = PixelType_BGR8packed;
	CPylonImage pylonImage;

	//相机采集图像并转换格式
	while (camera[index].IsGrabbing())
	{
		camera[index].RetrieveResult(5000, grabResult);

		formatConverter.Convert(pylonImage, grabResult);

		 Mat image = Mat(grabResult->GetHeight(), grabResult->GetWidth(), CV_8UC3, (uint8_t*)pylonImage.GetBuffer());
		 namedWindow("相机图像", 1);
		 imshow("相机图像", image);

		 waitKey(10);
	}

	//释放pylon运行系统资源
	PylonTerminate();
	return 0;
}
```

<img src="C:/Users/llll/AppData/Roaming/Typora/typora-user-images/image-20240823120058969.png" alt="image-20240823120058969" style="zoom:67%;" />

<img src="https://raw.githubusercontent.com/HL-Li1999/CloudPic/master/img/image-20240902181650887.png" alt="image-20240902181650887" style="zoom:33%;" />

