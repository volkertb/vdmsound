VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmMain 
   Caption         =   "VDMSound LaunchPad"
   ClientHeight    =   5775
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   10035
   Icon            =   "frmMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5775
   ScaleWidth      =   10035
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ImageList imlToolbar1 
      Left            =   5400
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16711935
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   12
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":08CA
            Key             =   "New File"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":09BA
            Key             =   "New Folder"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0AB6
            Key             =   "Cut"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0B9A
            Key             =   "Copy"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0C92
            Key             =   "Paste"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0E2A
            Key             =   "Delete"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":0F26
            Key             =   "Properties"
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1036
            Key             =   "View Large Icons"
         EndProperty
         BeginProperty ListImage9 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1132
            Key             =   "View Small Icons"
         EndProperty
         BeginProperty ListImage10 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":122E
            Key             =   "View List"
         EndProperty
         BeginProperty ListImage11 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":132A
            Key             =   "View Details"
         EndProperty
         BeginProperty ListImage12 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":142A
            Key             =   "Refresh"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ImageList imlListIcons32 
      Left            =   7200
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   32
      ImageHeight     =   32
      MaskColor       =   12632256
      UseMaskColor    =   0   'False
      _Version        =   393216
   End
   Begin MSComctlLib.ImageList imlListIcons16 
      Left            =   6600
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      UseMaskColor    =   0   'False
      _Version        =   393216
   End
   Begin MSComctlLib.ImageList imlTreeIcons 
      Left            =   6000
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   128
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   3
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1526
            Key             =   "Desktop"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1680
            Key             =   "Open Folder"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":17DA
            Key             =   "Closed Folder"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.ImageList imlToolbar0 
      Left            =   4800
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16711935
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   12
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1934
            Key             =   "New File"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1A24
            Key             =   "New Folder"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1B1C
            Key             =   "Cut"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1C00
            Key             =   "Copy"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1CF8
            Key             =   "Paste"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1DF8
            Key             =   "Delete"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1EE4
            Key             =   "Properties"
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":1FDC
            Key             =   "View Large Icons"
         EndProperty
         BeginProperty ListImage9 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":20D4
            Key             =   "View Small Icons"
         EndProperty
         BeginProperty ListImage10 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":21CC
            Key             =   "View List"
         EndProperty
         BeginProperty ListImage11 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":22C0
            Key             =   "View Details"
         EndProperty
         BeginProperty ListImage12 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmMain.frx":23BC
            Key             =   "Refresh"
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
      Tag             =   "CtlCategories"
      Top             =   705
      Width           =   2370
      _ExtentX        =   4180
      _ExtentY        =   8467
      _Version        =   393217
      Indentation     =   0
      PathSeparator   =   "."
      Sorted          =   -1  'True
      Style           =   7
      ImageList       =   "imlTreeIcons"
      Appearance      =   1
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      OLEDragMode     =   1
      OLEDropMode     =   1
   End
   Begin MSComctlLib.ListView lvListView 
      Height          =   4800
      Left            =   2415
      TabIndex        =   5
      Tag             =   "CtlApplications"
      Top             =   705
      Width           =   5610
      _ExtentX        =   9895
      _ExtentY        =   8467
      Sorted          =   -1  'True
      MultiSelect     =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      OLEDragMode     =   1
      AllowReorder    =   -1  'True
      FullRowSelect   =   -1  'True
      _Version        =   393217
      Icons           =   "imlListIcons32"
      SmallIcons      =   "imlListIcons16"
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      OLEDragMode     =   1
      NumItems        =   1
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Key             =   "name"
         Text            =   "Name"
         Object.Width           =   2540
      EndProperty
   End
   Begin VB.PictureBox picTitles 
      Align           =   1  'Align Top
      Appearance      =   0  'Flat
      BorderStyle     =   0  'None
      ForeColor       =   &H80000008&
      Height          =   300
      Left            =   0
      ScaleHeight     =   300
      ScaleWidth      =   10035
      TabIndex        =   2
      TabStop         =   0   'False
      Top             =   360
      Width           =   10035
      Begin VB.Label lblTitle 
         BorderStyle     =   1  'Fixed Single
         Caption         =   "DOS Applications"
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
      Height          =   360
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   10035
      _ExtentX        =   17701
      _ExtentY        =   635
      ButtonWidth     =   609
      ButtonHeight    =   582
      Appearance      =   1
      Style           =   1
      ImageList       =   "imlToolbar0"
      HotImageList    =   "imlToolbar1"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   16
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Object.Visible         =   0   'False
            Description     =   "Separator"
            Style           =   3
            Object.Width           =   1e-4
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "New"
            Description     =   "New"
            Object.ToolTipText     =   "New"
            ImageKey        =   "New File"
            Style           =   5
            BeginProperty ButtonMenus {66833FEC-8583-11D1-B16A-00C0F0283628} 
               NumButtonMenus  =   2
               BeginProperty ButtonMenu1 {66833FEE-8583-11D1-B16A-00C0F0283628} 
                  Key             =   "NewCategory"
                  Text            =   "New &Category"
               EndProperty
               BeginProperty ButtonMenu2 {66833FEE-8583-11D1-B16A-00C0F0283628} 
                  Key             =   "NewApplication"
                  Text            =   "New DOS &Application"
               EndProperty
            EndProperty
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Description     =   "Separator"
            Style           =   3
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Cut"
            Description     =   "Cut"
            Object.ToolTipText     =   "Cut"
            ImageKey        =   "Cut"
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Copy"
            Description     =   "Copy"
            Object.ToolTipText     =   "Copy"
            ImageKey        =   "Copy"
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Paste"
            Description     =   "Paste"
            Object.ToolTipText     =   "Paste"
            ImageKey        =   "Paste"
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Delete"
            Description     =   "Delete"
            Object.ToolTipText     =   "Delete"
            ImageKey        =   "Delete"
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Description     =   "Separator"
            Style           =   3
         EndProperty
         BeginProperty Button9 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Key             =   "Properties"
            Description     =   "Properties"
            Object.ToolTipText     =   "Properties"
            ImageKey        =   "Properties"
         EndProperty
         BeginProperty Button10 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Description     =   "Separator"
            Style           =   3
         EndProperty
         BeginProperty Button11 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "Refresh"
            Description     =   "Refresh"
            Object.ToolTipText     =   "Refresh"
            ImageKey        =   "Refresh"
         EndProperty
         BeginProperty Button12 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Enabled         =   0   'False
            Description     =   "Separator"
            Style           =   3
         EndProperty
         BeginProperty Button13 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Large Icons"
            Description     =   "View Large Icons"
            Object.ToolTipText     =   "View Large Icons"
            ImageKey        =   "View Large Icons"
            Style           =   2
         EndProperty
         BeginProperty Button14 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Small Icons"
            Description     =   "View Small Icons"
            Object.ToolTipText     =   "View Small Icons"
            ImageKey        =   "View Small Icons"
            Style           =   2
         EndProperty
         BeginProperty Button15 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View List"
            Description     =   "View List"
            Object.ToolTipText     =   "View List"
            ImageKey        =   "View List"
            Style           =   2
         EndProperty
         BeginProperty Button16 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "View Details"
            Description     =   "View Details"
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
      Width           =   10035
      _ExtentX        =   17701
      _ExtentY        =   476
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   3
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   12144
            Text            =   "Status"
            TextSave        =   "Status"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            AutoSize        =   2
            TextSave        =   "8/10/01"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            AutoSize        =   2
            TextSave        =   "01:08 AM"
         EndProperty
      EndProperty
   End
   Begin VB.Label Label1 
      Caption         =   "Label1"
      Height          =   255
      Left            =   8520
      TabIndex        =   8
      Top             =   2880
      Width           =   1095
   End
   Begin VB.Image picMSDOS32 
      Height          =   480
      Left            =   8280
      Picture         =   "frmMain.frx":24BC
      Top             =   2040
      Width           =   480
   End
   Begin VB.Image picMSDOS16 
      Height          =   240
      Left            =   8280
      Picture         =   "frmMain.frx":27C6
      Top             =   1680
      Width           =   240
   End
   Begin VB.Image imgSplitter 
      Height          =   4785
      Left            =   2280
      MouseIcon       =   "frmMain.frx":2D50
      MousePointer    =   99  'Custom
      Top             =   705
      Width           =   150
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileNew 
         Caption         =   "Ne&w"
         Begin VB.Menu mnuFileNewCategory 
            Caption         =   "&Category"
         End
         Begin VB.Menu mnuFileNewApplication 
            Caption         =   "DOS &Application"
            Shortcut        =   ^N
         End
      End
      Begin VB.Menu mnuFileBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileCreateshortcut 
         Caption         =   "Create &Shortcut"
         Enabled         =   0   'False
      End
      Begin VB.Menu mnuFileRename 
         Caption         =   "Rena&me"
      End
      Begin VB.Menu mnuFileDelete 
         Caption         =   "&Delete"
         Shortcut        =   {DEL}
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
         Shortcut        =   {F5}
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
      Begin VB.Menu mnuTreeViewExpandAll 
         Caption         =   "E&xpand All"
      End
      Begin VB.Menu mnuTreeViewCollapseAll 
         Caption         =   "&Collapse All"
      End
      Begin VB.Menu mnuTreeViewMenuBar0 
         Caption         =   "-"
      End
      Begin VB.Menu mnuTreeViewNew 
         Caption         =   "Ne&w"
         Begin VB.Menu mnuTreeViewNewCategory 
            Caption         =   "&Category"
         End
         Begin VB.Menu mnuTreeViewNewApplication 
            Caption         =   "DOS &Application"
         End
      End
      Begin VB.Menu mnuTreeViewRename 
         Caption         =   "Rena&me"
      End
      Begin VB.Menu mnuTreeViewDelete 
         Caption         =   "&Delete"
      End
   End
   Begin VB.Menu mnuListViewMenu 
      Caption         =   "lvListViewPopup"
      Visible         =   0   'False
      Begin VB.Menu mnuListViewNewApplication 
         Caption         =   "New DOS &Application"
      End
      Begin VB.Menu mnuListViewRename 
         Caption         =   "Rena&me"
      End
      Begin VB.Menu mnuListViewDelete 
         Caption         =   "&Delete"
      End
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Last known primary selection (when the applications pane
'  was last synchronized)
'
Private lastNode As Node

'============================================================================
'
' Wizard-generated code
'
'============================================================================

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
  SelectListingMode Val(GetSetting(App.Title, "Settings", "ViewMode", "0"))
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Dim i As Integer

  'close all sub forms
  For i = Forms.Count - 1 To 1 Step -1
    Unload Forms(i)
  Next
  If Me.WindowState = vbNormal Then
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
    Case "New"
'     mnuFileNewApplication_Click
    Case "Delete"
      mnuFileDelete_Click
    Case "Properties"
'     mnuFileProperties_Click
    Case "Refresh"
      mnuViewRefresh_Click
    Case "Cut"
'     mnuEditCut_Click
    Case "Copy"
'     mnuEditCopy_Click
    Case "Paste"
'     mnuEditPaste_Click
    Case "View Large Icons"
      SelectListingMode lvwIcon
    Case "View Small Icons"
      SelectListingMode lvwSmallIcon
    Case "View List"
      SelectListingMode lvwList
    Case "View Details"
      SelectListingMode lvwReport
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

'============================================================================
'
' Custom code
'
'============================================================================

'----------------------------------------------------------------------------
' UTILITY PROCEDURES
'----------------------------------------------------------------------------

'
' Gets the tree view node to which context-menu commands should apply
'
Public Function GetContextNode( _
) As Node
  If tvTreeView.DropHighlight Is Nothing Then
    Set GetContextNode = tvTreeView.SelectedItem
  Else
    Set GetContextNode = tvTreeView.DropHighlight
  End If
End Function

'
' Gets the list view items to which context-menu commands should apply
'
Public Function GetContextItems( _
  Optional isSingleSelection As Boolean = True _
) As Collection
  Dim collRetVal As New Collection

  If isSingleSelection Then
    collRetVal.Add lvListView.SelectedItem
  Else
    Dim lvThisItem As ListItem
    For Each lvThisItem In lvListView.ListItems
      If lvThisItem.Selected Then collRetVal.Add lvThisItem
    Next
  End If

  Set GetContextItems = collRetVal
End Function

'
' Changes the primary selection in the category tree view
'
Public Sub SelectCategory( _
  tvNode As Node _
)
  ' Update the tree control state
  Set tvTreeView.SelectedItem = tvNode
  Set tvTreeView.DropHighlight = Nothing

  ' Don't update the applicatiosn list view (costly) if the
  '  main selection hasn't changed
  If lastNode Is tvNode Then Exit Sub
  Set lastNode = tvNode

  tvNode.EnsureVisible
  modConfig.LoadApplications lvListView, tvNode.Key
  modConfig.SetLastSelectedCategory tvTreeView
End Sub

'
' Changes the way the applications list view is displayed
'
Public Sub SelectListingMode( _
  lngMode As Long _
)
  lvListView.View = lngMode

  If lngMode = lvwIcon Then
    tbToolBar.Buttons(LISTVIEW_MODE0).Value = tbrPressed
    mnuListViewMode.Item(0).Checked = True
  Else
    mnuListViewMode.Item(0).Checked = False
  End If

  If lngMode = lvwSmallIcon Then
    tbToolBar.Buttons(LISTVIEW_MODE1).Value = tbrPressed
    mnuListViewMode.Item(1).Checked = True
  Else
    mnuListViewMode.Item(1).Checked = False
  End If

  If lngMode = lvwList Then
    tbToolBar.Buttons(LISTVIEW_MODE2).Value = tbrPressed
    mnuListViewMode.Item(2).Checked = True
  Else
    mnuListViewMode.Item(2).Checked = False
  End If

  If lngMode = lvwReport Then
    tbToolBar.Buttons(LISTVIEW_MODE3).Value = tbrPressed
    mnuListViewMode.Item(3).Checked = True
  Else
    mnuListViewMode.Item(3).Checked = False
  End If
End Sub

'----------------------------------------------------------------------------
' UTILITY PROCEDURES (2)
'----------------------------------------------------------------------------

'
' Pops up the tree view context menu
'
Public Sub PopUpCategoriesContextMenu( _
  x As Single, _
  y As Single _
)
  Dim tvContextNode As Node
  Set tvContextNode = GetContextNode()

  If tvContextNode Is Nothing Then Exit Sub

  ' Update context menu based on the node on which any context-menu
  '  operations will be performed
  If tvContextNode.Parent Is Nothing Then
    mnuTreeViewDelete.Enabled = False ' don't delete the root node
  Else
    mnuTreeViewDelete.Enabled = True
  End If

  If tvContextNode.Children > 0 Then
    mnuTreeViewExpandAll.Enabled = True
    mnuTreeViewCollapseAll.Enabled = True
  Else
    mnuTreeViewExpandAll.Enabled = False
    mnuTreeViewCollapseAll.Enabled = False
  End If

  PopupMenu mnuTreeViewMenu, vbPopupMenuRightButton, x, y
End Sub

'
' Pops up the list view context menu
'
Public Sub PopUpApplicationsContextMenu( _
  x As Single, _
  y As Single _
)
  Dim lvContextItems As Collection
  Set lvContextItems = GetContextItems(False)

  If lvContextItems.Count = 1 Then
    mnuListViewRename.Enabled = True
  Else
    mnuListViewRename.Enabled = False
  End If

  If lvContextItems.Count > 0 Then
    mnuListViewDelete.Enabled = True
  Else
    mnuListViewDelete.Enabled = False
  End If

  PopupMenu mnuListViewMenu, vbPopupMenuRightButton, x, y
End Sub

'----------------------------------------------------------------------------
' MAIN MENU
'----------------------------------------------------------------------------

'
' New Application command.  Creates a new application
'  placeholder in the currently selected category
'
Private Sub mnuFileNewApplication_Click()
  On Error Resume Next

  ' Node under which to create the category
  Dim tvContextNode As Node
  Set tvContextNode = GetContextNode()

  ' New application (default) name
  Dim strText As String
  strText = "New Application"

  ' Discriminator, in case the default name is already taken
  Dim appIndex As Integer
  appIndex = 2

  ' Look for a name that does not already exist
  Do Until modListUtil.GetItemByText(lvListView, strText) Is Nothing
    strText = "New Application (" & appIndex & ")"
    appIndex = appIndex + 1
  Loop

  ' Add the application
  If tvContextNode <> tvTreeView.SelectedItem Then
    modConfig.AddApplication Nothing, tvContextNode.Key, strText
  Else
    Dim lvItem As ListItem
    Set lvItem = modConfig.AddApplication(lvListView, tvContextNode.Key, strText)

    ' If the application was added successfully, select it and prompt the
    '  user for a new name (start editing on its label)
    If Not lvItem Is Nothing Then
      lvListView.SelectedItem = lvItem
      lvListView.StartLabelEdit
    End If
  End If
End Sub

'
' New Category command.  Creates a new category
'  under the currently selected category
'
Private Sub mnuFileNewCategory_Click()
  On Error Resume Next

  ' Node under which to create the category
  Dim tvContextNode As Node
  Set tvContextNode = GetContextNode()

  ' New category (default) name
  Dim strText As String
  strText = "New Category"

  ' Discriminator, in case the default name is already taken
  Dim catIndex As Integer
  catIndex = 2

  ' Look for a name that does not already exist
  Do Until modTreeUtil.GetNodeByText(tvTreeView, tvContextNode, strText) Is Nothing
    strText = "New Category (" & catIndex & ")"
    catIndex = catIndex + 1
  Loop

  ' Add the category
  Dim tvNode As Node
  Set tvNode = modConfig.AddCategory(tvTreeView, tvContextNode, strText)

  ' If the category was added successfully, select it and prompt the
  '  user for a new name (start editing on its label)
  If Not tvNode Is Nothing Then
    SelectCategory tvNode
    tvTreeView.StartLabelEdit
  End If
End Sub

'
' Rename command. Performs a rename on either the
'  tree view or list view depending on which one has the focus
'
Private Sub mnuFileRename_Click()
  On Error Resume Next

  ' See if the categories or applications panes are active
  Select Case ActiveControl.Tag
     Case "CtlCategories"
      mnuTreeViewRename_Click
    Case "CtlApplications"
      mnuListViewRename_Click
  End Select
End Sub

'
' Delete command from the File menu. Performs a delete on either the
'  tree view or list view depending on which one has the focus
'
Private Sub mnuFileDelete_Click()
  On Error Resume Next

  ' See if the categories or applications panes are active
  Select Case ActiveControl.Tag
    Case "CtlCategories"
      mnuTreeViewDelete_Click
    Case "CtlApplications"
      mnuListViewDelete_Click
  End Select
End Sub

'
' Arrange the icons in the applications list view
'
Private Sub mnuViewArrangeIcons_Click()
  On Error Resume Next

  ' Force the view to rearrange
  lvListView.Arrange = lvwAutoTop
  ' Switch back, allowing the icons to be moved again
  lvListView.Arrange = lvwNone
End Sub

'
' Refresh the categories list in the categories tree view by
'  resynchronizing with the INI file
'
Private Sub mnuViewRefresh_Click()
  On Error Resume Next

  modConfig.LoadCategories tvTreeView
  SelectCategory modConfig.GetLastSelectedCategory(tvTreeView)
End Sub

'
' Change the way applications are displayed in the list view
'
Private Sub mnuListViewMode_Click(Index As Integer)
  On Error Resume Next

  Select Case Index
    Case 0
      SelectListingMode lvwIcon
    Case 1
      SelectListingMode lvwSmallIcon
    Case 2
      SelectListingMode lvwList
    Case 3
      SelectListingMode lvwReport
  End Select
End Sub

'----------------------------------------------------------------------------
' CATEGORIES TREE VIEW
'----------------------------------------------------------------------------

'
' Pops up a context menu on a category (on the tree view)
'
Private Sub tvTreeView_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
  On Error Resume Next

  If Button = 2 Then  ' Check if right mouse button was clicked.
    ' Pop up the menu
    PopUpCategoriesContextMenu tvTreeView.Left + x, tvTreeView.Top + y
  End If
End Sub

'
' Pops up a context menu on a category (on the tree view)
'
Private Sub tvTreeView_KeyUp(KeyCode As Integer, Shift As Integer)
  On Error Resume Next

  If KeyCode = 93 Then  ' check for 'context-menu' key
    ' Pop up the menu
    PopUpCategoriesContextMenu tvTreeView.Left, tvTreeView.Top
  End If
End Sub

'
' Synchronizes the applications pane (list view) with the
'  currently selected category
'
Private Sub tvTreeView_NodeClick(ByVal Node As MSComctlLib.Node)
  On Error Resume Next

  sbStatusBar.Panels.Item(1).Text = Node.Text
  SelectCategory Node
End Sub

'
' Validates a category rename (in the tree view)
'
Private Sub tvTreeView_AfterLabelEdit(Cancel As Integer, NewString As String)
  On Error Resume Next

  If Len(NewString) > 0 Then
    Select Case modConfig.RenameCategory(tvTreeView, GetContextNode(), NewString)
      Case 0
        Cancel = 0
      Case 1
        MsgBox "The category '" & NewString & "' already exists." & Chr$(13) & "Please choose another name."
        Cancel = 1
      Case Else
        Cancel = 1
    End Select
  Else
    MsgBox "Please type in a valid name."
    Cancel = 1
  End If
End Sub

'
' Make sure the "Root" node is always expanded
'
Private Sub tvTreeView_Collapse(ByVal Node As MSComctlLib.Node)
  On Error Resume Next

  If Node.Parent Is Nothing Then
    Node.Expanded = True
  End If
End Sub

'
'
'
Private Sub tvTreeView_OLEDragOver(Data As MSComctlLib.DataObject, Effect As Long, Button As Integer, Shift As Integer, x As Single, y As Single, State As Integer)
  On Error Resume Next

  Select Case State
    Case vbEnter
    Case vbOver
      Dim tvNode As Node
      Set tvNode = tvTreeView.HitTest(x, y)
      tvTreeView.DropHighlight = tvNode
    Case vbLeave
      tvTreeView.DropHighlight = Nothing
  End Select
End Sub

'----------------------------------------------------------------------------
' CATEGORIES TREE VIEW CONTEXT MENU
'----------------------------------------------------------------------------

'
' Expand all nodes
'
Private Sub mnuTreeViewExpandAll_Click()
  On Error Resume Next
  ExpandCollapseAllNodes GetContextNode(), True
End Sub

'
' Collapse all nodes
'
Private Sub mnuTreeViewCollapseAll_Click()
  On Error Resume Next
  ExpandCollapseAllNodes GetContextNode(), False
End Sub

'
' New Application command.  Creates a new application
'  placeholder in the currently selected category
'
Private Sub mnuTreeViewNewApplication_Click()
  mnuFileNewApplication_Click
End Sub

'
' New Category command.  Creates a new category
'  under the currently selected category
'
Private Sub mnuTreeViewNewCategory_Click()
  mnuFileNewCategory_Click
End Sub

'
' Rename command. Initiates a category rename (on the tree view)
'
Private Sub mnuTreeViewRename_Click()
  On Error Resume Next

  ' First make the context-menu selection into our primary selection
  SelectCategory GetContextNode()
  ' Edit the primary selection
  tvTreeView.StartLabelEdit
End Sub

'
' Delete command. Recursively delete a category and applications under it
'
Private Sub mnuTreeViewDelete_Click()
  On Error Resume Next

  ' Node to delete
  Dim tvContextNode As Node
  Set tvContextNode = GetContextNode()

  If tvContextNode.Parent Is Nothing Then Exit Sub

  If MsgBox("Are you sure you want to remove the category '" & tvContextNode.Text & "' and all its contents?", vbYesNo + vbDefaultButton1 + vbExclamation, "Delete '" & tvContextNode.Text & "'") = vbYes Then
    Dim tvParentNode As Node
    If tvContextNode.Parent Is Nothing Then
      Set tvParentNode = tvContextNode.Next
    Else
      Set tvParentNode = tvContextNode.Parent
    End If

    modConfig.DeleteCategory tvTreeView, tvContextNode

    SelectCategory tvParentNode
  End If
End Sub

'----------------------------------------------------------------------------
' APPLICATIONS LIST VIEW
'----------------------------------------------------------------------------

'
' Pops up a context menu on an application (on the list view)
'
Private Sub lvListView_MouseUp(Button As Integer, Shift As Integer, x As Single, y As Single)
  On Error Resume Next

  If Button = 2 Then  ' Check if right mouse button was clicked.
    ' Pop up the menu
    PopUpApplicationsContextMenu lvListView.Left + x, lvListView.Top + y
  End If
End Sub

'
' Pops up a context menu on an application (on the list view)
'
Private Sub lvListView_KeyUp(KeyCode As Integer, Shift As Integer)
  On Error Resume Next

  If KeyCode = 93 Then  ' check for 'context-menu' key
    ' Pop up the menu
    PopUpCategoriesContextMenu lvListView.Left, lvListView.Top
  End If
End Sub

Private Sub lvListView_OLEStartDrag(Data As MSComctlLib.DataObject, AllowedEffects As Long)
  On Error Resume Next

' TODO: implement
End Sub

'
' Validates an application rename (in the list view)
'
Private Sub lvListView_AfterLabelEdit(Cancel As Integer, NewString As String)
  On Error Resume Next

  If Len(NewString) > 0 Then
    Select Case modConfig.RenameApplication(lvListView, lvListView.SelectedItem, NewString)
      Case 0
        Cancel = 0
      Case 1
        MsgBox "The application '" & NewString & "' already exists." & Chr$(13) & "Please choose another name."
        Cancel = 1
      Case Else
        Cancel = 1
    End Select
  Else
    MsgBox "Please type in a valid name."
    Cancel = 1
  End If
End Sub

'----------------------------------------------------------------------------
' APPLICATIONS LIST VIEW CONTEXT MENU
'----------------------------------------------------------------------------

'
' New Application command.  Creates a new application
'  placeholder in the currently selected category
'
Private Sub mnuListViewNewApplication_Click()
  mnuFileNewApplication_Click
End Sub

'
' Rename command. Initiates an application rename (on the list view)
'
Private Sub mnuListViewRename_Click()
  On Error Resume Next

  ' Edit the primary selection
  lvListView.StartLabelEdit
End Sub

'
' Delete command. Recursively delete a category and applications under it
'
Private Sub mnuListViewDelete_Click()
  On Error Resume Next

  ' Items to delete
  Dim lvContextItems As Collection
  Set lvContextItems = GetContextItems(False)

  If lvContextItems.Count <= 0 Then Exit Sub

  Dim strPrompt As String

  If lvContextItems.Count = 1 Then
    strPrompt = "'" & lvContextItems(1).Text & "'"
  Else
    strPrompt = lvContextItems.Count & " items"
  End If

  If MsgBox("Are you sure you want to delete " & strPrompt & "?", vbYesNo + vbDefaultButton1 + vbExclamation, "Delete " & strPrompt) = vbYes Then
    Dim lvItem As ListItem
    For Each lvItem In lvContextItems
      modConfig.DeleteApplication lvListView, lvItem, tvTreeView.SelectedItem.Key
    Next
  End If
End Sub
