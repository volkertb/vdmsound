Attribute VB_Name = "modMain"
Global Const LISTVIEW_MODE0 = "View Large Icons"
Global Const LISTVIEW_MODE1 = "View Small Icons"
Global Const LISTVIEW_MODE2 = "View List"
Global Const LISTVIEW_MODE3 = "View Details"
Public fMainForm As frmMain
Private lastNode As Node

Sub Main()
  Set fMainForm = New frmMain

  ' Initialize INI file
  modConfig.iniFileName = modIniFile.MakeIniPathStr(App.Path, "LaunchPad.ini")
  modIniFile.EnsurePathExists modConfig.iniFileName

  ' Load categories
  modConfig.LoadCategories fMainForm.tvTreeView
  SelectCategory GetLastSelectedCategory(fMainForm.tvTreeView)

  ' Start the GUI
  fMainForm.Show
End Sub

'
'
'
Public Sub SelectCategory( _
  tvNode As Node _
)
  If lastNode Is tvNode Then Exit Sub

  fMainForm.tvTreeView.SelectedItem = tvNode
  Set lastNode = tvNode

  If tvNode Is Nothing Then
    modConfig.LoadApplications fMainForm.lvListView, "cat.root"
    fMainForm.mnuTreeViewDelete.Enabled = False
    fMainForm.mnuTreeViewRename.Enabled = False
  Else
    modConfig.LoadApplications fMainForm.lvListView, tvNode.Key
    fMainForm.mnuTreeViewDelete.Enabled = True
    fMainForm.mnuTreeViewRename.Enabled = True
  End If
End Sub
