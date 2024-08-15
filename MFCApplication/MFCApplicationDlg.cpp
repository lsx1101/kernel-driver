
// MFCApplicationDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCApplication.h"
#include "MFCApplicationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CMFCApplicationDlg 对话框



CMFCApplicationDlg::CMFCApplicationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCAPPLICATION_DIALOG, pParent)
	, SysFilePath(_T(""))
	, m_pid(0)
	, m_address(0)
	, writeData(_T(""))
	, readData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplicationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_MFCEDITBROWSE2, SysFilePath);
	DDX_Text(pDX, IDC_EDIT1, m_pid);
	DDX_Text(pDX, IDC_EDIT2, m_address);
	DDX_Text(pDX, IDC_EDT_PROCESS2, writeData);
	DDX_Text(pDX, IDC_EDT_PROCESS3, readData);
}

BEGIN_MESSAGE_MAP(CMFCApplicationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_Load, &CMFCApplicationDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_Unload, &CMFCApplicationDlg::OnBnClickedBtnUnload)
	ON_BN_CLICKED(IDC_BTN_READ_INT, &CMFCApplicationDlg::OnBnClickedBtnReadInt)
	ON_BN_CLICKED(IDC_BTN_READ_BYTE, &CMFCApplicationDlg::OnBnClickedBtnReadByte)
	ON_BN_CLICKED(IDC_BTN_READ_LONG, &CMFCApplicationDlg::OnBnClickedBtnReadLong)
	ON_BN_CLICKED(IDC_BTN_READ_BYTES, &CMFCApplicationDlg::OnBnClickedBtnReadBytes)
	ON_BN_CLICKED(IDC_BTN_WRITE_BYTE, &CMFCApplicationDlg::OnBnClickedBtnWriteByte)
	ON_BN_CLICKED(IDC_BTN_WRITE_INT, &CMFCApplicationDlg::OnBnClickedBtnWriteInt)
	ON_BN_CLICKED(IDC_BTN_READ_FLOAT, &CMFCApplicationDlg::OnBnClickedBtnReadFloat)
	ON_BN_CLICKED(IDC_BTN_READ_DOUBLE_FLOAT, &CMFCApplicationDlg::OnBnClickedBtnReadDoubleFloat)
	ON_BN_CLICKED(IDC_BTN_WRITE_LONG, &CMFCApplicationDlg::OnBnClickedBtnWriteLong)
	ON_BN_CLICKED(IDC_BTN_WRITE_FLOAT, &CMFCApplicationDlg::OnBnClickedBtnWriteFloat)
	ON_BN_CLICKED(IDC_BTN_WRITE_DOUBLE, &CMFCApplicationDlg::OnBnClickedBtnWriteDouble)
	ON_BN_CLICKED(IDC_BTN_WRITE_BYTES, &CMFCApplicationDlg::OnBnClickedBtnWriteBytes)
END_MESSAGE_MAP()


// CMFCApplicationDlg 消息处理程序

BOOL CMFCApplicationDlg::OnInitDialog()
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

void CMFCApplicationDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplicationDlg::OnPaint()
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
HCURSOR CMFCApplicationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


CString GetFileName(const CString& fullPath)
{
	// 找到最后一个反斜杠的位置
	int lastSlash = fullPath.ReverseFind(_T('\\'));

	// 如果找不到反斜杠，说明路径可能以文件名开始，此时从0开始
	if (lastSlash == -1) lastSlash = 0;
	else                  lastSlash++; // 跳过反斜杠本身

	// 找到最后一个点号的位置（表示文件扩展名的开始）
	int lastDot = fullPath.ReverseFind(_T('.'));

	// 如果找不到点号，说明没有扩展名，直接返回从lastSlash到字符串结束的部分
	if (lastDot == -1 || lastDot < lastSlash)
		return fullPath.Mid(lastSlash);
	else // 否则，返回从lastSlash到lastDot之间的部分
		return fullPath.Mid(lastSlash, lastDot - lastSlash);
}

#include<winioctl.h>
#include"driver.h"
#include"debug.h"
#include"MemoryReaderWriter.h"
#include"DnfHelper.h"

CMemoryReaderWriter memory;
DnfHelper dnfHelper;

CString ServiceName("myDriver");

void CMFCApplicationDlg::OnBnClickedBtnLoad()
{
	// 加载驱动文件
	UpdateData(TRUE);
	m_address = 0x140000000;
	UpdateData(FALSE);
	if (SysFilePath.IsEmpty())
	{
		AfxMessageBox(_T("请先选择文件"));
		return;
	}
	BOOL isDriver = (SysFilePath.Right(4).CompareNoCase(_T(".sys")) == 0);
	if (!isDriver)
	{
		AfxMessageBox(_T("请选择驱动文件,后缀为.sys"));
		return;
	}
	ServiceName = GetFileName(SysFilePath);
	//LoadDriver(ServiceName, SysFilePath);
	if (!LoadDriver1(ServiceName, SysFilePath)) {
		AfxMessageBox(_T("驱动加载失败"));
		return;
	}
	if (!memory.OpenDriver()) {
		AfxMessageBox(_T("无法打开驱动程序"));
		return;
	}
	AfxMessageBox(_T("驱动加载成功"));
}

void CMFCApplicationDlg::OnBnClickedBtnUnload()
{
	// 卸载驱动文件
	UpdateData(TRUE);
	// 关闭驱动
	memory.CloseDriver();
	if (!UnloadDriver1(ServiceName)) {
		AfxMessageBox(_T("驱动卸载失败"));
		return;
	}
	AfxMessageBox(_T("驱动卸载成功"));
}

void CMFCApplicationDlg::OnBnClickedBtnReadByte()
{
	// TODO: 读字节
	UpdateData(TRUE);
	DebugOutput(_T("lsx:读字节 pid=%u address=%p"), m_pid, m_address);
	// 读取字节
	BYTE value;
	if (memory.ReadByte(m_pid, m_address, value)) {
		DebugOutput(_T("读取字节成功，值:%u\n"), value);
		readData.Format(_T("%u"), value);
		UpdateData(0);
	}else{
		DebugOutput(_T("读取字节失败\n"));
	}
}

void CMFCApplicationDlg::OnBnClickedBtnWriteByte()
{
	// 写入字节
	BYTE value = { 0x90 };
	if (memory.WriteByte(m_pid, m_address, value)) {
		DebugOutput(_T("写入字节成功，值=%d\n"), value);
	}
	else {
		DebugOutput(_T("写入字节失败\n"));
	}
}


void CMFCApplicationDlg::OnBnClickedBtnReadInt()
{
	// TODO: 读整形
	UpdateData(TRUE);
	// 读取整形
	WORD byteValue;
	if (memory.ReadWord(m_pid, m_address, byteValue)) {
		DebugOutput(_T("读取整形成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("读取整形失败，值:%u\n"), byteValue);
	}
}

void CMFCApplicationDlg::OnBnClickedBtnReadLong()
{
	// TODO: 读长整形
	UpdateData(TRUE);
	// 读取整形
	DWORD byteValue;
	if (memory.ReadDword(m_pid, m_address, byteValue)) {
		DebugOutput(_T("读取长整形成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("读取长整形失败，值:%u\n"), byteValue);
	}

}

void CMFCApplicationDlg::OnBnClickedBtnReadFloat()
{
	// TODO: 读单浮点
	UpdateData(TRUE);
	float byteValue;
	if (memory.ReadFloat(m_pid, m_address, byteValue)) {
		DebugOutput(_T("读单浮点成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("读单浮点失败，值:%u\n"), byteValue);
	}
}


void CMFCApplicationDlg::OnBnClickedBtnReadDoubleFloat()
{
	// TODO: 读双浮点
	UpdateData(TRUE);
	double byteValue;
	if (memory.ReadDouble(m_pid, m_address, byteValue)) {
		DebugOutput(_T("读双浮点成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("读双浮点失败，值:%u\n"), byteValue);
	}
}

void CMFCApplicationDlg::OnBnClickedBtnReadBytes()
{
	// TODO: 读字节数组
	UpdateData(TRUE);
	// 读字节数组
	BYTE buffer[10] = { 0 };
	if (memory.ReadByteArray(m_pid, m_address, buffer,sizeof(buffer))) {
		DebugOutput(_T("读取字节数组成功，值:"));
		DebugOutput(buffer, 10);
	}
	else {
		DebugOutput(_T("读取字节数组失败，值:%p\n"), buffer);
	}
}




void CMFCApplicationDlg::OnBnClickedBtnWriteInt()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	// 读取整形
	WORD byteValue = 0;
	if (memory.WriteWord(m_pid, m_address, byteValue)) {
		DebugOutput(_T("写整形成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("写整形失败，值:%u\n"), byteValue);
	}
}



void CMFCApplicationDlg::OnBnClickedBtnWriteLong()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	// 读取整形
	DWORD byteValue = 12345;
	if (memory.WriteDword(m_pid, m_address, byteValue)) {
		DebugOutput(_T("写整形成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("写整形失败，值:%u\n"), byteValue);
	}
}


void CMFCApplicationDlg::OnBnClickedBtnWriteFloat()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	// 读取整形
	/*float byteValue = 2;
	if (memory.WriteFloat(m_pid, m_address, byteValue)) {
		DebugOutput(_T("写整形成功，值:%u\n"), byteValue);
	}
	else {
		DebugOutput(_T("写整形失败，值:%u\n"), byteValue);
	}*/
	dnfHelper.TownSpeed((float)2.9);
	dnfHelper.RoleSpeed((float)2.9);
	dnfHelper.RoleAttack((float)8.4);
	dnfHelper.RoleInvinc();
	dnfHelper.RoleSuperArmor();
	dnfHelper.BuildIgnor();
	dnfHelper.MapIgnor();
	dnfHelper.SkipAnimation();
	dnfHelper.QuickFlip();
	dnfHelper.AutoPick();
	dnfHelper.UnlimitedMoveItem();
}


void CMFCApplicationDlg::OnBnClickedBtnWriteDouble()
{
	// TODO: 在此添加控件通知处理程序代码
}
#include<Windows.h>
extern "C" uint64_t  test_asm(uint64_t arg1);
void CMFCApplicationDlg::OnBnClickedBtnWriteBytes()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	IsDebuggerPresent();
	// 读字节数组
	BYTE buffer[2] = { 0x90,0x90 };
	DebugOutput(_T("汇编返回，值:%d\n"), test_asm(1));
	if (memory.WriteByteArray(m_pid, m_address, buffer, 2)) {
		DebugOutput(_T("读取字节数组成功，值:%u\n"), buffer);
	}
	else {
		DebugOutput(_T("读取字节数组失败，值:%u\n"), buffer);
	}
	
}
