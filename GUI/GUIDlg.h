
// GUIDlg.h : header file
//

#pragma once


// CGUIDlg dialog
class CGUIDlg : public CDialogEx
{
// Construction
public:
	CGUIDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GUI_DIALOG };
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
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void insertData(LPWSTR name, LPWSTR version);
	CListCtrl m_listView;
	int m_nSelected;
	afx_msg void OnBnClickedDeleteBtn();
	pprogram find_program(CString program_name, std::list<pprogram> temp_list);
	afx_msg void OnBnClickedSelectBtn();
	afx_msg void OnBnClickedResetBtn();
	afx_msg void OnBnClickedBtnVerify();
private:
	std::list<pprogram> my_list;
	std::list<pprogram> delete_list;
public:
	afx_msg void OnDestroy();
	afx_msg void OnLvnColumnclickList1(NMHDR *pNMHDR, LRESULT *pResult);
};


