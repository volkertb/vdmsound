Attribute VB_Name = "modListUtil"
'
' Adds an item to the given list.  If the item already
'  exists then a reference to an existing item is returned,
'  otherwise a reference to the new item is returned.
'
Public Function AddItem( _
  lvList As ListView, _
  ByVal strText As String, _
  ByVal strKey As String _
) As ListItem

  Dim lvItem As ListItem

  ' Check if an item with this name already exists
  Dim lvDupeItem As ListItem
  Set lvDupeItem = GetItemByText(lvList, strText)

  If lvDupeItem Is Nothing Then
    Set lvItem = lvList.ListItems.Add(, strKey, strText, "default", "default") ' return, new item created
  Else
    Set lvItem = lvDupeItem  ' return, the existing item
  End If

  Set AddItem = lvItem
End Function

'
' Renames an item.  If another item with the same text exists
'  then the function returns a non-zero value, otherwise the item is
'  renamed and zero is returned.
'
Public Function RenameItem( _
  lvList As ListView, _
  lvItem As ListItem, _
  ByVal strNewText As String _
) As Integer

  Dim dupeItem As ListItem
  Set dupeItem = GetItemByText(lvList, strNewText)

  ' Only rename the item if another with the same name does *not* exist,
  '  or if attempting to change the case of the same item's spelling
  If (dupeItem Is Nothing) Or (dupeItem Is lvItem) Then
    lvItem.Text = strNewText
    RenameItem = 0  ' return
  Else
    RenameItem = 1  ' return
  End If
End Function

'
' Removes an item in a list
'
Public Sub RemoveItem( _
  lvList As ListView, _
  lvItem As ListItem _
)
  lvList.ListItems.Remove lvItem.Key
End Sub

'
' Looks up an item by key
'
Public Function GetItemByKey( _
  lvList As ListView, _
  ByVal strKey As String _
) As ListItem

  On Error GoTo Error

  Set GetItemByKey = lvList.ListItems.Item(strKey)
  Exit Function

Error:
  Set GetItemByKey = Nothing
End Function

'
' Looks up an item by text
'
Public Function GetItemByText( _
  lvList As ListView, _
  ByVal strText As String _
) As ListItem

  If lvList.ListItems.Count <= 0 Then GoTo Error_NotFound

  strLCaseText = LCase(strText)

  Dim lvItem As ListItem
  For Each lvItem In lvList.ListItems
    If LCase$(lvItem.Text) = strLCaseText Then
       Set GetItemByText = lvItem
       Exit Function
    End If
  Next

Error_NotFound:
  Set GetItemByText = Nothing
End Function
