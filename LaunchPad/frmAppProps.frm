VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmAppProps 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Program Properties"
   ClientHeight    =   5280
   ClientLeft      =   255
   ClientTop       =   1500
   ClientWidth     =   5550
   Icon            =   "frmAppProps.frx":0000
   KeyPreview      =   -1  'True
   LinkTopic       =   "Form1"
   LockControls    =   -1  'True
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5280
   ScaleWidth      =   5550
   ShowInTaskbar   =   0   'False
   WhatsThisButton =   -1  'True
   WhatsThisHelp   =   -1  'True
   Begin MSComctlLib.ImageList imlIcons 
      Left            =   720
      Top             =   4680
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   12632256
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   1
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":000C
            Key             =   "default"
         EndProperty
      EndProperty
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   0
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   1
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
      Begin VB.Frame fraExecutables 
         Caption         =   "&Executable(s)"
         Height          =   1755
         Left            =   60
         TabIndex        =   10
         Top             =   1920
         Width           =   4935
         Begin VB.CommandButton butExecutable 
            Height          =   315
            Left            =   4380
            MaskColor       =   &H00FF00FF&
            Picture         =   "frmAppProps.frx":05A6
            Style           =   1  'Graphical
            TabIndex        =   16
            ToolTipText     =   "Browse for file..."
            Top             =   720
            UseMaskColor    =   -1  'True
            Width           =   375
         End
         Begin VB.CommandButton butExecutablesTest 
            Caption         =   "&Test executable"
            Enabled         =   0   'False
            Height          =   345
            Left            =   1800
            TabIndex        =   17
            Top             =   1230
            Width           =   1695
         End
         Begin VB.TextBox edtDirectory 
            Height          =   315
            Left            =   1800
            TabIndex        =   12
            Top             =   300
            Width           =   2475
         End
         Begin VB.CommandButton butDirectory 
            Height          =   315
            Left            =   4380
            MaskColor       =   &H00FF00FF&
            Picture         =   "frmAppProps.frx":0692
            Style           =   1  'Graphical
            TabIndex        =   13
            ToolTipText     =   "Browse for folder..."
            Top             =   300
            UseMaskColor    =   -1  'True
            Width           =   375
         End
         Begin VB.ComboBox lstExecutable 
            Height          =   315
            Left            =   1800
            Sorted          =   -1  'True
            TabIndex        =   15
            Top             =   720
            Width           =   2475
         End
         Begin VB.CommandButton butExecutablesMore 
            Caption         =   "Mo&re..."
            Enabled         =   0   'False
            Height          =   345
            Left            =   3660
            TabIndex        =   18
            Top             =   1230
            Width           =   1095
         End
         Begin VB.Label lblDirectory 
            Alignment       =   1  'Right Justify
            Caption         =   "Program &directory:"
            Height          =   255
            Left            =   120
            TabIndex        =   11
            Top             =   360
            Width           =   1575
         End
         Begin VB.Label lblExecutable 
            Alignment       =   1  'Right Justify
            Caption         =   "Main e&xecutable:"
            Height          =   255
            Left            =   120
            TabIndex        =   14
            Top             =   780
            Width           =   1575
         End
      End
      Begin VB.Frame fraGeneral 
         Caption         =   "&General"
         Height          =   1695
         Left            =   60
         TabIndex        =   2
         Top             =   120
         Width           =   4935
         Begin VB.CommandButton butIcon 
            Height          =   330
            Left            =   4380
            MaskColor       =   &H00FF00FF&
            Picture         =   "frmAppProps.frx":077E
            Style           =   1  'Graphical
            TabIndex        =   9
            ToolTipText     =   "Browse for file..."
            Top             =   1140
            UseMaskColor    =   -1  'True
            Width           =   375
         End
         Begin VB.TextBox edtCategory 
            BackColor       =   &H8000000F&
            Height          =   315
            Left            =   1800
            Locked          =   -1  'True
            TabIndex        =   4
            Top             =   300
            Width           =   2955
         End
         Begin VB.TextBox edtName 
            Height          =   315
            Left            =   1800
            TabIndex        =   6
            Top             =   720
            Width           =   2955
         End
         Begin MSComctlLib.ImageCombo lstIcon 
            Height          =   330
            Left            =   1800
            TabIndex        =   8
            Top             =   1140
            Width           =   2475
            _ExtentX        =   4366
            _ExtentY        =   582
            _Version        =   393216
            ForeColor       =   -2147483640
            BackColor       =   -2147483643
            ImageList       =   "imlIcons"
         End
         Begin VB.Label lblIcon 
            Alignment       =   1  'Right Justify
            Caption         =   "Program &icon:"
            Height          =   255
            Left            =   120
            TabIndex        =   7
            Top             =   1200
            Width           =   1575
         End
         Begin VB.Label lblCategory 
            Alignment       =   1  'Right Justify
            Caption         =   "Category:"
            Height          =   255
            Left            =   840
            TabIndex        =   3
            Top             =   360
            Width           =   855
         End
         Begin VB.Label lblName 
            Alignment       =   1  'Right Justify
            Caption         =   "&Name:"
            Height          =   255
            Left            =   840
            TabIndex        =   5
            Top             =   780
            Width           =   855
         End
         Begin VB.Image picProgramIcon 
            Height          =   480
            Left            =   240
            Top             =   420
            Width           =   480
         End
      End
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   6
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   24
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   5
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   23
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   4
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   22
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   3
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   21
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   2
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   20
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   1
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   19
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
   End
   Begin MSComctlLib.ImageList imlTabs 
      Left            =   120
      Top             =   4680
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483633
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   16711935
      UseMaskColor    =   0   'False
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   7
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":086A
            Key             =   "Program"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":09C4
            Key             =   "Memory"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0B1E
            Key             =   "Sound"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0C78
            Key             =   "Music"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0DD2
            Key             =   "Joystick"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0F2C
            Key             =   "Video"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":1086
            Key             =   "Troubleshooting"
         EndProperty
      EndProperty
   End
   Begin VB.CommandButton cmdApply 
      Caption         =   "&Apply"
      Height          =   345
      Left            =   4320
      TabIndex        =   27
      Top             =   4800
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   345
      Left            =   3120
      TabIndex        =   26
      Top             =   4800
      Width           =   1095
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   345
      Left            =   1920
      TabIndex        =   25
      Top             =   4800
      Width           =   1095
   End
   Begin MSComctlLib.TabStrip tbsOptions 
      Height          =   4545
      Left            =   105
      TabIndex        =   0
      Top             =   120
      Width           =   5295
      _ExtentX        =   9340
      _ExtentY        =   8017
      MultiRow        =   -1  'True
      TabMinWidth     =   1270
      ImageList       =   "imlTabs"
      _Version        =   393216
      BeginProperty Tabs {1EFB6598-857C-11D1-B16A-00C0F0283628} 
         NumTabs         =   7
         BeginProperty Tab1 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Program"
            Key             =   "Program"
            Object.ToolTipText     =   "General program options"
            ImageVarType    =   8
            ImageKey        =   "Program"
         EndProperty
         BeginProperty Tab2 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Memor&y"
            Key             =   "Memory"
            Object.ToolTipText     =   "Memory options"
            ImageVarType    =   8
            ImageKey        =   "Memory"
         EndProperty
         BeginProperty Tab3 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Sound"
            Key             =   "Sound"
            Object.ToolTipText     =   "Digital sound options"
            ImageVarType    =   8
            ImageKey        =   "Sound"
         EndProperty
         BeginProperty Tab4 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Music"
            Key             =   "Music"
            Object.ToolTipText     =   "FM and MIDI music options"
            ImageVarType    =   8
            ImageKey        =   "Music"
         EndProperty
         BeginProperty Tab5 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Joystick"
            Key             =   "Joystick"
            Object.ToolTipText     =   "Joystick options"
            ImageVarType    =   8
            ImageKey        =   "Joystick"
         EndProperty
         BeginProperty Tab6 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Video"
            Key             =   "Video"
            Object.ToolTipText     =   "Video options"
            ImageVarType    =   8
            ImageKey        =   "Video"
         EndProperty
         BeginProperty Tab7 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "&Troubleshooting"
            Key             =   "Troubleshooting"
            Object.ToolTipText     =   "Troubleshooting tips"
            ImageVarType    =   8
            ImageKey        =   "Troubleshooting"
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
End
Attribute VB_Name = "frmAppProps"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'
' Private state variables
'
Private strLastDirectory As String
Private idxLastIcon As Integer

'============================================================================
'
' Wizard-generated code
'
'============================================================================

Option Explicit

Private Sub cmdApply_Click()
    MsgBox "Place code here to set options w/o closing dialog!"
End Sub

Private Sub cmdCancel_Click()
    Unload Me
End Sub

Private Sub cmdOK_Click()
    MsgBox "Place code here to set options and close dialog!"
    Unload Me
End Sub

Private Sub Form_KeyDown(KeyCode As Integer, Shift As Integer)
    Dim i As Integer
    'handle ctrl+tab to move to the next tab
    If Shift = vbCtrlMask And KeyCode = vbKeyTab Then
        i = tbsOptions.SelectedItem.Index
        If i = tbsOptions.Tabs.Count Then
            'last tab so we need to wrap to tab 1
            Set tbsOptions.SelectedItem = tbsOptions.Tabs(1)
        Else
            'increment the tab
            Set tbsOptions.SelectedItem = tbsOptions.Tabs(i + 1)
        End If
    End If
End Sub

Private Sub tbsOptions_Click()
    
    Dim i As Integer
    'show and enable the selected tab's controls
    'and hide and disable all others
    For i = 0 To tbsOptions.Tabs.Count - 1
        If i = tbsOptions.SelectedItem.Index - 1 Then
            picOptions(i).Left = 210
            picOptions(i).Enabled = True
        Else
            picOptions(i).Left = -20000
            picOptions(i).Enabled = False
        End If
    Next
    
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
' Splits a path into a directory and a file name around the last '\' present
'  in the path (if any).  If no '\' can be found, the path is assumed to be
'  a file name, and the directory name is empty
'
Private Sub splitPath( _
  ByVal strPath As String, _
  ByRef strDirectory As String, _
  ByRef strFile As String _
)

  Dim splitPos As Integer
  splitPos = InStrRev(strPath, "\")

  strFile = Mid(strPath, splitPos + 1)
  strDirectory = Left$(strPath, splitPos)
End Sub

'
' Checks whether the given path is an absolute UNC or DOS path,
'  or whether it looks like a relative path
'
Private Function isAbsolutePath( _
  ByVal strPath As String _
) As Boolean

  If Mid$(strPath, 1, 2) = "\\" Then
    isAbsolutePath = True   ' UNC absolute path
  ElseIf Mid$(strPath, 2, 1) = ":" Then
    isAbsolutePath = True   ' DOS absolute path
  Else
    isAbsolutePath = False  ' looks like a relative path
  End If
End Function


'
' Given an absolute path, this function checks it against the given
'  base directory and sees if a relative path can be found
'
Private Function makeRelativePath( _
  ByVal strPath As String, _
  ByVal strBaseDir As String _
) As String

  ' Check whether whether the given base directory exists at all,
  '  whether the given path is non-empty, and whether the given path
  '  is already relative
  If Not isAbsolutePath(strBaseDir) Or _
     Len(strPath) < 1 Or _
     Not isAbsolutePath(strPath) _
  Then
    makeRelativePath = strPath
    Exit Function
  End If

  ' Make sure directory ends with a trailing '\'
  If Right$(strBaseDir, 1) <> "\" Then strBaseDir = strBaseDir & "\"

  ' Check if base directoy is a parent in the path
  If LCase$(strBaseDir) = LCase$(Left$(strPath, Len(strBaseDir))) Then
    makeRelativePath = Mid$(strPath, Len(strBaseDir) + 1)
  Else
    makeRelativePath = strPath
  End If
End Function

'
' Given a relative path, this function constructs an absolute
'  path
'
Private Function makeAbsoultePath( _
  ByVal strPath As String, _
  ByVal strBaseDir As String _
) As String

  ' Check whether whether the given base directory exists at all,
  '  whether the given path is non-empty, and whether the given path
  '  is already absoulte
  If Not isAbsolutePath(strBaseDir) Or _
     Len(strPath) < 1 Or _
     isAbsolutePath(strPath) _
  Then
    makeAbsoultePath = strPath
    Exit Function
  End If

  ' Make sure directory ends with a trailing '\'
  If Right$(strBaseDir, 1) <> "\" Then strBaseDir = strBaseDir & "\"

  ' Check if the path is semi-absolute (starts from the root of the directory
  '  hierarchy but is relative to a drive or server)
  If Left$(strPath, 1) = "\" Then
    makeAbsoultePath = Mid$(strBaseDir, InStr(3, strBaseDir, "\")) & Mid$(strPath, 2)
  Else
    makeAbsoultePath = strBaseDir & strPath
  End If
End Function

'----------------------------------------------------------------------------
' UTILITY PROCEDURES (2)
'----------------------------------------------------------------------------

'
' Called when the text in the 'directory' edit box changes
'
Private Sub onDirectoryChange( _
)

  Dim strNewDirectory As String, strNewExecutable As String, strNewIcon As String
  strNewDirectory = edtDirectory.Text

  ' Ensure we have a trailing '\' (simplifies new vs. old folder comparisons)
  If Right$(edtDirectory.Text, 1) <> "\" Then strNewDirectory = strNewDirectory & "\"

  ' Look at the executable currently in the "Executable" box and make it relative
  '  to the new path in the 'directory' edit box
  strNewExecutable = makeRelativePath(makeAbsoultePath(lstExecutable.Text, strLastDirectory), strNewDirectory)

  ' Look at the icon currently in the "Icons" box and make it relative
  '  to the new path in the 'directory' edit box
  strNewIcon = makeRelativePath(makeAbsoultePath(lstIcon.Text, strLastDirectory), strNewDirectory)

  ' If the directory changed, rescan the quick-list of executables
  If LCase$(strNewDirectory) <> LCase$(strLastDirectory) Then
    Dim strFile As String

    On Error Resume Next

    ' Rebuild the executables and icons list
    strFile = FileSystem.Dir(strNewDirectory & "*.???", vbNormal + vbReadOnly + vbHidden + vbSystem)

    Dim iconsImageList As ImageList

    lstIcon.ComboItems.Clear
    lstIcon.Text = strNewIcon

    Set iconsImageList = lstIcon.ImageList
    Set lstIcon.ImageList = Nothing
    iconsImageList.ListImages.Clear
    iconsImageList.ListImages.Add , "", modMain.fMainForm.picMSDOS16.Picture
    Set lstIcon.ImageList = iconsImageList

    lstExecutable.Clear
    lstExecutable.Text = strNewExecutable

    Do While Len(strFile) > 0
      If LCase$(Right$(strFile, 4)) = ".bat" Or _
         LCase$(Right$(strFile, 4)) = ".com" Or _
         LCase$(Right$(strFile, 4)) = ".exe" _
      Then
        lstExecutable.AddItem strFile
      ElseIf LCase$(Right$(strFile, 4)) = ".ico" Then
        Dim strLCaseKey As String
        strLCaseKey = LCase$(strFile)
        iconsImageList.ListImages.Add , strLCaseKey, LoadPicture(strNewDirectory & strFile, , , 16, 16)
        lstIcon.ComboItems.Add , strLCaseKey, strFile, strLCaseKey, strLCaseKey
      End If

      strFile = FileSystem.Dir
    Loop

    ' Make sure the icon shows (Windows limitation)
    onIconChange
  End If

  strLastDirectory = strNewDirectory
End Sub

'
' Called when the text in the 'icons' list box changes
'
Private Sub onIconChange( _
)

  If Len(lstIcon.Text) < 1 Then Exit Sub

  ' Look at the executable currently in the "Executable" box and make it relative
  '  to the path in the 'directory' edit box
  If isAbsolutePath(lstIcon.Text) Then
    lstIcon.Text = makeRelativePath(lstIcon.Text, edtDirectory.Text)
  End If

  ' Set the main (32x32) icon
  setProgramIcon

  ' If the icon path is not already in the drop-down, add it as the
  '  first list item with its icon and select it (otherwise Windows
  '  loses the icon!)
  Dim item As ComboItem
  Dim strLCaseKey As String
  strLCaseKey = LCase$(lstIcon.Text)

  On Error Resume Next

  Set item = lstIcon.ComboItems.item(strLCaseKey)

  ' If the icon in the edit part of the combo is not in the list then add it
  If item Is Nothing Then
    ' Load the icon into the icon list (so that it can be associated with a list item)
    lstIcon.ImageList.ListImages.Add , strLCaseKey, LoadPicture(makeAbsoultePath(lstIcon.Text, edtDirectory.Text), , , 16, 16)

    If lstIcon.ComboItems.Count < 1 Then GoTo EmptyList

    If lstIcon.ComboItems.item(1).Tag = "custom" Then
      Set item = lstIcon.ComboItems.item(1)
      item.Key = strLCaseKey
      item.Image = strLCaseKey
      item.SelImage = strLCaseKey
      item.Text = lstIcon.Text
    Else
EmptyList:
      Set item = lstIcon.ComboItems.Add(1, strLCaseKey, lstIcon.Text, strLCaseKey, strLCaseKey)
      item.Tag = "custom"
    End If
  End If

  lstIcon.SelectedItem = item
End Sub

'
' Called when the text in the 'executable(s)' list box changes
'
Private Sub onExecutableChange( _
)

  If Len(lstExecutable.Text) < 1 Then Exit Sub

  ' Look at the executable currently in the "Executable" box and make it relative
  '  to the path in the 'directory' edit box
  If isAbsolutePath(lstExecutable.Text) Then
    lstExecutable.Text = makeRelativePath(lstExecutable.Text, edtDirectory.Text)
  End If
End Sub

'----------------------------------------------------------------------------
' UTILITY PROCEDURES (3)
'----------------------------------------------------------------------------

'
' Called when the text in the 'executable(s)' list box changes
'
Private Sub setProgramIcon( _
)
  On Error Resume Next
  Set picProgramIcon.Picture = LoadPicture(makeAbsoultePath(lstIcon.Text, edtDirectory.Text), , , 32, 32)
End Sub

'----------------------------------------------------------------------------
' FORM METHODS
'----------------------------------------------------------------------------

'
'
'
Private Sub Form_Load()
  'center the form
  Me.Move (Screen.Width - Me.Width) / 2, (Screen.Height - Me.Height) / 2

  strLastDirectory = ""
End Sub

'----------------------------------------------------------------------------
' PROGRAM TAB METHODS
'----------------------------------------------------------------------------

'
' Browse for icon files
'
Private Sub butIcon_Click()
  Dim strPath As String
  strPath = makeAbsoultePath(lstIcon.Text, edtDirectory.Text)

  Dim strDirectory As String, strFile As String
  splitPath strPath, strDirectory, strFile
  If Len(strDirectory) < 1 Then strDirectory = edtDirectory.Text

  strFile = modComDlg.DialogFile(Me.hWnd, 1, "Open Icon File", strFile, "Icon Files (*.ico)" & Chr$(0) & "*.ico" & Chr$(0) & "All Files" & Chr$(0) & "*.*", strDirectory, "")

  If Len(strFile) > 0 Then
    lstIcon.Text = strFile
    onIconChange
  End If
End Sub

'
' Browse for program directory
'
Private Sub butDirectory_Click()
  Dim strDirectory As String
  strDirectory = BrowseFolder(Me.hWnd, "Browse for Folder")

  If Len(strDirectory) > 0 Then
    edtDirectory.Text = strDirectory
    onDirectoryChange
  End If
End Sub

'
' Browse for program executables
'
Private Sub butExecutable_Click()
  Dim strPath As String
  strPath = makeAbsoultePath(lstExecutable.Text, edtDirectory.Text)

  Dim strDirectory As String, strFile As String
  splitPath strPath, strDirectory, strFile
  If Len(strDirectory) < 1 Then strDirectory = edtDirectory.Text

  strFile = modComDlg.DialogFile(Me.hWnd, 1, "Open Executable File", strFile, "All Executable Files (*.bat,*.com,*.exe)" & Chr$(0) & "*.bat;*.com;*.exe" & Chr$(0) & "Batch Files (*.bat)" & Chr$(0) & "*.bat" & Chr$(0) & "Program Files (*.com,*.exe)" & Chr$(0) & "*.com;*.exe" & Chr$(0) & "All Files" & Chr$(0) & "*.*", strDirectory, "")

  If Len(strFile) > 0 Then
    lstExecutable.Text = strFile
    onExecutableChange
  End If
End Sub

'
' Update relative paths in icons & executables when the
'  program base directory changes
'
Private Sub edtDirectory_Validate(Cancel As Boolean)
  Cancel = False
  onDirectoryChange
End Sub

'
' Update main (32x32) program icon when selection changes inside the
'  icons combo box/list
'
Private Sub lstIcon_Click()
  If Not lstIcon.SelectedItem Is Nothing Then
    If idxLastIcon <> lstIcon.SelectedItem.Index Then
      setProgramIcon
    End If
  End If
End Sub

'
' If the icon path is absolute then make it relative when leaving the edit box
'
Private Sub lstIcon_Validate(Cancel As Boolean)
  Cancel = False
  onIconChange
End Sub

'
' If the executable path is absolute then make it relative when leaving the edit box
'
Private Sub lstExecutable_Validate(Cancel As Boolean)
  Cancel = False
  onExecutableChange
End Sub
