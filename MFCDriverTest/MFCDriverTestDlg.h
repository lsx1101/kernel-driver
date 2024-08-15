
// MFCDriverTestDlg.h: 头文件
//

#pragma once


// CMFCDriverTestDlg 对话框
class CMFCDriverTestDlg : public CDialogEx
{
// 构造
public:
	CMFCDriverTestDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDRIVERTEST_DIALOG };
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
	UINT64 m_edit_int64;
	CString m_edit_cstr;
	afx_msg void OnEnChangeEdit2();
	afx_msg void OnBnClickedButton1();
};
