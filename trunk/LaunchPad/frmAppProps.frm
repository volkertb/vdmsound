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
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   5280
   ScaleWidth      =   5550
   ShowInTaskbar   =   0   'False
   Begin VB.PictureBox picOptions 
      BorderStyle     =   0  'None
      Height          =   3795
      Index           =   6
      Left            =   210
      ScaleHeight     =   3795
      ScaleWidth      =   5085
      TabIndex        =   6
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
      TabIndex        =   5
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
      TabIndex        =   4
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
      TabIndex        =   3
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
      TabIndex        =   2
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
      TabIndex        =   1
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
            Picture         =   "frmAppProps.frx":000C
            Key             =   "Program"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0166
            Key             =   "Memory"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":02C0
            Key             =   "Sound"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":041A
            Key             =   "Music"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0574
            Key             =   "Joystick"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":06CE
            Key             =   "Video"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "frmAppProps.frx":0828
            Key             =   "Troubleshooting"
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
      TabIndex        =   0
      TabStop         =   0   'False
      Top             =   750
      Width           =   5085
      Begin VB.Frame fraExecutables 
         Caption         =   "&Executable(s)"
         Height          =   1755
         Left            =   120
         TabIndex        =   18
         Top             =   1920
         Width           =   4815
         Begin VB.TextBox edtDirectory 
            Height          =   315
            Left            =   1800
            TabIndex        =   20
            Top             =   300
            Width           =   2415
         End
         Begin VB.CommandButton butDirectory 
            Caption         =   "..."
            Height          =   315
            Left            =   4320
            TabIndex        =   21
            ToolTipText     =   "Browse..."
            Top             =   300
            Width           =   255
         End
         Begin VB.ComboBox lstExecutable 
            Height          =   315
            Left            =   1800
            TabIndex        =   23
            Top             =   720
            Width           =   2775
         End
         Begin VB.CommandButton butExecutablesAdvanced 
            Caption         =   "Ad&vanced..."
            Enabled         =   0   'False
            Height          =   345
            Left            =   3360
            TabIndex        =   24
            Top             =   1200
            Width           =   1215
         End
         Begin VB.Label lblDirectory 
            Alignment       =   1  'Right Justify
            Caption         =   "Program &directory:"
            Height          =   255
            Left            =   120
            TabIndex        =   19
            Top             =   360
            Width           =   1575
         End
         Begin VB.Label lblExecutable 
            Alignment       =   1  'Right Justify
            Caption         =   "Main e&xecutable:"
            Height          =   255
            Left            =   120
            TabIndex        =   22
            Top             =   780
            Width           =   1575
         End
      End
      Begin VB.Frame fraGeneral 
         Caption         =   "&General"
         Height          =   1695
         Left            =   120
         TabIndex        =   11
         Top             =   120
         Width           =   4815
         Begin VB.TextBox edtCategory 
            BackColor       =   &H8000000F&
            Height          =   315
            Left            =   1800
            Locked          =   -1  'True
            TabIndex        =   13
            Top             =   300
            Width           =   2775
         End
         Begin VB.TextBox edtName 
            Height          =   315
            Left            =   1800
            TabIndex        =   15
            Top             =   720
            Width           =   2775
         End
         Begin MSComctlLib.ImageCombo lstIcon 
            Height          =   330
            Left            =   1800
            TabIndex        =   17
            Top             =   1140
            Width           =   2775
            _ExtentX        =   4895
            _ExtentY        =   582
            _Version        =   393216
            ForeColor       =   -2147483640
            BackColor       =   -2147483643
            Text            =   "(default)"
         End
         Begin VB.Label lblIcon 
            Alignment       =   1  'Right Justify
            Caption         =   "Program &icon:"
            Height          =   255
            Left            =   120
            TabIndex        =   16
            Top             =   1200
            Width           =   1575
         End
         Begin VB.Label lblCategory 
            Alignment       =   1  'Right Justify
            Caption         =   "Category:"
            Height          =   255
            Left            =   840
            TabIndex        =   12
            Top             =   360
            Width           =   855
         End
         Begin VB.Label lblName 
            Alignment       =   1  'Right Justify
            Caption         =   "&Name:"
            Height          =   255
            Left            =   840
            TabIndex        =   14
            Top             =   780
            Width           =   855
         End
         Begin VB.Image picProgramIcon 
            Height          =   480
            Left            =   240
            Picture         =   "frmAppProps.frx":0982
            Top             =   420
            Width           =   480
         End
      End
   End
   Begin VB.CommandButton cmdApply 
      Caption         =   "&Apply"
      Height          =   345
      Left            =   4320
      TabIndex        =   9
      Top             =   4800
      Width           =   1095
   End
   Begin VB.CommandButton cmdCancel 
      Cancel          =   -1  'True
      Caption         =   "Cancel"
      Height          =   345
      Left            =   3120
      TabIndex        =   8
      Top             =   4800
      Width           =   1095
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "OK"
      Default         =   -1  'True
      Height          =   345
      Left            =   1920
      TabIndex        =   7
      Top             =   4800
      Width           =   1095
   End
   Begin MSComctlLib.TabStrip tbsOptions 
      Height          =   4545
      Left            =   105
      TabIndex        =   10
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

Private Sub Form_Load()
    'center the form
    Me.Move (Screen.Width - Me.Width) / 2, (Screen.Height - Me.Height) / 2
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
