// Gen_Iso_ToolDlg.h : 標頭檔
//

#pragma once
#include "afxwin.h"

typedef enum
{
	SET_2K = 0,
	SET_4K,
	SET_MAX
} E_SETTING;

// CGen_Iso_ToolDlg 對話方塊
class CGen_Iso_ToolDlg : public CDialog
{
// 建構
public:
	CGen_Iso_ToolDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_GEN_ISO_TOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援

// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedSrcIso();
	afx_msg void OnBnClickedOutIso();
	afx_msg void OnBnClickedGenIso();
	afx_msg void OnBnClickedRadio2k();
	afx_msg void OnBnClickedRadio4k();

	BOOL ExeCommand();
	CString Get_Curr_Path();
	BOOL CheckMkyaffs2ImageExist();
	void SettingLayout(E_SETTING e_Setting);

	BOOL bIsReady;
	CEdit m_IsoSrcPath;
	CEdit m_IsoOutPath;	
	CString strIsoSrc;
	CString strIsoOut;
	CString m_strCur;
	CString strMessage;	
	CString m_PageSize;
	CString m_SpareSize;
	E_SETTING m_eSetting;
	CButton m_ctrlRadio_2K;	
};
