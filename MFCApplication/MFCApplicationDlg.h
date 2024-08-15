
// MFCApplicationDlg.h: 头文件
//

#pragma once


// CMFCApplicationDlg 对话框
class CMFCApplicationDlg : public CDialogEx
{
// 构造
public:
	CMFCApplicationDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION_DIALOG };
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
public:
	CString SysFilePath;
	afx_msg void OnBnClickedBtnLoad();
	afx_msg void OnBnClickedBtnUnload();
	ULONG m_pid;
	ULONG_PTR m_address;
	afx_msg void OnBnClickedBtnReadInt();
	afx_msg void OnBnClickedBtnReadByte();
	ULONG p_pid;
	afx_msg void OnBnClickedBtnReadLong();
	afx_msg void OnBnClickedBtnReadBytes();
	afx_msg void OnBnClickedBtnWriteByte();
	afx_msg void OnBnClickedBtnWriteInt();
	afx_msg void OnBnClickedBtnReadFloat();
	afx_msg void OnBnClickedBtnReadDoubleFloat();
	afx_msg void OnBnClickedBtnWriteLong();
	afx_msg void OnBnClickedBtnWriteFloat();
	afx_msg void OnBnClickedBtnWriteDouble();
	afx_msg void OnBnClickedBtnWriteBytes();
	CString writeData;
	CString readData;
};
