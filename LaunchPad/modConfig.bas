Attribute VB_Name = "modConfig"
'
'
'
Public iniFileName As String

'
' Allocates a uniquely identifying key (for either categories or
'  programs, based on prefix) inside the main INI file
'
Private Function AllocateKey( _
  strPrefix As String _
)

  Dim strKey As String

  For i = 0 To &H7FFFFFFF
    strKey = Hex(i)
    strKey = Mid$("00000000", Len(strKey) + 1) & strKey

    If Len(ReadIniSection(iniFileName, strPrefix & strKey)) <= 0 Then
      AllocateKey = strPrefix & strKey
      Exit Function
    End If
  Next

  AlloateKey = ""
  Exit Function
End Function

'
' Deletes any references a parent section might have to a given section
'
Private Sub DeleteParentReferences( _
  ByVal strKey As String, _
  ByVal strParentKey As String _
)

  ' Remove references to this element from the parent category
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strParentKey, "count"))

  Dim numDeletes As Long
  numDeletes = 0

  strKey = LCase$(strKey)

  ' Go through the elements listed in the parent
  For i = 0 To numItems - 1
    Dim itemValue As String
    itemValue = modIniFile.ReadIniString(iniFileName, strParentKey, "item" & Format$(i))

    If numDeletes > 0 Then
      modIniFile.WriteIniString iniFileName, strParentKey, "item" & Format$(i - numDeletes), itemValue
    End If

    If LCase$(itemValue) = strKey Then
      numDeletes = numDeletes + 1
    End If
  Next

  ' Finally remove the trailing element(s) and shorten the children list count
  If numDeletes > 0 Then
    modIniFile.WriteIniString iniFileName, strParentKey, "count", Format$(numItems - numDeletes)
  End If

  For j = 1 To numDeletes
    modIniFile.DeleteIniString iniFileName, strParentKey, "item" & Format$(numItems - j)
  Next
End Sub

'----------------------------------------------------------------------------
' CATEGORIES
'----------------------------------------------------------------------------

'
' Loads categories from the main .ini file
'
Public Function LoadCategories( _
  tvTree As TreeView, _
  Optional strRoot As String = "root", _
  Optional tvParentNode As Node = Nothing _
) As Long

  On Error GoTo Error

  ' Empty the categories tree and
  tvTree.Nodes.Clear

  ' Load top-level categories, skipping the first (root) level category
  LoadCategory tvTree, "cat.root", Nothing

  LoadCategories = 0
  Exit Function

Error:
  modError.ReportError "Unable to load categories from '" & iniFileName & "'."
Error_silent:
  LoadCategories = -1
End Function

'
'
'
Private Sub LoadCategory( _
  tvTree As TreeView, _
  strKey As String, _
  tvParentNode As Node _
)

  ' Get the category's name
  Dim strText As String
  strText = StrDecode(modIniFile.ReadIniString(iniFileName, strKey, "name"))

  ' Create this category in the tree (unless we have to skip some levels)
  Dim tvThisNode As Node
  If tvParentNode Is Nothing Then
    If Len(strText) <= 0 Then strText = "VDMS Desktop"
    Set tvThisNode = modTreeUtil.AddNode(tvTree, Nothing, strText, strKey, "Desktop", "Desktop")
    tvThisNode.Expanded = True
  Else
    Set tvThisNode = modTreeUtil.AddNode(tvTree, tvParentNode, strText, strKey)
  End If

  ' Get the number of sub-elements that exist
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strKey, "count"))

  ' Create each sub-category
  For i = 0 To numItems - 1
    Dim itemValue As String
    itemValue = modIniFile.ReadIniString(iniFileName, strKey, "item" & Format$(i))

    If LCase$(Left$(itemValue, 4)) = "cat." Then
      LoadCategory tvTree, itemValue, tvThisNode
    End If
  Next
End Sub

'
'
'
Public Function GetLastSelectedCategory( _
  tvTree As TreeView _
) As Node
  Set GetLastSelectedCategory = modTreeUtil.GetNodeByKey(tvTree, modIniFile.ReadIniString(modConfig.iniFileName, "general", "lastCat"))
End Function

'
'
'
Public Function SetLastSelectedCategory( _
  tvTree As TreeView _
) As Long
  On Error GoTo Error

  modIniFile.WriteIniString modConfig.iniFileName, "general", "lastCat", tvTree.SelectedItem.Key

  SetLastSelectedCategory = 0
  Exit Function
Error:
  SetLastSelectedCategory = -1
End Function

'
' Adds a category to the category tree and main INI file
'
Public Function AddCategory( _
  tvTree As TreeView, _
  tvParentNode As Node, _
  strText As String _
) As Node

  On Error GoTo Error

  Dim strKey As String
  strKey = modConfig.AllocateKey("cat.")

  modIniFile.WriteIniString iniFileName, strKey, "count", "0"
  modIniFile.WriteIniString iniFileName, strKey, "name", StrEncode(strText)
  
  Dim strParentKey As String
  strParentKey = tvParentNode.Key

  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strParentKey, "count"))
  modIniFile.WriteIniString iniFileName, strParentKey, "count", Format$(numItems + 1)
  modIniFile.WriteIniString iniFileName, strParentKey, "item" & Format$(numItems), strKey

  If tvTree Is Nothing Then
    Set AddCategory = Nothing
  Else
    Set AddCategory = modTreeUtil.AddNode(tvTree, tvParentNode, strText, strKey)
  End If
  Exit Function

Error:
  modError.ReportError "Unable to create category '" & strText & "'."
  Set AddCategory = Nothing
End Function

'
' Attempts to rename a category.  If the function is successful, the
'  function returns zero, otherwise (e.g. if the category name already
'  exists), it returns a non-zero value.
' If the renaming is successful, it is mirrored in the .ini configuration file.
'
Public Function RenameCategory( _
  tvTree As TreeView, _
  tvNode As Node, _
  ByVal strNewText As String _
) As Integer

  Dim strOldText As String
  strOldText = tvNode.Text

  On Error GoTo Error

  ' Attempt to rename the current node (assuming the new key is unique)
  If modTreeUtil.RenameNode(tvTree, tvNode, strNewText) <> 0 Then
    RenameCategory = 1  ' return, duplicate name
  Else
    ' Rename category inside .ini file
    modIniFile.WriteIniString iniFileName, tvNode.Key, "name", StrEncode(strNewText)

    RenameCategory = 0  ' return, the node was successfully renamed
  End If
  Exit Function

Error:
  modError.ReportError "Unable to rename category to '" & strNewText & "'."
  On Error Resume Next
  modTreeUtil.RenameNode tvTree, tvNode, strOldText ' roll back changes
  RenameCategory = -1   ' return, error
End Function

'
'
'
Public Function DeleteCategory( _
  tvTree As TreeView, _
  tvNode As Node _
) As Long

  On Error GoTo Error

  Dim strKey As String
  strKey = tvNode.Key

  ' We have to locate and erase any reference the parent category might have
  '  to this element, otherwise those references will point to non-existent
  '  sections in the INI file
  DeleteParentReferences strKey, tvNode.Parent.Key

  ' Perform a recursive delete on this category and everything below it
  If DeleteCategory_Helper(strKey) <> 0 Then
    LoadCategories tvTree ' an error resulted in only a partial delete, so we resynchronize
  Else
    modTreeUtil.RemoveNode tvTree, tvNode ' the delete was successful, remove the branch from the GUI
  End If
  DeleteCategory = 0  ' return, success
  Exit Function

Error:
  modError.ReportError "An error was encountered while deleting the category '" & strKey & "'."
  DeleteCategory = -1 ' return, error
End Function

'
'
'
Private Function DeleteCategory_Helper( _
  strKey As String _
)

  On Error GoTo Error

  Dim numItems As Long
  Dim itemValue As String

  ' If dealing with a category we have to recursively delete any
  '  sub-categories or programs
  If LCase$(Left$(strKey, 4)) = "cat." Then
    ' Get the number of sub-elements that exist in this category
    numItems = Val(modIniFile.ReadIniString(iniFileName, strKey, "count"))

    ' Delete each sub-category and/or program listed
    For i = 0 To numItems - 1
      itemValue = modIniFile.ReadIniString(iniFileName, strKey, "item" & Format$(i))
      If DeleteCategory_Helper(itemValue) <> 0 Then GoTo Error
    Next
  End If

  ' Finally erase the element's section from the INI file
  modIniFile.DeleteIniSection iniFileName, strKey
  DeleteCategory_Helper = 0   ' return, success
  Exit Function

Error:
  DeleteCategory_Helper = -1  ' return, partial or complete failure
End Function

'----------------------------------------------------------------------------
' PROGRAMS
'----------------------------------------------------------------------------

'
' Loads list of programs for a given category from the main .ini file
'
Public Function LoadPrograms( _
  lvList As ListView, _
  strKey As String _
) As Long

  On Error GoTo Error

  ' Empty the programs list
  fMainForm.lvListView.ListItems.Clear

  ' Reset the program icons (detach image list from list view first)
  ResetProgramsIcons lvList, 32, modMain.fMainForm.picMSDOS32
  ResetProgramsIcons lvList, 16, modMain.fMainForm.picMSDOS16

  ' Get the number of sub-elements that exist
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strKey, "count"))

  ' Create each program
  For i = 0 To numItems - 1
    Dim itemValue As String
    itemValue = modIniFile.ReadIniString(iniFileName, strKey, "item" & Format$(i))

    If LCase$(Left$(itemValue, 4)) = "app." Then
      LoadProgram lvList, itemValue
    End If
  Next
  LoadPrograms = 0
  Exit Function

Error:
  modError.ReportError "Unable to load programs from '" & iniFileName & "'."
  LoadPrograms = -1
End Function

'
'
'
Private Sub ResetProgramsIcons( _
  lvList As ListView, _
  intSize As Integer, _
  icoDefault As Picture _
)
  Dim imlListIcons As ImageList

  Select Case intSize
    Case 32
      Set imlListIcons = lvList.Icons
      Set lvList.Icons = Nothing
    Case 16
      Set imlListIcons = lvList.SmallIcons
      Set lvList.SmallIcons = Nothing
    Case Else
      Err.Raise ccInvalidProcedureCall, "ResetProgramsIcons", "Invalid icon size (" & intSize & ")"
  End Select

  imlListIcons.ListImages.Clear
  imlListIcons.ImageHeight = intSize
  imlListIcons.ImageWidth = intSize
  imlListIcons.ListImages.Add , "default", icoDefault

  Select Case intSize
    Case 32
      Set lvList.Icons = imlListIcons
    Case 16
      Set lvList.SmallIcons = imlListIcons
    Case Else
      Err.Raise ccInvalidProcedureCall, "ResetProgramsIcons", "Invalid icon size (" & intSize & ")"
  End Select
End Sub

'
'
'
Private Sub LoadProgram( _
  lvList As ListView, _
  ByVal strKey As String _
)

  Dim icoKey As String
  icoKey = "default"

  On Error GoTo Error_UseDefaultIcon

  Dim strText As String
  strText = StrDecode(modIniFile.ReadIniString(iniFileName, strKey, "name"))

  Dim strIconPath As String
  strIconPath = modIniFile.ReadIniString(iniFileName, strKey, "icon")

  fMainForm.imlListIcons32.ListImages.Add , strKey, LoadPicture(strIconPath, , , 32, 32)
  fMainForm.imlListIcons16.ListImages.Add , strKey, LoadPicture(strIconPath, , , 16, 16)
  icoKey = strKey

Error_UseDefaultIcon:
  lvList.ListItems.Add , strKey, strText, icoKey, icoKey
End Sub

'
' Adds a program to the category tree and main INI file
'
Public Function AddProgram( _
  lvList As ListView, _
  strParentKey As String, _
  strText As String _
) As ListItem

  On Error GoTo Error

  Dim strKey As String
  strKey = modConfig.AllocateKey("app.")

  modIniFile.WriteIniString iniFileName, strKey, "name", StrEncode(strText)

  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strParentKey, "count"))
  modIniFile.WriteIniString iniFileName, strParentKey, "count", Format$(numItems + 1)
  modIniFile.WriteIniString iniFileName, strParentKey, "item" & Format$(numItems), strKey

  If lvList Is Nothing Then
    Set AddProgram = Nothing
  Else
    Set AddProgram = modListUtil.AddItem(lvList, strText, strKey)
  End If
  Exit Function

Error:
  modError.ReportError "Unable to create program '" & strText & "'."
  Set AddProgram = Nothing
End Function

'
' Attempts to rename a program.  If the function is successful, the
'  function returns zero, otherwise (e.g. if the program name already
'  exists), it returns a non-zero value.
' If the renaming is successful, it is mirrored in the .ini configuration file.
'
Public Function RenameProgram( _
  lvList As ListView, _
  lvItem As ListItem, _
  ByVal strNewText As String _
) As Integer

  Dim strOldText As String
  strOldText = lvItem.Text

  On Error GoTo Error

  ' Attempt to rename the current node (assuming the new key is unique)
  If modListUtil.RenameItem(lvList, lvItem, strNewText) <> 0 Then
    RenameProgram = 1  ' return, duplicate name
  Else
    ' Rename category inside .ini file
    modIniFile.WriteIniString iniFileName, lvItem.Key, "name", StrEncode(strNewText)

    RenameProgram = 0 ' return, the item was successfully renamed
  End If
  Exit Function

Error:
  modError.ReportError "Unable to rename program to '" & strNewText & "'."
  On Error Resume Next
  modListUtil.RenameItem lvList, lvItem, strOldText ' roll back changes
  RenameProgram = -1  ' return, error
End Function

'
'
'
Public Function DeleteProgram( _
  lvList As ListView, _
  lvItem As ListItem, _
  strParentKey As String _
) As Long

  On Error GoTo Error

  Dim strKey As String
  strKey = lvItem.Key

  ' We have to locate and erase any reference the parent category might have
  '  to this element, otherwise those references will point to non-existent
  '  sections in the INI file
  DeleteParentReferences strKey, strParentKey

  ' Finally erase the element's section from the INI file
  modIniFile.DeleteIniSection iniFileName, strKey

  modListUtil.RemoveItem lvList, lvItem ' remove the item from the GUI
  DeleteProgram = 0   ' return, success
  Exit Function

Error:
  modError.ReportError "An error was encountered while deleting the program '" & strKey & "'."
  DeleteProgram = -1  ' return, error
End Function

'----------------------------------------------------------------------------
' STRING ENCODING/DECODING
'----------------------------------------------------------------------------

'
' Encodes all non-ANSI, non-alphanumeric characters using the
'  form '%xxxx', where 'xxxx' is the hexa-decimal representation
'  of the encoded character(s)
'
Public Function StrEncode( _
  ByVal strSrc As String _
) As String

  Dim retVal As String
  Dim curChr As String

  retVal = ""                     ' the return value

  For i = 1 To Len(strSrc)
    curChr = Mid$(strSrc, i, 1)   ' the character being currently processed

    If (Asc(curChr) >= Asc("0") And Asc(curChr) <= Asc("9")) Or _
       (Asc(curChr) >= Asc("A") And Asc(curChr) <= Asc("Z")) Or _
       (Asc(curChr) >= Asc("a") And Asc(curChr) <= Asc("z")) _
    Then                          ' an ANSI alphanumeric character
      retVal = retVal + curChr
    Else                          ' non-ANSI, non-alphanumeric: needs to be encoded!
      Dim strTmp As String        ' build the character's hex representation into this string
      strTmp = Hex(Asc(curChr))
      strTmp = Mid$("0000", Len(strTmp) + 1) + strTmp       ' 0-pad hex the representation to 4 figures
      retVal = retVal + "%" + strTmp                        ' prepend the 4 hex figures with '%' and put in return value
    End If
  Next

  StrEncode = retVal    ' return
End Function

'
' Decodes strings encoded using StrEncode
'
Public Function StrDecode( _
  ByVal strSrc As String _
) As String

  Dim retVal As String
  Dim curChr As String

  retVal = ""                     ' the return value

  For i = 1 To Len(strSrc)
    curChr = Mid$(strSrc, i, 1)   ' the character being currently processed

    If curChr <> "%" Then         ' non-encoded character
      retVal = retVal + curChr
    Else                          ' encoded character: must decode!
      Dim lngTmp As Long          ' hex value of the encoded character
      lngTmp = Val("&H" + Mid$(strSrc, i + 1, 4))
      retVal = retVal + Chr$(lngTmp)    ' append decoded character value to return value
      i = i + 4                   ' skip the following 4 characters because they are the 4 hex digits we just processed
    End If
  Next

  StrDecode = retVal    ' return
End Function
