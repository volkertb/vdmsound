Attribute VB_Name = "modConfig"
Public iniFileName As String

'
' Loads categories from the main .ini file
'
Public Sub LoadCategories(fMainForm As frmMain)
  ' Get the number of categories that exist
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, "categories", "count"))

  ' Empty the categories tree
  fMainForm.tvTreeView.Nodes.Clear

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
  ByVal strNodeKey As String) As Node

  ' Pick up the last separating '.' which splits the branch into
  '  a parent-branch key and a child leaf-node key
  Dim splitPos As Long
  splitPos = InStrRev(strNodeKey, ".")

  ' Recursevly create the parent-branch, then add the child leaf-node
  If splitPos > 0 Then
    Dim tvParentNode As Node
    Set tvParentNode = LoadCategoryBranch(tvTree, Left$(strNodeKey, splitPos - 1))
    Set LoadCategoryBranch = AddNode(tvTree, tvParentNode, Mid$(strNodeKey, splitPos + 1)) ' return, middle/leaf node
  Else
    Set LoadCategoryBranch = AddNode(tvTree, Nothing, strNodeKey)  ' return, root node
  End If
End Function

'
' Loads list of applications for a given category from the main .ini file
'
Public Sub LoadApplications(fMainForm As frmMain, _
  strNodeKey As String)

  Dim strIniSection As String
  strIniSection = "categories." & strNodeKey

  ' Get the number of applications that exist
  Dim numItems As Long
  numItems = Val(modIniFile.ReadIniString(iniFileName, strIniSection, "count"))

  ' Empty the applications list
  fMainForm.lvListView.ListItems.Clear

  ' Create each application
  For i = 0 To numItems - 1
    Dim itemName As String
    Dim itemValue As String
    itemName = "item" & Format$(i)
    itemValue = modIniFile.ReadIniString(iniFileName, strIniSection, itemName)

    LoadApplicationPreview fMainForm.lvListView, strNodeKey, itemValue
  Next
End Sub

'
' Loads list of applications for a given category from the main .ini file
'
Public Sub LoadApplicationPreview(lvList As ListView, _
  ByVal strNodeKey As String, _
  ByVal strAppKey As String)

  lvList.ListItems.Add , strNodeKey & "." & strAppKey, StrDecode(strAppKey)
End Sub

'
' Attempts to rename a category.  The category's key (as well as
'  the keys of its children) are updated.  If the function is
'  successful, the function returns zero, otherwise (e.g. if the
'  key/category name already exists), it returns a non-zero value.
' If the renaming is successful, it is mirrored in the configuration file.
'
Public Function RenameCategory(tvTree As TreeView, _
  tvNode As Node, _
  ByVal strNewNodeText As String) As Integer

  ' Attempt to rename the current node (assuming the new key is unique)
  If RenameNode(tvTree, tvNode, strNewNodeText) <> 0 Then
    RenameCategory = 1  ' return, duplicate key
  Else
    'TODO: rename category inside .ini file

    ' If the renamed node has children then we  must recursively rebuild
    '  all children's keys to take into account this node's new key
    If tvNode.Children > 0 Then
      Dim curNode As Node
      Set curNode = tvNode.Child

      Do Until curNode Is Nothing
        RenameCategory tvTree, curNode, curNode.Text
        Set curNode = curNode.Next
      Loop
    End If

    RenameCategory = 0  ' return, the node was successfully renamed
  End If
End Function

'
' Adds a node to the given tree.  If the node already
'  exists thena  reference to an existing node is returned,
'  otherwise a reference to the new node is returned.
'
Private Function AddNode(tvTree As TreeView, _
  tvParentNode As Node, _
  ByVal strPartialNodeKey As String) As Node

  Dim strNodeKey As String
  Dim tmpParent As Variant

  If tvParentNode Is Nothing Then
    ' Adding a node at the root
    tmpParent = Null
    strNodeKey = strPartialNodeKey
  Else
    ' Adding a node under another node
    tmpParent = tvParentNode
    strNodeKey = tvParentNode.Key & "." & strPartialNodeKey
  End If

  ' Check if a node with this key already exists
  Dim tmpChild As Variant
  Set tmpChild = GetNode(tvTree, strNodeKey)

  If tmpChild Is Nothing Then
    Set AddNode = tvTree.Nodes.Add(tmpParent, tvwChild, strNodeKey, StrDecode(strPartialNodeKey), "Folder Closed", "Folder Open") ' return, new node created
  Else
    Set AddNode = tmpChild ' return, the existing node with the same key
  End If
End Function

'
' Renames a node (both human-readable text and encoded key).
'  If another (sibling) node with the same name exists then the
'  function returns a non-zero value, otherwise the node is
'  renamed and zero is returned.
'
Private Function RenameNode(tvTree As TreeView, _
  tvNode As Node, _
  ByVal strNewText As String) As Integer

  Dim strNewNodeKey As String

  If tvNode.Parent Is Nothing Then
    strNewNodeKey = StrEncode(strNewText)
  Else
    strNewNodeKey = tvNode.Parent.Key & "." & StrEncode(strNewText)
  End If

  Dim dupeNode As Node
  Set dupeNode = GetNode(tvTree, strNewNodeKey)

  ' Only rename node if another with the same name does *not* exist,
  '  or if attempting to change the case of the same node's spelling
  If (dupeNode Is Nothing) Or (dupeNode Is tvNode) Then
    tvNode.Text = strNewText
    tvNode.Key = strNewNodeKey
    RenameNode = 0  ' return
  Else
    RenameNode = 1  ' return
  End If
End Function

'
' Looks up a node by its key (case insensitive)
'
Private Function GetNode(tvTree As TreeView, _
  ByVal strNodeKey As String) As Node

  strNodeKey = LCase(strNodeKey)

  ' Go through all nodes
  For i = 1 To tvTree.Nodes.Count
    If LCase(tvTree.Nodes.Item(i).Key) = strNodeKey Then
      Set GetNode = tvTree.Nodes.Item(i)  ' return
      Exit Function
    End If
  Next
End Function

'
' Encodes all non-ANSI, non-alphanumeric characters using the
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
