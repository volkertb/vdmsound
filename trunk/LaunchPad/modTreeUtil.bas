Attribute VB_Name = "modTreeUtil"
'
' Adds a node to the given tree.  If the node already
'  exists then a reference to an existing node is returned,
'  otherwise a reference to the new node is returned.
'
Public Function AddNode( _
  tvTree As TreeView, _
  tvParentNode As Node, _
  ByVal strText As String, _
  ByVal strKey As String, _
  Optional strIcon As String = "Closed Folder", _
  Optional strSelIcon As String = "Open Folder" _
) As Node

  Dim tvNode As Node

  ' Check if a node with this name already exists
  Dim tvDupeNode As Node
  Set tvDupeNode = GetNodeByText(tvTree, tvParentNode, strText)

  If tvDupeNode Is Nothing Then
    If tvParentNode Is Nothing Then
      Set tvNode = tvTree.Nodes.Add(, , strKey, strText, strIcon, strSelIcon) ' return, new node created
    Else
      Set tvNode = tvTree.Nodes.Add(tvParentNode, tvwChild, strKey, strText, strIcon, strSelIcon) ' return, new node created
    End If
  Else
    Set tvNode = tvDupeNode  ' return, the existing node
  End If

  If tvNode.Parent Is Nothing Then tvTree.Sorted = tvTree.Sorted Else tvNode.Parent.Sorted = tvTree.Sorted ' Resort the siblings

  Set AddNode = tvNode
End Function

'
' Renames a node.  If another (sibling) node with the same text exists
'  then the function returns a non-zero value, otherwise the node is
'  renamed and zero is returned.
'
Public Function RenameNode( _
  tvTree As TreeView, _
  tvNode As Node, _
  ByVal strNewText As String _
) As Integer

  Debug.Assert Not tvNode Is Nothing

  Dim dupeNode As Node
  Set dupeNode = GetNodeByText(tvTree, tvNode.Parent, strNewText)

  ' Only rename node if another with the same name does *not* exist,
  '  or if attempting to change the case of the same node's spelling
  If (dupeNode Is Nothing) Or (dupeNode Is tvNode) Then
    tvNode.Text = strNewText
    If tvNode.Parent Is Nothing Then tvTree.Sorted = tvTree.Sorted Else tvNode.Parent.Sorted = tvTree.Sorted ' Resort the siblings

    RenameNode = 0  ' return
  Else
    RenameNode = 1  ' return
  End If
End Function

'
' Removes a node (and all its children) in a tree
'
Public Sub RemoveNode( _
  tvTree As TreeView, _
  tvNode As Node _
)

  Debug.Assert Not tvNode Is Nothing

  Dim tvChildNode As Node
  Set tvChildNode = tvNode.Child

  ' Go through all child nodes
  Do Until tvChildNode Is Nothing
    Dim tvNextChildNode As Node
    Set tvNextChildNode = tvChildNode.Next
    RemoveNode tvTree, tvChildNode
    Set tvChildNode = tvNextChildNode
  Loop

  tvTree.Nodes.Remove tvNode.Key
End Sub

'
' Looks up a node by key
'
Public Function GetNodeByKey( _
  tvTree As TreeView, _
  ByVal strKey As String _
) As Node

  On Error GoTo Error

  Set GetNodeByKey = tvTree.Nodes.Item(strKey)
  Exit Function

Error:
  Set GetNodeByKey = Nothing
End Function

'
' Looks up a node by text
'
Public Function GetNodeByText( _
  tvTree As TreeView, _
  tvParentNode As Node, _
  ByVal strText As String _
) As Node

  If tvTree.Nodes.Count <= 0 Then GoTo Error_NotFound

  Dim tvSiblingNode As Node

  If tvParentNode Is Nothing Then
    Set tvSiblingNode = tvTree.Nodes.Item(1).Root.FirstSibling
  Else
    Set tvSiblingNode = tvParentNode.Child
  End If

  strLCaseText = LCase(strText)

  ' Go through all child nodes
  Do Until tvSiblingNode Is Nothing
    If LCase$(tvSiblingNode.Text) = strLCaseText Then
       Set GetNodeByText = tvSiblingNode
       Exit Function
    End If

    Set tvSiblingNode = tvSiblingNode.Next
  Loop

Error_NotFound:
  Set GetNodeByText = Nothing
End Function

'
' Recursively expands or collapses the given node and all sub-nodes
'
Public Sub ExpandCollapseAllNodes( _
  tvNode As Node, _
  isExpanded As Boolean _
)

  Debug.Assert Not tvNode Is Nothing

  Dim tvChildNode As Node
  Set tvChildNode = tvNode.Child

  ' Go through all child nodes
  Do Until tvChildNode Is Nothing
    ExpandCollapseAllNodes tvChildNode, isExpanded
    Set tvChildNode = tvChildNode.Next
  Loop

  tvNode.Expanded = isExpanded
End Sub
