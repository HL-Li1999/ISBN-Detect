
// ShowPylonImageDlg.h: 头文件
//

#pragma once


// CShowPylonImageDlg 对话框
class CShowPylonImageDlg : public CDialogEx
{
// 构造
public:
	CShowPylonImageDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SHOWPYLONIMAGE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	void ShowMsg(CString text);
public:
	CComboBox m_CamInfoList;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedStopcam();
	afx_msg void OnBnClickedSearch();
	afx_msg void OnBnClickedStartcam();
	CButton m_StopCam;
	afx_msg void OnBnClickedRecognizenum();
	afx_msg void OnBnClickedSaveimg();
	CEdit m_MsgBox;
	afx_msg void OnEnChangeMsgbox();
};
