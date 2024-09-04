// WinCVMat.cpp

//#include "stdafx.h"
#include <stdio.h>
//#include <time.h>
#include <windows.h>
#include <Strsafe.h>
#include "WinCVMat.h"

//#ifdef _DEBUG
//#pragma comment(lib,"opencv_core2413d.lib")
//#pragma comment(lib,"opencv_highgui2413d.lib")
//#pragma comment(lib,"opencv_imgproc2413d.lib")
//#pragma comment(lib,"opencv_features2d2413d.lib")
//#pragma comment(lib,"opencv_calib3d2413d.lib")
//#pragma comment(lib,"opencv_video2413d.lib")
//#pragma comment(lib,"opencv_videostab2413d.lib")
//#pragma comment(lib,"opencv_objdetect2413d.lib")
//#else
//#pragma comment(lib,"opencv_core2413.lib")
//#pragma comment(lib,"opencv_highgui2413.lib")
//#pragma comment(lib,"opencv_imgproc2413.lib")
//#pragma comment(lib,"opencv_features2d2413.lib")
//#pragma comment(lib,"opencv_calib3d2413.lib")
//#pragma comment(lib,"opencv_video2413.lib")
//#pragma comment(lib,"opencv_videostab2413.lib")
//#pragma comment(lib,"opencv_objdetect2413.lib")
//#endif

//#ifdef _DEBUG
//#pragma comment(lib,"opencv_world455d.lib")
//#else
//#pragma comment(lib,"opencv_world455.lib")
//#endif

// BITMAPINFO的缓存
char buffer[sizeof(BITMAPINFO)+255 * sizeof(RGBQUAD)];

/****************************************************************/
/*                     File IO                                  */
/****************************************************************/
// 功能：将二维多通道数组按照行保存为文本文件。这样便于在MATLAB中进一步分析。
int __stdcall SaveMatToText(const char* filename, const cv::Mat& mat)
{
	if (mat.dims != 2)
		return 0;
	int rows = mat.rows;
	int cols = mat.cols;
	int channel = mat.channels();
	int tmp_type = CV_MAKE_TYPE(CV_64F, channel);
	cv::Mat tmp(rows, cols, tmp_type);
	mat.convertTo(tmp, tmp_type); // convert to double type
	int i, j;
	char buf[32];
	//dllLog(filename.c_str());
	FILE *f;
	errno_t err = fopen_s(&f, filename, "w+t");
	if (err)
		return 0;
	for (i = 0; i<rows; ++i) 
	{
		double* var = tmp.ptr<double>(i);
		for (j = 0; j<channel*cols - 1; ++j) 
		{
			StringCbPrintfA(buf, sizeof(buf), "%g, ", var[j]);
			fwrite(buf, 1, strlen(buf), f);
		} // for columns
		// last column
		StringCbPrintfA(buf, sizeof(buf), "%g\r\n", var[j]);
		fwrite(buf, 1, strlen(buf), f);
	} // for rows
	fclose(f);
	return i;
}

// 功能：从文本文件读取二维多通道数组数据。文件可来源于MATLAB命令：save '文件名' 变量名 -ascii -double。
int __stdcall LoadMatFromText(const char* filename, cv::Mat& mat)
{
	if (mat.dims != 2)
		return 0;
	int rows = mat.rows;
	int cols = mat.cols;
	int channel = mat.channels();
	int tmp_type = CV_MAKE_TYPE(CV_64F, channel);
	cv::Mat tmp(rows, cols, tmp_type);
	int i, j;
	char buf[32*4096]; // maximum 4096 columns
	char charSet1[] = { ' ', '\t', 0 }; // white space
	char charSet2[] = { ' ', '\t', '|', ',', ';', '\n', '\r', 0 }; // delimiter
	//dllLog(filename.c_str());
	FILE *f;
	errno_t err = fopen_s(&f, filename, "r+t");
	if (err) 
		return 0;
	for (i = 0; i<rows; ++i) {
		if (!fgets(buf, sizeof(buf), f)) break; // read a line
		char *str = buf;
		double* var = tmp.ptr<double>(i);
		for (j = 0; j<cols*channel; ++j) {
			// process data
			double d = atof(str);
			int pos = (int)strspn(str, charSet1); // find the first character that is not a white space.
			str += pos;
			pos = (int)strcspn(str, charSet2);// find the first character that is a delimiter.
			str += pos + 1;
			if (pos <= 0) break; // end of line
			var[j] = d; // record data
		}
		for (; j<cols*channel; ++j)
			var[j] = 0.0; // filling zeros
	}
	fclose(f);
	int textlines = i;
	for (; i<rows; ++i) {
		double* var = tmp.ptr<double>(i);
		for (j = 0; j<cols*channel; ++j)
			var[j] = 0.0; // filling zeros
	}
	tmp.convertTo(mat, mat.type());
	return textlines;
}

/****************************************************************/
/*                     Bitmap Display                           */
/****************************************************************/
// 辅助函数：使矩形参数正常，即top<bottom，left<right。
void __stdcall ValidRect(RECT& dest, const RECT& src)
{
	dest = src;
	if (dest.top > dest.bottom)
	{
		dest.top = src.bottom;
		dest.bottom = src.top;
	}
	if (dest.left > dest.right)
	{
		dest.left = src.right;
		dest.right = src.left;
	}
}

//// 辅助函数：计算两个矩形的重叠部分。调用前应保证参数正常。
//void __stdcall InterSectRect(RECT& rectDest, const RECT& rectSrc1, const RECT& rectSrc2) 
//{
//	RECT r;
//	rectDest = rectSrc1;
//
//	// y direction
//	if (rectDest.top < r.top)
//	{
//		rectDest.top = r.top;
//	}
//	if (rectDest.bottom > r.bottom)
//	{
//		rectDest.bottom = r.bottom;
//	}
//
//	// x direction
//	if (rectDest.left < r.left)
//	{
//		rectDest.left = r.left;
//	}
//	if (rectDest.right > r.right)
//	{
//		rectDest.right = r.right;
//	}
//}

// 辅助函数：坐标变换x->u: u = k*(x-x0) + u0。
double __stdcall xTou(double x, double x0, double k, double u0)
{
	return (x - x0)*k + u0;
}

// 辅助函数：坐标变换：平移和缩放。矩形左上角为原点
void __stdcall TransformRect(RECT& rectDest, const RECT& rectSrc, int x0, int y0, double x_ratio, double y_ratio, int u0, int v0)
{
	RECT r;
	r.left = (LONG)xTou(rectSrc.left, x0, x_ratio, u0);
	r.right = (LONG)xTou(rectSrc.right, x0, x_ratio, u0);
	r.top = (LONG)xTou(rectSrc.top, y0, y_ratio, v0);
	r.bottom = (LONG)xTou(rectSrc.bottom, y0, y_ratio, v0);
	ValidRect(rectDest, r);
}

// 辅助函数：按照源矩形宽高比aspectRatio切割Dest的宽或高
void __stdcall ClipRect(RECT& Dest, double aspectRatio) 
{
	//double aspectRatio = (double)width / height;
	int w = Dest.right - Dest.left;
	int h = Dest.bottom - Dest.top;
	int cx = (Dest.right + Dest.left) / 2;
	int cy = (Dest.bottom + Dest.top) / 2;
	if (h*aspectRatio<w) {
		Dest.right = cx + (int)(h*aspectRatio / 2);
		Dest.left = cx - (int)(h*aspectRatio / 2);
	}
	else {
		Dest.bottom = cy + (int)(w / aspectRatio / 2);
		Dest.top = cy - (int)(w / aspectRatio / 2);
	}
}

// 辅助函数：提取二维数组的bitmap头
void _stdcall BMPHeaderFromMat(BITMAPINFO* binfo, const cv::Mat& img, int TopDown)
{
	BITMAPINFOHEADER* bmih = &(binfo->bmiHeader);
	bmih->biSize = sizeof(BITMAPINFOHEADER);
	bmih->biWidth = (long)img.cols;
	bmih->biHeight = (long)img.rows;
	if (TopDown)
		bmih->biHeight = -bmih->biHeight;
	bmih->biPlanes = 1;
	bmih->biBitCount = (WORD)img.channels()*8;
	bmih->biCompression = BI_RGB;
	bmih->biSizeImage = DWORD(img.cols*img.rows*img.channels());
	bmih->biClrUsed = 0;
	bmih->biClrImportant = 0;
	if (img.channels() == 1) {
		RGBQUAD* palette = binfo->bmiColors;
		for (int i = 0; i<256; ++i) {
			palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
			palette[i].rgbReserved = 0;
		}
	}
}

// 辅助函数
// 功能：在指定窗口的整个可视区域显示整个图像点阵（保持图像宽高比）。
void __stdcall ShowBmp(HWND hwnd, const BITMAPINFO* bmi, const BYTE* pixels)
{
	if (!hwnd || !bmi || !pixels)
		return;
	int xSrc = 0;
	int ySrc = 0;
	int wSrc = bmi->bmiHeader.biWidth;
	int hSrc = bmi->bmiHeader.biHeight;
	if (hSrc<0) hSrc = -hSrc;
	RECT rectDest;
	::GetClientRect(hwnd, &rectDest);          // 获得窗口大小
	ClipRect(rectDest, (double)wSrc/hSrc);
	int xDest = rectDest.left;
	int wDest = rectDest.right - xDest;
	int yDest = rectDest.top;
	int hDest = rectDest.bottom - yDest;

	// show image
	HDC hdc = ::GetDC(hwnd);                                // 创建窗口用户区设备描述符
	::SetStretchBltMode(hdc, COLORONCOLOR);                 // 避免彩色位图显示失真
	::StretchDIBits(hdc, xDest, yDest, wDest, hDest,
		xSrc, ySrc, wSrc, hSrc, pixels, bmi, DIB_RGB_COLORS, SRCCOPY); // 输出位图
	::ReleaseDC(hwnd, hdc);                                 // 释放窗口设备
}

// 辅助函数
// 功能：在指定窗口的指定矩形显示整个图像点阵（保持图像宽高比）。
void __stdcall ShowBmp2(HWND hwnd, const RECT& rectDest, const BITMAPINFO* bmi, const BYTE* pixels)
{
	if (!hwnd || !bmi || !pixels)
		return;
	RECT rect2; // destination
	ValidRect(rect2, rectDest);
	int xSrc = 0;
	int ySrc = 0;
	int wSrc = bmi->bmiHeader.biWidth;
	int hSrc = bmi->bmiHeader.biHeight;
	if (hSrc<0) hSrc = -hSrc;
	ClipRect(rect2, (double)wSrc / hSrc);
	int xDest = rect2.left;
	int wDest = rect2.right - xDest;
	int yDest = rect2.bottom; // 图像y坐标与窗口y坐标反向
	int hDest = rect2.top - yDest;
	if (!wSrc || !hSrc || !wDest || !hDest)
		return;

	// coordinates transformation 
	double x_ratio = (double)wDest / wSrc;
	double y_ratio = (double)hDest / hSrc;

	// intersect rectangles
	RECT rectWin;
	::GetClientRect(hwnd, &rectWin);          // 获得窗口大小
	RECT rect4;
	::IntersectRect(&rect4, &rectWin, &rect2); // intersect win & dest 
	RECT rect6; // destination on source coordinates
	TransformRect(rect6, rect4, xDest, yDest, 1 / x_ratio, 1 / y_ratio, xSrc, ySrc);

	// re-calculate source and destination
	xSrc = rect6.left;
	ySrc = rect6.top;
	wSrc = rect6.right - xSrc;
	hSrc = rect6.bottom - ySrc;
	xDest = rect4.left;
	wDest = rect4.right - xDest;
	yDest = rect4.top;
	hDest = rect4.bottom - yDest;

	// show image
	HDC hdc = ::GetDC(hwnd);                                // 创建窗口用户区设备描述符
	::SetStretchBltMode(hdc, COLORONCOLOR);                 // 避免彩色位图显示失真
	::StretchDIBits(hdc, xDest, yDest, wDest, hDest,
		xSrc, ySrc, wSrc, hSrc, pixels, bmi, DIB_RGB_COLORS, SRCCOPY); // 输出位图
	::ReleaseDC(hwnd, hdc);                                 // 释放窗口设备
}

// 辅助函数
// 功能：在指定窗口的指定矩形显示图像点阵的指定范围。注意图像0行是底部。
void __stdcall ShowBmpEx(HWND hwnd, const RECT& rectDest,
	const BITMAPINFO* bmi, const BYTE* pixels, const RECT& rectSrc, int mirror, DWORD dwRop)
{
	if (!hwnd || !bmi || !pixels)
		return;
	RECT rect1, rect2; // source, destination
	ValidRect(rect1, rectSrc);
	ValidRect(rect2, rectDest);

	// define transformation of coordinates  
	int xSrc = rect1.left;
	int wSrc = rect1.right - xSrc;
	int ySrc = rect1.top;
	int hSrc = rect1.bottom - ySrc;
	if (mirror & 1) // horizontal mirror
	{
		xSrc = rect1.right;
		wSrc = rect1.left - xSrc;
	}
	if (mirror & 2) // vertical mirror
	{
		ySrc = rect1.bottom;
		hSrc = rect1.top - ySrc;
	}
	int xDest = rect2.left;
	int wDest = rect2.right - xDest;
	int yDest = rect2.bottom; // 图像y坐标与窗口y坐标反向
	int hDest = rect2.top - yDest;
	if (!wSrc || !hSrc || !wDest || !hDest)
		return;
	double x_ratio = (double)wDest / wSrc;
	double y_ratio = (double)hDest / hSrc;

	// intersect rectangles
	RECT rectWin;
	::GetClientRect(hwnd, &rectWin);          // 获得窗口大小
	RECT rectImg = { 0, 0, bmi->bmiHeader.biWidth, bmi->bmiHeader.biHeight };
	if (rectImg.bottom<0)
		rectImg.bottom = -rectImg.bottom;
	RECT rect3; // source on destination coordinates
	TransformRect(rect3, rectImg, xSrc, ySrc, x_ratio, y_ratio, xDest, yDest);
	RECT rect4, rect5;
	::IntersectRect(&rect4, &rectWin, &rect2); // intersect win & dest 
	::IntersectRect(&rect5, &rect3, &rect4);   // intersect win & dest & img
	RECT rect6; // destination on source coordinates
	TransformRect(rect6, rect5, xDest, yDest, 1 / x_ratio, 1 / y_ratio, xSrc, ySrc);

	// re-calculate source and destination
	xSrc = rect6.left;
	wSrc = rect6.right - xSrc;
	ySrc = rect6.top;
	hSrc = rect6.bottom - ySrc;
	xDest = rect5.left;
	wDest = rect5.right - xDest;
	yDest = rect5.top;
	hDest = rect5.bottom - yDest;
	if (mirror & 1) // horizontal mirror
	{
		xDest = rect5.right;
		wDest = rect5.left - xDest;
	}
	if (mirror & 2) // vertical mirror
	{
		yDest = rect5.bottom;
		hDest = rect5.top - yDest;
	}

	// show image
	HDC hdc = ::GetDC(hwnd);                                // 创建窗口用户区设备描述符
	::SetStretchBltMode(hdc, COLORONCOLOR);                 // 避免彩色位图显示失真
	::StretchDIBits(hdc, xDest, yDest, wDest, hDest,
		xSrc, ySrc, wSrc, hSrc, pixels, bmi, DIB_RGB_COLORS, dwRop); // 输出位图
	::ReleaseDC(hwnd, hdc);                                 // 释放窗口设备
}

// 功能：在指定窗口的整个可视区域显示二维矩阵（矩阵图像保持图像比例）。仅支持1~4通道数组。
void _stdcall ShowCVMat(HWND hwnd, const cv::Mat& img, double minVal, double maxVal, int TopDown)
{
	if (!hwnd || img.dims != 2 || img.channels()>4 || minVal>=maxVal)
		return;
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	BMPHeaderFromMat(bmi, img, TopDown);

	// show matrix
	if (img.depth() == CV_8U || img.depth() == CV_8S)
	{
		ShowBmp(hwnd, bmi, img.data);
		return;
	}
	int mat_type = CV_MAKETYPE(CV_8U, img.channels());
	double alpha = 255.0 / (maxVal - minVal);
	double beta = -minVal*alpha;
	cv::Mat M(img.cols, img.rows, mat_type);
	img.convertTo(M, mat_type, alpha, beta);
	ShowBmp(hwnd, bmi, M.data);
}

// 功能：在指定窗口的指定矩形显示二维矩阵（矩阵图像保持图像比例）。仅支持1~4通道数组。
void _stdcall ShowCVMat2(HWND hwnd, const RECT& rectDest, const cv::Mat& img, double minVal, double maxVal, int TopDown)
{
	if (!hwnd || img.dims != 2 || img.channels()>4 || minVal >= maxVal)
		return;
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	BMPHeaderFromMat(bmi, img, TopDown);

	// show matrix
	if (img.depth() == CV_8U || img.depth() == CV_8S)
	{
		ShowBmp2(hwnd, rectDest, bmi, img.data);
		return;
	}
	int mat_type = CV_MAKETYPE(CV_8U, img.channels());
	double alpha = 255.0 / (maxVal - minVal);
	double beta = -minVal*alpha;
	cv::Mat M(img.cols, img.rows, mat_type);
	img.convertTo(M, mat_type, alpha, beta);
	ShowBmp2(hwnd, rectDest, bmi, M.data);
}

// 功能：在指定窗口显示二维矩阵（矩阵图像自上而下，即矩阵第一行对应图像顶部）。仅支持1~4通道数组。
void _stdcall ShowCVMatEx(HWND hwnd, const RECT& rectDest, const cv::Mat& img, const RECT& rectSrc,
	double minVal, double maxVal, int mirror, DWORD dwRop)
{
	if (!hwnd || img.dims != 2 || img.channels()>4 || minVal >= maxVal)
		return;
	BITMAPINFO* bmi = (BITMAPINFO*)buffer;
	BMPHeaderFromMat(bmi, img, 1); // top-down image
	
	// convert matrix rectangle to bitmap rectangle (from bottom line)
	RECT transRect = { rectSrc.left, img.rows - rectSrc.bottom, rectSrc.right, img.rows - rectSrc.top };

	// show matrix
	if (img.depth() == CV_8U || img.depth() == CV_8S)
	{
		ShowBmpEx(hwnd, rectDest, bmi, img.data, transRect, mirror, dwRop);
		return;
	}
	int mat_type = CV_MAKETYPE(CV_8U, img.channels());
	double alpha = 255.0 / (maxVal - minVal);
	double beta = -minVal*alpha;
	cv::Mat M(img.cols, img.rows, mat_type);
	img.convertTo(M, mat_type, alpha, beta);
	ShowBmpEx(hwnd, rectDest, bmi, M.data, transRect, mirror, dwRop);
}
