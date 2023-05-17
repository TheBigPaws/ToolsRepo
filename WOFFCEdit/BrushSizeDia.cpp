
#include "stdafx.h"
#include "BrushSizeDia.h"

// BrushSizeDia dialog

IMPLEMENT_DYNAMIC(BrushSizeDia, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(BrushSizeDia, CDialogEx)
	ON_COMMAND(IDOK, &BrushSizeDia::End)					//ok button
	ON_WM_HSCROLL() // Called when Slider is scrolled
	//ok button
	ON_BN_CLICKED(IDOK, &BrushSizeDia::OnBnClickedOk)
END_MESSAGE_MAP()



BrushSizeDia::BrushSizeDia(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_DIALOG2, pParent)
{

	
}

BrushSizeDia::~BrushSizeDia()
{
}


void BrushSizeDia::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, BS_slider);
}



void BrushSizeDia::End()
{

	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}

void BrushSizeDia::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	*brushSize = float(BS_slider.GetPos())/100.0f;

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	//DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}

BOOL BrushSizeDia::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	//uncomment for modal only
/*	//roll through all the objects in the scene graph and put an entry for each in the listbox
	int numSceneObjects = m_sceneGraph->size();
	for (size_t i = 0; i < numSceneObjects; i++)
	{
		//easily possible to make the data string presented more complex. showing other columns.
		std::wstring listBoxEntry = std::to_wstring(m_sceneGraph->at(i).ID);
		m_listBox.AddString(listBoxEntry.c_str());
	}*/

	BS_slider.SetRange(100, 2000, TRUE);
	BS_slider.SetPos(1000);

	//CSliderCtrl()

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void BrushSizeDia::PostNcDestroy()
{
}




// BrushSizeDia message handlers callback   - We only need this if the dailogue is being setup-with createDialogue().  We are doing
//it manually so its better to use the messagemap
/*INT_PTR CALLBACK SelectProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		//	EndDialog(hwndDlg, wParam);
			DestroyWindow(hwndDlg);
			return TRUE;


		case IDCANCEL:
			EndDialog(hwndDlg, wParam);
			return TRUE;
			break;
		}
	}

	return INT_PTR();
}*/


void BrushSizeDia::OnBnClickedOk()
{
	

	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();


}

