VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   Caption         =   "VDMSound LaunchPad"
   ClientHeight    =   5775
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   7935
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5775
   ScaleWidth      =   7935
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ImageList imlTreeIcons 
      Left            =   4920
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   2
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":08CA
            Key             =   "Folder Closed"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0D1C
            Key             =   "Folder Open"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ImageList imlToolbarIcons 
      Left            =   4320
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   11
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":116E
            Key             =   "Back"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1280
            Key             =   "Forward"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1392
            Key             =   "Cut"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":14A4
            Key             =   "Copy"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":15B6
            Key             =   "Paste"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":16C8
            Key             =   "Delete"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":17DA
            Key             =   "Properties"
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":18EC
            Key             =   "View Large Icons"
         EndProperty
         BeginProperty ListImage9 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":19FE
            Key             =   "View Small Icons"
         EndProperty
         BeginProperty ListImage10 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1B10
            Key             =   "View List"
         EndProperty
         BeginProperty ListImage11 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1C22
            Key             =   "View Details"
         EndProperty
      EndProperty
   End
   Begin VB.PictureBox picSplitter 
      BackColor       =   &H00808080&
      BorderStyle     =   0  'None
      FillColor       =   &H00808080&
      Height          =   4800
      Left            =   8880
      ScaleHeight     =   2090.126
      ScaleMode       =   0  'User
      ScaleWidth      =   780
      TabIndex        =   7
      Top             =   705
      Visible         =   0   'False
      Width           =   72
   End
   Begin MSComctlLib.TreeView tvTreeView 
      Height          =   4800
      Left            =   0
      TabIndex        =   6
      Top             =   705
      Width           =   2370
      _ExtentX        =   4180
      _ExtentY        =   8467
      _Version        =   393217
      Indentation     =   0
      LineStyle       =   1
      PathSeparator   =   "."
      Style           =   7
      ImageList       =   "imlTreeIcons"
      Appearance      =   1
   End
   Begin MSComctlLib.ListView lvListView 
      Height          =   4800
      Left            =   2415
      TabIndex        =   5
      Top             =   705
      Width           =   5610
      _ExtentX        =   9895
      _ExtentY        =   8467
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   0
   End
   Begin VB.PictureBox picTitles 
      Align           =   1  'Align Top
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   300
      Left            =   0
      ScaleHeight     =   300
      ScaleWidth      =   7935
      TabIndex        =   2
      TabStop         =   0   'False
      Top             =   420
      Width           =   7935
      Begin VB.Label lblTitle 
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Applications"
         Height          =   270
         Index           =   1
         Left            =   2445
         TabIndex        =   4
         Tag             =   " ListView:"
         Top             =   15
         Width           =   5610
      End
      Begin VB.Label lblTitle 
         BorderStyle     =   1  'Fixed Single
         Caption         =   "Categories"
         Height          =   270
         Index           =   0
         Left            =   0
         TabIndex        =   3
         Tag             =   " TreeView:"
         Top             =   15
         Width           =   2370
      End
   End
   Begin MSComctlLib.Toolbar tbToolBar 
      Align           =   1  'Align Top
      Height          =   420
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   7935
      _ExtentX        =   13996
      _ExtentY        =   741
      ButtonWidth     =   609
      ButtonHeight    =   582
      Appearance      =   1
      ImageList       =   "imlToolbarIcons"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   15
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Back"
            Object.ToolTipText     =   "Back"
            ImageKey        =   "Back"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Forward"
            Object.ToolTipText     =   "Forward"
            ImageKey        =   "Forward"
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Cut"
            Object.ToolTipText     =   "Cut"
            ImageKey        =   "Cut"
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Copy"
            Object.ToolTipText     =   "Copy"
            ImageKey        =   "Copy"
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Paste"
            Object.ToolTipText     =   "Paste"
            ImageKey        =   "Paste"
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Delete"
            Object.ToolTipText     =   "Delete"
            ImageKey        =   "Delete"
         EndProperty
         BeginProperty Button9 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button10 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Properties"
            Object.ToolTipText     =   "Properties"
            ImageKey        =   "Properties"
         EndProperty
         BeginProperty Button11 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button12 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Large Icons"
            Object.ToolTipText     =   "View Large Icons"
            ImageKey        =   "View Large Icons"
            Style           =   2
         EndProperty
         BeginProperty Button13 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Small Icons"
            Object.ToolTipText     =   "View Small Icons"
            ImageKey        =   "View Small Icons"
            Style           =   2
         EndProperty
         BeginProperty Button14 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View List"
            Object.ToolTipText     =   "View List"
            ImageKey        =   "View List"
            Style           =   2
         EndProperty
         BeginProperty Button15 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Details"
            Object.ToolTipText     =   "View Details"
            ImageKey        =   "View Details"
            Style           =   2
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.StatusBar sbStatusBar 
      Align           =   2  'Align Bottom
      Height          =   270
      Left            =   0
      TabIndex        =   0
      Top             =   5505
      Width           =   7935
      _ExtentX        =   13996
      _ExtentY        =   476
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   8440
            Text            =   "Status"
            TextSave        =   "Status"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            AutoSize        =   2
            TextSave        =   "1/10/01"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            AutoSize        =   2
            TextSave        =   "20:28 PM"
         EndProperty
      EndProperty
   End
   Begin VB.Image imgSplitter 
      Height          =   4785
      Left            =   2280
      MousePointer    =   9  'Size W E
      Top             =   705
      Width           =   150
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileNew 
         Caption         =   "&New"
         Begin VB.Menu mnuFileNewCategory 
            Caption         =   "&Category..."
         End
         Begin VB.Menu mnuFileNewApplication 
            Caption         =   "&Application..."
         End
      End
      Begin VB.Menu mnuFileBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileCreateshortcut 
         Caption         =   "Create &Shortcut"
      End
      Begin VB.Menu mnuFileBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileClose 
         Caption         =   "&Close"
      End
   End
   Begin VB.Menu mnuView 
      Caption         =   "&View"
      Begin VB.Menu mnuViewToolbar 
         Caption         =   "&Toolbar"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuViewStatusBar 
         Caption         =   "Status &Bar"
         Checked         =   -1  'True
      End
      Begin VB.Menu mnuViewBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "Lar&ge Icons"
         Index           =   0
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "S&mall Icons"
         Index           =   1
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&List"
         Index           =   2
      End
      Begin VB.Menu mnuListViewMode 
         Caption         =   "&Details"
         Index           =   3
      End
      Begin VB.Menu mnuViewBar1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuViewArrangeIcons 
         Caption         =   "Arrange &Icons"
      End
      Begin VB.Menu mnuViewBar2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuViewRefresh 
         Caption         =   "&Refresh"
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpContents 
         Caption         =   "&Contents"
      End
      Begin VB.Menu mnuHelpSearchForHelpOn 
         Caption         =   "&Search For Help On..."
      End
      Begin VB.Menu mnuHelpBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "&About "
      End
   End
   Begin VB.Menu mnuTreeViewMenu 
      Caption         =   "tvTreeViewPopup"
      Visible         =   0   'False
      Begin VB.Menu mnuTreeViewNew 
         Caption         =   "New"
         Begin VB.Menu mnuTreeViewNewCategory 
            Caption         =   "&Category..."
         End
         Begin VB.Menu mnuTreeViewNewApplication 
            Caption         =   "&Application..."
         End
      End
      Begin VB.Menu mnuTreeViewRename 
         Caption         =   "Rena&me"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Const NAME_COLUMN = 0
Const TYPE_COLUMN = 1
Const SIZE_COLUMN = 2
Const DATE_COLUMN = 3
Private Declare Function OSWinHelp% Lib "user32" Alias "WinHelpA" (ByVal hWnd&, ByVal HelpFile$, ByVal wCommand%, dwData As Any)
  
Dim mbMoving As Boolean
Const sglSplitLimit = 500

Private Sub Form_Load()
  Me.Left = GetSetting(App.Title, "Settings", "MainLeft", 1000)
  Me.Top = GetSetting(App.Title, "Settings", "MainTop", 1000)
  Me.Width = GetSetting(App.Title, "Settings", "MainWidth", 6500)
  Me.Height = GetSetting(App.Title, "Settings", "MainHeight", 6500)
End Sub

Private Sub Form_Paint()
  lvListView.View = Val(GetSetting(App.Title, "Settings", "ViewMode", "0"))
  Select Case lvListView.View
    Case lvwIcon
      tbToolBar.Buttons(LISTVIEW_MODE0).Value = tbrPressed
    Case lvwSmallIcon
      tbToolBar.Buttons(LISTVIEW_MODE1).Value = tbrPressed
    Case lvwList
      tbToolBar.Buttons(LISTVIEW_MODE2).Value = tbrPressed
    Case lvwReport
      tbToolBar.Buttons(LISTVIEW_MODE3).Value = tbrPressed
  End Select
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Dim i As Integer

  'close all sub forms
  For i = Forms.Count - 1 To 1 Step -1
    Unload Forms(i)
  Next
  If Me.WindowState <> vbMinimized Then
    SaveSetting App.Title, "Settings", "MainLeft", Me.Left
    SaveSetting App.Title, "Settings", "MainTop", Me.Top
    SaveSetting App.Title, "Settings", "MainWidth", Me.Width
    SaveSetting App.Title, "Settings", "MainHeight", Me.Height
  End If
  SaveSetting App.Title, "Settings", "ViewMode", lvListView.View
End Sub


Private Sub Form_Resize()
  On Error Resume Next
  If Me.Width < 3000 Then Me.Width = 3000
  SizeControls imgSplitter.Left
End Sub

Private Sub imgSplitter_MouseDown(Button As Integer, Shift As Integer, x As Single, y As Single)
  With imgSplitter
    picSplitter.Move .Left, .Top, .Width \ 2, .Height - 20
  End With
  picSplitter.Visible = True
  mbMoving = True
End Sub

Private Sub imgSplitter_MouseMove(Button As Integer, Shift As Integer, x As Single, y As Single)
  Dim sglPos As Single
  
  If mbMoving Then
    sglPos = x + imgSplitter.Left
    If sglPos < sglSplitLimit Then
      picSplitter.Left = sglSplitLimit
    ElseIf sglPos > Me.Width - sglSplitLimit Then
      picSplitter.Left = Me.Width - sglSplitLimit
    Else
      picSplitter.Left = sglPos
    End If
  End If
End Sub

Private Sub imgSplitter_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
  SizeControls picSplitter.Left
  picSplitter.Visible = False
  mbMoving = False
End Sub

Private Sub mnuTreeViewRename_Click()
  tvTreeView.StartLabelEdit
End Sub

Private Sub tvTreeView_DragDrop(Source As Control, x As Single, y As Single)
  If Source = imgSplitter Then
    SizeControls x
  End If
End Sub

Sub SizeControls(x As Single)
  On Error Resume Next
  
  'set the width
  If x < 1500 Then x = 1500
  If x > (Me.Width - 1500) Then x = Me.Width - 1500
  tvTreeView.Width = x
  imgSplitter.Left = x
  lvListView.Left = x + 40
  lvListView.Width = Me.Width - (tvTreeView.Width + 140)
  lblTitle(0).Width = tvTreeView.Width
  lblTitle(1).Left = lvListView.Left + 20
  lblTitle(1).Width = lvListView.Width - 40

  'set the top
  
  If tbToolBar.Visible Then
    tvTreeView.Top = tbToolBar.Height + picTitles.Height
  Else
    tvTreeView.Top = picTitles.Height
  End If

  lvListView.Top = tvTreeView.Top
  
  'set the height
  If sbStatusBar.Visible Then
    tvTreeView.Height = Me.ScaleHeight - (picTitles.Top + picTitles.Height + sbStatusBar.Height)
  Else
    tvTreeView.Height = Me.ScaleHeight - (picTitles.Top + picTitles.Height)
  End If
  
  lvListView.Height = tvTreeView.Height
  imgSplitter.Top = tvTreeView.Top
  imgSplitter.Height = tvTreeView.Height
End Sub

Private Sub tbToolBar_ButtonClick(ByVal Button As MSComctlLib.Button)
  On Error Resume Next
  Select Case Button.Key
    Case "Back"
      'ToDo: Add 'Back' button code.
      MsgBox "Add 'Back' button code."
    Case "Forward"
      'ToDo: Add 'Forward' button code.
      MsgBox "Add 'Forward' button code."
    Case "Cut"
'      mnuEditCut_Click
    Case "Copy"
'      mnuEditCopy_Click
    Case "Paste"
'      mnuEditPaste_Click
    Case "Delete"
'      mnuFileDelete_Click
    Case "Properties"
'      mnuFileProperties_Click
    Case "View Large Icons"
      lvListView.View = lvwIcon
    Case "View Small Icons"
      lvListView.View = lvwSmallIcon
    Case "View List"
      lvListView.View = lvwList
    Case "View Details"
      lvListView.View = lvwReport
  End Select
End Sub

Private Sub mnuHelpAbout_Click()
  frmAbout.Show vbModal, Me
End Sub

Private Sub mnuHelpSearchForHelpOn_Click()
  Dim nRet As Integer

  'if there is no helpfile for this project display a message to the user
  'you can set the HelpFile for your application in the
  'Project Properties dialog
  If Len(App.HelpFile) = 0 Then
    MsgBox "Unable to display Help Contents. There is no Help associated with this project.", vbInformation, Me.Caption
  Else
    On Error Resume Next
    nRet = OSWinHelp(Me.hWnd, App.HelpFile, 261, 0)
    If Err Then
      MsgBox Err.Description
    End If
  End If

End Sub

Private Sub mnuHelpContents_Click()
  Dim nRet As Integer

  'if there is no helpfile for this project display a message to the user
  'you can set the HelpFile for your application in the
  'Project Properties dialog
  If Len(App.HelpFile) = 0 Then
    MsgBox "Unable to display Help Contents. There is no Help associated with this project.", vbInformation, Me.Caption
  Else
    On Error Resume Next
    nRet = OSWinHelp(Me.hWnd, App.HelpFile, 3, 0)
    If Err Then
      MsgBox Err.Description
    End If
  End If

End Sub


Private Sub mnuViewWebBrowser_Click()
  'ToDo: Add 'mnuViewWebBrowser_Click' code.
  MsgBox "Add 'mnuViewWebBrowser_Click' code."
End Sub

Private Sub mnuViewOptions_Click()
  'ToDo: Add 'mnuViewOptions_Click' code.
  MsgBox "Add 'mnuViewOptions_Click' code."
End Sub

Private Sub mnuViewRefresh_Click()
  'ToDo: Add 'mnuViewRefresh_Click' code.
  MsgBox "Add 'mnuViewRefresh_Click' code."
End Sub


Private Sub mnuVAIByDate_Click()
  'ToDo: Add 'mnuVAIByDate_Click' code.
'  lvListView.SortKey = DATE_COLUMN
End Sub

Private Sub mnuVAIByName_Click()
  'ToDo: Add 'mnuVAIByName_Click' code.
'  lvListView.SortKey = NAME_COLUMN
End Sub

Private Sub mnuVAIBySize_Click()
  'ToDo: Add 'mnuVAIBySize_Click' code.
'  lvListView.SortKey = SIZE_COLUMN
End Sub

Private Sub mnuVAIByType_Click()
  'ToDo: Add 'mnuVAIByType_Click' code.
'  lvListView.SortKey = TYPE_COLUMN
End Sub

Private Sub mnuViewStatusBar_Click()
  mnuViewStatusBar.Checked = Not mnuViewStatusBar.Checked
  sbStatusBar.Visible = mnuViewStatusBar.Checked
  SizeControls imgSplitter.Left
End Sub

Private Sub mnuViewToolbar_Click()
  mnuViewToolbar.Checked = Not mnuViewToolbar.Checked
  tbToolBar.Visible = mnuViewToolbar.Checked
  SizeControls imgSplitter.Left
End Sub

Private Sub mnuFileClose_Click()
  'unload the form
  Unload Me
End Sub

Private Sub mnuFileRename_Click()
  'ToDo: Add 'mnuFileRename_Click' code.
  MsgBox "Add 'mnuFileRename_Click' code."
End Sub

Private Sub mnuFileOpen_Click()
  Dim sFile As String

  With dlgCommonDialog
    .DialogTitle = "Open"
    .CancelError = False
    'ToDo: set the flags and attributes of the common dialog control
    .Filter = "All Files (*.*)|*.*"
    .ShowOpen
    If Len(.FileName) = 0 Then
      Exit Sub
    End If
    sFile = .FileName
  End With
  'ToDo: add code to process the opened file

End Sub

Private Sub tvTreeView_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
  If Button = 2 Then  ' Check if right mouse button was clicked.
    PopupMenu mnuTreeViewMenu, vbPopupMenuRightButton
  End If
End Sub

Private Sub tvTreeView_NodeClick(ByVal Node As MSComctlLib.Node)
  sbStatusBar.Panels.Item(1).Text = Node.FullPath
End Sub
