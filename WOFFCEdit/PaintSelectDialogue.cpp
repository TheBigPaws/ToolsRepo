#include "stdafx.h"
#include "PaintSelectDialogue.h"

// PaintSelectDialogue dialog

IMPLEMENT_DYNAMIC(PaintSelectDialogue, CDialogEx)

//Message map.  Just like MFCMAIN.cpp.  This is where we catch button presses etc and point them to a handy dandy method.
BEGIN_MESSAGE_MAP(PaintSelectDialogue, CDialogEx)
	ON_COMMAND(IDOK, &PaintSelectDialogue::End)					//ok button
	ON_WM_HSCROLL() // Called when Slider is scrolled
	ON_BN_CLICKED(IDOK, &PaintSelectDialogue::OnBnClickedOk)
END_MESSAGE_MAP()



PaintSelectDialogue::PaintSelectDialogue(CWnd* pParent)			//constructor used in modeless
	: CDialogEx(IDD_DIALOG3, pParent)
{


}

PaintSelectDialogue::~PaintSelectDialogue()
{
}


void PaintSelectDialogue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER1, BS_slider);
}



void PaintSelectDialogue::End()
{

	DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}


void PaintSelectDialogue::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	*paintType = BS_slider.GetPos();

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
	//DestroyWindow();	//destory the window properly.  INcluding the links and pointers created.  THis is so the dialogue can start again. 
}


BOOL PaintSelectDialogue::OnInitDialog()
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

	//m_picture1.ModifyStyle(0xF, SS_BITMAP, SWP_NOSIZE);
	//HBITMAP hb = (HBITMAP)::LoadImage(AfxGetInstanceHandle(), IDB_BITMAP1, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	//HBITMAP hBtMpBall = LoadBitmap(NULL, MAKEINTRESOURCE(IDB_BITMAP1)); //Here we have to use the executable module to load our bitmap resource

	BS_slider.SetRange(0, 3, TRUE);
	BS_slider.SetPos(1);
	//CSliderCtrl()

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void PaintSelectDialogue::PostNcDestroy()
{
}




// PaintSelectDialogue message handlers callback   - We only need this if the dailogue is being setup-with createDialogue().  We are doing
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


void PaintSelectDialogue::OnBnClickedOk()
{


	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();


}

