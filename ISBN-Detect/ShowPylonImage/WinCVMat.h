// WinCVMat.h
//
// Windows interface for Mat object of openCV
// by Lin, Jingyu, linjy02@hotmail.com, 2016.08
//
#ifndef WinCVMat_H
#define WinCVMat_H

//#include "opencv/cv.h"
//#include "opencv/highgui.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

//#define WinCVMatExport extern "C" __declspec(dllexport)
#define WinCVMatExport

/****************************************************************/
/*                     File IO                                  */
/****************************************************************/
// ���ܣ�����ά��ͨ�����鰴���б���Ϊ�ı��ļ�������������MATLAB�н�һ��������
// ���룺filename - �ı��ļ�����mat - Դ���顣
// ���������д���ı��ļ���
// ����ֵ���ı�������
// ˵�����ı��ļ���һ��Ϊ�����һ�У�һ�������ö��ŷָ�����ʽ���£�
//       ��һ��ͨ��1����һ��ͨ��2��...����һ��ͨ��K���ڶ���ͨ��1��...����N��ͨ��K��
WinCVMatExport int __stdcall SaveMatToText(const char* filename, const cv::Mat& mat);

// ���ܣ����ı��ļ���ȡ��ά��ͨ���������ݡ��ļ�����Դ��MATLAB���save '�ļ���' ������ -ascii -double��
// ���룺filename - �ı��ļ�����mat - ��ȡ���ݵ����飬��������������ͨ�����涨���ı��ļ��ĸ�ʽ��
// �����mat - ��ȡ���ı��ļ���ȡ�����ݡ����ݲ�������0��
// ����ֵ���ı�������
// ˵�����ı��ļ���һ��Ϊ�����һ�У�һ�������ö��š��ֺš���|����հ׷ָ�����ʽ���£�
//       ��һ��ͨ��1����һ��ͨ��2��...����һ��ͨ��K���ڶ���ͨ��1��...����N��ͨ��K��
WinCVMatExport int __stdcall LoadMatFromText(const char* filename, cv::Mat& mat);

/****************************************************************/
/*                     Bitmap Display                           */
/****************************************************************/
// ShowCVMat��ShowCVMat2��ʾ����ͼ�񣬱���ͼ���߱ȡ�ǰ��ռ���������ڣ����߿��Ըı���ʾλ�á�
// ShowCVMatEx��ʵ�ֶ�����ʾ��ʽ��

// ���ܣ���ָ�����ڵ���������������ʾ������ά���󣨾���ͼ�񱣳�ͼ���߱ȣ�����֧��1~4ͨ�����顣
// ���룺hwnd - ������ʾ����Ĵ��ڡ�mat - ��ά�������ݡ�
//       minVal, maxVal - ����Ԫ�ص��½���Ͻ磬�����½����ֵת��Ϊ��Сǿ��ֵ�������Ͻ����ֵת��Ϊ���ǿ��ֵ��
//       TopDown - ����ͼ���Ƿ����϶��¡�1��ʾ�����һ�ж�Ӧͼ�񶥲���0��ʾ�����һ�ж�Ӧͼ��ײ���
// ˵����Ƶ����ʾӦʹ����������Ϊ8bit����CV_8U��CV_8S���ľ��󣨲���Ҫ����ת��������ҪminVal��maxVal����
WinCVMatExport void _stdcall ShowCVMat(HWND hwnd, const cv::Mat& img, double minVal, double maxVal, int TopDown);

// ���ܣ���ָ�����ڵ�ָ��������ʾ������ά���󣨾���ͼ�񱣳�ͼ���߱ȣ�����֧��1~4ͨ�����顣
// ���룺hwnd - ������ʾ����Ĵ��ڡ�rectWin - ������������ʾ������mat - ��ά�������ݡ�
//       minVal, maxVal - ����Ԫ�ص��½���Ͻ磬�����½����ֵת��Ϊ��Сǿ��ֵ�������Ͻ����ֵת��Ϊ���ǿ��ֵ��
//       TopDown - ����ͼ���Ƿ����϶��¡�1��ʾ�����һ�ж�Ӧͼ�񶥲���0��ʾ�����һ�ж�Ӧͼ��ײ���
// ˵����Ƶ����ʾӦʹ����������Ϊ8bit����CV_8U��CV_8S���ľ��󣨲���Ҫ����ת��������ҪminVal��maxVal����
WinCVMatExport void _stdcall ShowCVMat2(HWND hwnd, const RECT& rectDest, const cv::Mat& img, double minVal, double maxVal, int TopDown);

// ���ܣ���ָ�����ڵ�ָ��������ʾ��ά�����ָ����Χ����֧��1~4ͨ�����󡣾���ͼ�����϶��£��������һ�ж�Ӧͼ�񶥲���
// ���룺hwnd - ������ʾ����Ĵ��ڡ�rectWin - ������������ʾ������
//       mat - ��ά�������ݡ�rectMat - ������������ʾ�����򣨲���ͼ�����򣩡�
//       minVal, maxVal - ����Ԫ�ص��½���Ͻ磬�����½����ֵת��Ϊ��Сǿ��ֵ�������Ͻ����ֵת��Ϊ���ǿ��ֵ��
//       mirror - ������ʾ��0��ʾԭͼ��1���Ҿ���2���¾���3�������Ҿ���
//       dwRop - ��դ�����룬ȡֵ����BitBlt������ֵ���£�
//#define SRCCOPY             (DWORD)0x00CC0020 /* dest = source                   */
//#define SRCPAINT            (DWORD)0x00EE0086 /* dest = source OR dest           */
//#define SRCAND              (DWORD)0x008800C6 /* dest = source AND dest          */
//#define SRCINVERT           (DWORD)0x00660046 /* dest = source XOR dest          */
//#define SRCERASE            (DWORD)0x00440328 /* dest = source AND (NOT dest )   */
//#define NOTSRCCOPY          (DWORD)0x00330008 /* dest = (NOT source)             */
//#define NOTSRCERASE         (DWORD)0x001100A6 /* dest = (NOT src) AND (NOT dest) */
//#define MERGECOPY           (DWORD)0x00C000CA /* dest = (source AND pattern)     */
//#define MERGEPAINT          (DWORD)0x00BB0226 /* dest = (NOT source) OR dest     */
//#define PATCOPY             (DWORD)0x00F00021 /* dest = pattern                  */
//#define PATPAINT            (DWORD)0x00FB0A09 /* dest = DPSnoo                   */
//#define PATINVERT           (DWORD)0x005A0049 /* dest = pattern XOR dest         */
//#define DSTINVERT           (DWORD)0x00550009 /* dest = (NOT dest)               */
//#define BLACKNESS           (DWORD)0x00000042 /* dest = BLACK                    */
//#define WHITENESS           (DWORD)0x00FF0062 /* dest = WHITE                    */
// ˵����Ƶ����ʾӦʹ����������Ϊ8bit����CV_8U��CV_8S���ľ��󣨲���Ҫ����ת��������ҪminVal��maxVal����
WinCVMatExport void _stdcall ShowCVMatEx(HWND hwnd, const RECT& rectWin, const cv::Mat& img, const RECT& rectMat,
	double minVal, double maxVal, int mirror, DWORD dwRop = SRCCOPY);

#endif // #ifndef WinOpenCV_H
