#if !defined(AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_)
#define AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AdvSettingsPage_Midi.h : header file
//

/////////////////////////////////////////////////////////////////////////////

#include "LaunchPadSettings.h"
#include "LaunchPadUtil.h"
#include "ContextHelp.h"

/////////////////////////////////////////////////////////////////////////////
// CAdvSettingsPage_Midi dialog

class CAdvSettingsPage_Midi : public CPropertyPage
{
// Construction
public:
	CAdvSettingsPage_Midi(CLaunchPadSettings& settings);
	~CAdvSettingsPage_Midi();

// Helper functions
protected:
  VOID SyncGUIData(BOOL bSave = TRUE);

// Member variables
protected:
  CLaunchPadSettings& m_settings;
  CContextHelp m_help;

// Public MFC stuff
public:

// Dialog Data
	//{{AFX_DATA(CAdvSettingsPage_Midi)
	enum { IDD = IDD_ADVPROPPAGE_MIDI };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAdvSettingsPage_Midi)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CAdvSettingsPage_Midi)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ADVSETTINGSPAGE_MIDI_H__EC36A586_E6F5_41C1_B1CD_C36209895512__INCLUDED_)
