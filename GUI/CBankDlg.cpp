// CBankDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CBankDlg.h"
#include "afxdialogex.h"


// CBankDlg dialog

IMPLEMENT_DYNAMIC(CBankDlg, CDialogEx)

CBankDlg::CBankDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BANK_DIALOG, pParent)
{

}

CBankDlg::~CBankDlg()
{
}

void CBankDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBankDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CBankDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CBankDlg message handlers


void CBankDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	OnDestroy();
	OnOK();
}
