Attribute VB_Name = "modMain"
Global Const LISTVIEW_MODE0 = "View Large Icons"
Global Const LISTVIEW_MODE1 = "View Small Icons"
Global Const LISTVIEW_MODE2 = "View List"
Global Const LISTVIEW_MODE3 = "View Details"
Public fMainForm As frmMain

Sub Main()
  Set fMainForm = New frmMain

  ' Initialize INI file
  modConfig.iniFileName = modIniFile.MakeIniPathStr(App.Path, "LaunchPad.ini")
  modIniFile.EnsurePathExists modConfig.iniFileName

  ' Load categories
  modConfig.LoadCategories fMainForm.tvTreeView
  fMainForm.SelectCategory modConfig.GetLastSelectedCategory(fMainForm.tvTreeView)

  ' Start the GUI
  fMainForm.Show
End Sub
