
// SimpleLogGeneratorDlg.h : header file
//

#pragma once

typedef struct _SHARED_MEMORY_DATA
{
	TCHAR logCreationPolicy[2];	// 0: everything,	1: malware only
	TCHAR backupFileStorage[2];	// 0: Delete		1: Storage
}SHARED_MEMORY_DATA, *LPSHARED_MEMORY_DATA;

static SHARED_MEMORY_DATA g_SharedData;

// CSimpleLogGeneratorDlg dialog
class CSimpleLogGeneratorDlg : public CDialogEx
{
// Construction
public:
	CSimpleLogGeneratorDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SIMPLELOGGENERATOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnClose();
	afx_msg void OnBnClickedGen();

private:
	CString m_strFilePath;
	CString m_strDetectDate;
	CString m_strDetectTime;
	CString m_strDocumentName;
	CString m_strProcessName;
	BOOL m_bIsMalware;
	HANDLE m_hEvent;
public:
	CButton mCheckMalware;
	CDateTimeCtrl mLogGenDate;
	CDateTimeCtrl mLogGenTime;
	CEdit mProcessName;
	CEdit mSentDocName;
};

UINT __cdecl threadProc(LPVOID lParam);