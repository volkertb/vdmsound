Attribute VB_Name = "modConfig"
'
' Loads the main .ini file and initializes the application accordingly
'
Public Sub LoadCategories(fMainForm As frmMain)
  Dim iniFileName As String
  iniFileName = modIniFile.MakeIniPathStr(App.Path, "LaunchPad.ini")
  modIniFile.EnsurePathExists iniFileName

  ' Get the number of categories that exist
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, "categories", "count"))

  ' Create each category
  For i = 0 To numItems - 1
    Dim itemName As String
    Dim itemValue As String
    itemName = "item" & Format$(i)
    itemValue = modIniFile.ReadIniString(iniFileName, "categories", itemName)

    LoadCategoryBranch fMainForm.tvTreeView, itemValue
  Next
End Sub

'
' Given an absolute path, this function will create an entire
'  branch of categories.
'
Public Function LoadCategoryBranch(tvTree As TreeView, _
  ByVal strFullPath As String) As Node

  Dim splitPos As Long
  splitPos = InStrRev(strFullPath, ".")

  If splitPos > 0 Then
    Dim tvParentNode As Node
    Set tvParentNode = LoadCategoryBranch(tvTree, Left$(strFullPath, splitPos - 1))
    Set LoadCategoryBranch = AddNode(tvTree, tvParentNode, Mid$(strFullPath, splitPos + 1)) ' return
  Else
    Set LoadCategoryBranch = AddNode(tvTree, Nothing, strFullPath)  ' return
  End If
End Function

'
' Attempts to rename a category.  The category's key (as well as
'  the keys of its children) are updated.  If the function is
'  successful, the function returns zero, otherwise (e.g. if the
'  key/category name already exists), it returns a non-zero value.
' If the renaming is successful, it is mirrored in the configuration file.
'
Public Function RenameCategory(tvNode As Node, _
  ByVal strNewNodeText As String) As Integer

  If RenameNode(tvNode, strNewNodeText) <> 0 Then
    RenameCategory = 1  ' return
  Else
    'TODO: rename category inside .ini file

    If tvNode.Children > 0 Then
      Dim curNode As Node
      Set curNode = tvNode.Child

      Do Until curNode Is Nothing
        RenameCategory curNode, curNode.Text
        Set curNode = curNode.Next
      Loop
    End If

    RenameCategory = 0  ' return
  End If
End Function

'
' Adds a node to the given tree.  If the node already
'  exists thena  reference to an existing node is returned,
'  otherwise a reference to the new node is returned.
'
Private Function AddNode(tvTree As TreeView, _
  tvParentNode As Node, _
  ByVal strNodeName As String) As Node

  Dim strFullNodeName As String

  On Error GoTo KeyExists

  If tvParentNode Is Nothing Then
    strFullNodeName = strNodeName
    Set AddNode = tvTree.Nodes.Add(, , strFullNodeName, StrDecode(strNodeName), "Folder Closed", "Folder Open") ' return
  Else
    strFullNodeName = tvParentNode.Key & "." & strNodeName
    Set AddNode = tvTree.Nodes.Add(tvParentNode.Key, tvwChild, strFullNodeName, StrDecode(strNodeName), "Folder Closed", "Folder Open") ' return
  End If
  Exit Function

KeyExists:
  Set AddNode = tvTree.Nodes.Item(strFullNodeName)  ' return

End Function

'
' Renames a node (both human-readable text and encoded key).
'  If another (sibling) node with the same name exists then the
'  function returns a non-zero value, otherwise the node is
'  renamed and zero is returned.
'
Private Function RenameNode(tvNode As Node, _
  ByVal strNewText As String) As Integer

  On Error GoTo KeyExists
  
  If tvNode.Parent Is Nothing Then
    tvNode.Key = StrEncode(strNewText)
  Else
    tvNode.Key = tvNode.Parent.Key & "." & StrEncode(strNewText)
  End If

  tvNode.Text = strNewText

  RenameNode = 0  ' return
  Exit Function

KeyExists:
  RenameNode = 1  ' return

End Function

'
' Encodes all non-ANSI non-alphanumeric characters using the
'  form '%xxxx', where 'xxxx' is the hexa-decimal representation
'  of the encoded character(s)
'
Public Function StrEncode(ByVal strSrc As String) As String
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
      strTmp = Mid$("0000", 1, 4 - Len(strTmp)) + strTmp    ' 0-pad hex the representation to 4 figures
      retVal = retVal + "%" + strTmp                        ' prepend the 4 hex figures with '%' and put in return value
    End If
  Next

  StrEncode = retVal    ' return
End Function

'
' Decodes strings encoded using StrEncode
'
Public Function StrDecode(ByVal strSrc As String) As String
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

'
'
'
Public Function Node2ASCII(leafNode As Node) As String
  Node2ASCII = Node2ASCII_Helper(leafNode, "")  ' return
End Function

Private Function Node2ASCII_Helper(leafNode As Node, suffix As String) As String
  If leafNode.Parent Is Nothing Then
    Node2ASCII_Helper = StrEncode(leafNode.Text) & suffix ' return
  Else
    Node2ASCII_Helper = Node2ASCII_Helper(leafNode.Parent, "." & StrEncode(leafNode.Text) & suffix) ' return
  End If
End Function
