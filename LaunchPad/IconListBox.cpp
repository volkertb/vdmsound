// IconListBox.cpp : implementation file
//

#include "stdafx.h"
#include "launchpad.h"
#include "IconListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIconListBox

CIconListBox::CIconListBox(int icoPadW, int icoPadH) : CListBox(), m_icoPadW(icoPadW), m_icoPadH(icoPadH)
{
}

CIconListBox::~CIconListBox()
{
}

int CIconListBox::LoadIcons(LPCTSTR fName, int index) {
  int icoW = ::GetSystemMetrics(SM_CXICON);
  int icoH = ::GetSystemMetrics(SM_CYICON);

  TCHAR itotBuf[20];
  int numIcons = (int)::ExtractIcon(AfxGetInstanceHandle(), fName, -1);

  UnloadIcons();

  SetRedraw(FALSE);

  SetColumnWidth(icoW + 2 * m_icoPadW);
  SetItemHeight(0, icoH + 2 * m_icoPadH);

  for (int i = 0; i < numIcons; i++) {
    HICON hIcon = ExtractIcon(AfxGetInstanceHandle(), fName, i);
    SetItemData(AddString(_itot(i, itotBuf, 10)), (DWORD)hIcon);
  }

  SetCurSel(abs(index));

  SetRedraw(TRUE);

  return numIcons;
}

void CIconListBox::UnloadIcons(void) {
  SetRedraw(FALSE);

  while (GetCount() > 0) {
    HICON hIcon = (HICON)GetItemData(0);
    DeleteString(0);
    ::DestroyIcon(hIcon);
  }

  SetRedraw(TRUE);
}

BEGIN_MESSAGE_MAP(CIconListBox, CListBox)
	//{{AFX_MSG_MAP(CIconListBox)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIconListBox message handlers

void CIconListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  ASSERT(lpDrawItemStruct != NULL);

  if (!lpDrawItemStruct)
    return;

  HBRUSH hBgBrush = ::GetSysColorBrush((lpDrawItemStruct->itemState & ODS_FOCUS) ? COLOR_HIGHLIGHT : COLOR_WINDOW);
  ::FillRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem), hBgBrush);
  ::DrawIcon(lpDrawItemStruct->hDC, lpDrawItemStruct->rcItem.left + m_icoPadW, lpDrawItemStruct->rcItem.top + m_icoPadH, (HICON)lpDrawItemStruct->itemData);

  if (lpDrawItemStruct->itemState & ODS_SELECTED)
    ::DrawFocusRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem));
}

void CIconListBox::OnDestroy() 
{
	CListBox::OnDestroy();
	
	UnloadIcons();
}
