#pragma once

#include <afxwin.h> 
#include <afxext.h>
#include <afx.h>
#include "pch.h"
#include "Game.h"
#include "ToolMain.h"
#include "resource.h"
#include "MFCFrame.h"
#include "SelectDialogue.h"
#include "BrushSizeDia.h"
#include "PaintSelectDialogue.h"

class MFCMain : public CWinApp 
{
public:
	MFCMain();
	~MFCMain();
	BOOL InitInstance();
	int  Run();

private:

	CMyFrame * m_frame;	//handle to the frame where all our UI is
	HWND m_toolHandle;	//Handle to the MFC window
	ToolMain m_ToolSystem;	//Instance of Tool System that we interface to. 
	CRect WindowRECT;	//Window area rectangle. 
	SelectDialogue m_ToolSelectDialogue;			//for modeless dialogue, declare it here
	BrushSizeDia m_ToolBSDialogue;			//for modeless dialogue, declare it here
	PaintSelectDialogue m_paintSelDialogue;			//for modeless dialogue, declare it here

	int m_width;		
	int m_height;
	
	//Interface funtions for menu and toolbar 
	afx_msg void MenuFileQuit();
	afx_msg void MenuFileSaveTerrain();
	afx_msg void MenuEditSelect();
	afx_msg void MenuEditSetBrushSize();

	afx_msg	void ToolBarButtonSave();
	afx_msg	void ToolBarButtonGrid();
	afx_msg	void ToolBarButtonCursor();
	afx_msg	void ToolBarButtonWireframe();

	afx_msg	void ToolBarButtonRaise();
	afx_msg	void ToolBarButtonLower();
	afx_msg	void ToolBarButtonFlatten();
	afx_msg	void ToolBarButtonPaint();

	afx_msg	void ToolBarButtonMoveObject();
	afx_msg	void ToolBarButtonScaleObject();
	afx_msg	void ToolBarButtonRotateObject();
	afx_msg	void ToolBarFocusCamera();
	afx_msg	void ToolBarToggleArcballCam();


	DECLARE_MESSAGE_MAP()	// required macro for message map functionality  One per class
};
