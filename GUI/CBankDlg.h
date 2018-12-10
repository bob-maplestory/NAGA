#pragma once

#include "resource.h"

// CBankDlg dialog

class CBankDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBankDlg)

public:
	CBankDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CBankDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BANK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
};
