#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include <vector>


// PaintSelectDialogue dialog

class PaintSelectDialogue : public CDialogEx
{
	DECLARE_DYNAMIC(PaintSelectDialogue)

public:
	PaintSelectDialogue(CWnd* pParent = NULL);
	virtual ~PaintSelectDialogue();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG3 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void End();		//kill the dialogue

	DECLARE_MESSAGE_MAP()
public:
	// Control variable for more efficient access of the listbox
	//CSliderCtrl Slider_;
	virtual BOOL OnInitDialog() override;
	virtual void PostNcDestroy();
	afx_msg void OnBnClickedOk();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl BS_slider;

	int* paintType;
};


//INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);