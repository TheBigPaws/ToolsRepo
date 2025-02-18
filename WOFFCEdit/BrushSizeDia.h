
#pragma once
#include "afxdialogex.h"
#include "resource.h"
#include "afxwin.h"
#include <vector>


// BrushSizeDia dialog

class BrushSizeDia : public CDialogEx
{
	DECLARE_DYNAMIC(BrushSizeDia)

public:
	BrushSizeDia(CWnd* pParent = NULL);
	virtual ~BrushSizeDia();

	// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG2 };
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

	float* brushSize;
};


//INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);