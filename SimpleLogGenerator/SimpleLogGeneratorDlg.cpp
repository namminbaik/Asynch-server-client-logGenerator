
// SimpleLogGeneratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SimpleLogGenerator.h"
#include "SimpleLogGeneratorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
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


// CSimpleLogGeneratorDlg dialog



CSimpleLogGeneratorDlg::CSimpleLogGeneratorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SIMPLELOGGENERATOR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpleLogGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_MALWARE, mCheckMalware);
	DDX_Control(pDX, IDC_LOG_GEN_DATE, mLogGenDate);
	DDX_Control(pDX, IDC_LOG_GEN_TIME, mLogGenTime);
	DDX_Control(pDX, IDC_PROCESS_NAME, mProcessName);
	DDX_Control(pDX, IDC_SENT_DOC_NAME, mSentDocName);
}

BEGIN_MESSAGE_MAP(CSimpleLogGeneratorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDGEN, &CSimpleLogGeneratorDlg::OnBnClickedGen)
END_MESSAGE_MAP()


// CSimpleLogGeneratorDlg message handlers

BOOL CSimpleLogGeneratorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_hEvent = CreateEvent(NULL, FALSE,	FALSE, _T("SLCPEvent"));
	if (m_hEvent == NULL)
	{
		MessageBox(_T("Failed to create event ..."));
		AfxGetMainWnd()->PostMessage(WM_CLOSE);
	}

	memset(&g_SharedData, 0, sizeof(g_SharedData));
	memcpy(&g_SharedData.logCreationPolicy, _T("0"), sizeof(_T("0")));
	memcpy(&g_SharedData.backupFileStorage, _T("0"), sizeof(_T("0")));

	AfxBeginThread(threadProc, m_hEvent);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSimpleLogGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSimpleLogGeneratorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CSimpleLogGeneratorDlg::OnClose()
{
	if (m_hEvent != NULL)
		CloseHandle(m_hEvent);
}

UINT __cdecl threadProc(LPVOID pParam)
{
	HANDLE hEvent = (HANDLE)pParam;

	while (true)
	{
		WaitForSingleObject(hEvent, INFINITE);

		HANDLE hSharedMem;
		LPCTSTR lpMapBuf;

		hSharedMem = OpenFileMapping(
			FILE_MAP_ALL_ACCESS, FALSE, _T("SLCPSharedMemory"));

		if (hSharedMem == NULL)
		{
			MessageBox(NULL, _T("Cannot open shared memory ..."), _T("ALERT"), MB_OK);
		}
		else
		{
			lpMapBuf = (LPTSTR)MapViewOfFile(
				hSharedMem, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SHARED_MEMORY_DATA));

			if (lpMapBuf == NULL)
			{
				MessageBox(NULL, _T("Cannot map view of shared memory ..."), _T("ALERT"), MB_OK);
			}
			else
			{
				memcpy(&g_SharedData, lpMapBuf, sizeof(SHARED_MEMORY_DATA));
				UnmapViewOfFile(lpMapBuf);
			}

			CloseHandle(hSharedMem);
		}
	}
	
	return 0;
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSimpleLogGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSimpleLogGeneratorDlg::OnBnClickedGen()
{
	m_bIsMalware = mCheckMalware.GetCheck();

	if (!memcmp(g_SharedData.logCreationPolicy, _T("1"), sizeof(_T("1"))) || m_bIsMalware == 1)
	{
		mLogGenDate.GetWindowText(m_strDetectDate);
		mLogGenTime.GetWindowText(m_strDetectTime);
		mSentDocName.GetWindowText(m_strDocumentName);
		mProcessName.GetWindowText(m_strProcessName);

		CString strWriteData{ "" };
		strWriteData.Format(_T("%s:%s\n%s:%s\n%s:%s\n%s:%s\n%s:%s\n")
			, _T("Detect Date"), m_strDetectDate
			, _T("Detect Time"), m_strDetectTime
			, _T("Sent Document"), m_strDocumentName
			, _T("Process Name"), m_strProcessName
			, _T("Malware"), (m_bIsMalware == 1 ? _T("YES") : _T("NO"))
		);

		MessageBox(strWriteData);

		TCHAR strCurPath[MAX_PATH];
		ZeroMemory(strCurPath, sizeof(strCurPath));

		_tgetcwd(strCurPath, MAX_PATH);
		TCHAR* lpFind = _tcsrchr(strCurPath, _T('\\'));
		int nFindIndex = static_cast<int>(lpFind - strCurPath + 1);
		memcpy(strCurPath + nFindIndex, _T("log"), sizeof(_T("log")));
		
		CreateDirectory(strCurPath, NULL);

		CTime curTime = CTime::GetCurrentTime();
		CString strCurTimeData = curTime.Format(_T("\\%Y-%m-%d_%Hh%Mm%Ss"));
		CString strfileName(strCurPath + strCurTimeData + _T(".log"));

		CFile fp;
		fp.Open(strfileName, CFile::modeCreate | CFile::modeWrite);
		fp.Write(strWriteData, sizeof(TCHAR) * strWriteData.GetLength());

		fp.Close();
	}
}