
// ShowPylonImageDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "ShowPylonImage.h"
#include "ShowPylonImageDlg.h"
#include "afxdialogex.h"
#include "PylonCV.h"
#include "RecognizeNum.h"
#include "WinCVMat.h"
#include<opencv2/highgui/highgui_c.h>
#include <pylon/PylonGUI.h>
#include <pylon/PylonImage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv;
using namespace Pylon;

CString msg_text;
void CShowPylonImageDlg::ShowMsg(CString text)
{
	msg_text += text;
	m_MsgBox.SetWindowText(msg_text);
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CShowPylonImageDlg 对话框



CShowPylonImageDlg::CShowPylonImageDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SHOWPYLONIMAGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowPylonImageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CamInfoList, m_CamInfoList);
	DDX_Control(pDX, IDC_StopCam, m_StopCam);
	DDX_Control(pDX, IDC_MsgBox, m_MsgBox);
}

BEGIN_MESSAGE_MAP(CShowPylonImageDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDCANCEL, &CShowPylonImageDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_StopCam, &CShowPylonImageDlg::OnBnClickedStopcam)
	ON_BN_CLICKED(IDC_Search, &CShowPylonImageDlg::OnBnClickedSearch)
	ON_BN_CLICKED(IDC_StartCam, &CShowPylonImageDlg::OnBnClickedStartcam)
	ON_BN_CLICKED(IDC_RecognizeNum, &CShowPylonImageDlg::OnBnClickedRecognizenum)
	ON_BN_CLICKED(IDC_SaveImg, &CShowPylonImageDlg::OnBnClickedSaveimg)
	ON_EN_CHANGE(IDC_MsgBox, &CShowPylonImageDlg::OnEnChangeMsgbox)
END_MESSAGE_MAP()


// CShowPylonImageDlg 消息处理程序

BOOL CShowPylonImageDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShowPylonImageDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShowPylonImageDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CShowPylonImageDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShowPylonImageDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDialogEx::OnTimer(nIDEvent);
}


void CShowPylonImageDlg::OnBnClickedCancel()
{

	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}

int stop_flag = 1;
void CShowPylonImageDlg::OnBnClickedStopcam()
{
	if (stop_flag == 1)
		m_StopCam.SetWindowTextW(L"继续");
	else if (stop_flag == 2)
		m_StopCam.SetWindowTextW(L"暂停");

	ShowImage(NULL, stop_flag);
	stop_flag = 3 - stop_flag;
}


void CShowPylonImageDlg::OnBnClickedSearch()
{
	string text = "开始搜索设备\r\n";
	ShowMsg((CString)text.c_str());

	SearchPylonCam();

	for (int i = 0; i < deviceNum; i++)
	{
		string info = camInfos[i].modelName + "--" + camInfos[i].ID;
		const char* cinfo = info.c_str();
		CString str(cinfo);
		m_CamInfoList.InsertString(i, str);
	}

	m_CamInfoList.SetCurSel(0);
	
	text = "搜索到设备数：" + to_string(deviceNum) + "\r\n";
	ShowMsg((CString)text.c_str());
}


void CShowPylonImageDlg::OnBnClickedStartcam()
{
	CameraStart(m_CamInfoList.GetCurSel());
	CPylonImage img;
	int index = 0;
	ShowImage(&index, 0);
}


void CShowPylonImageDlg::OnBnClickedRecognizenum()
{


	Mat ImgNow = GetImgNow();
	vector<int>  reco_result = GetRecoResult(ImgNow);
	string text = "识别结果：\r\n";
	for (int i = 0; i < reco_result.size(); i++) {
		text += to_string(reco_result[i]);
	}

	text += "\r\n";
	ShowMsg((CString)text.c_str());
}


void CShowPylonImageDlg::OnBnClickedSaveimg()
{
	SaveImage();
}


void CShowPylonImageDlg::OnEnChangeMsgbox()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


