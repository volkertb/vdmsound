Attribute VB_Name = "modConfig"
'
' Loads the main .ini file and initializes the application accordingly
'
Public Sub Initialize(fMainForm As frmMain)
  Dim iniFileName As String
  iniFileName = modIniFile.MakeIniPathStr(App.Path, "LaunchPad.ini")
  modIniFile.EnsurePathExists iniFileName

  LoadCategories fMainForm, iniFileName
End Sub

'
'
'
Private Sub LoadCategories(fMainForm As frmMain, strIni As String)
  Dim numItems As Long

  numItems = Val(modIniFile.ReadIniString(strIni, "categories", "count"))

  For i = 0 To numItems - 1
    Dim itemName As String
    Dim itemValue As String
    itemName = "item" & Format$(i)
    itemValue = modIniFile.ReadIniString(strIni, "categories", itemName)
    AddNode fMainForm.tvTreeView, itemValue
  Next
End Sub

'
'
'
Private Function AddNode(tvTree As TreeView, _
  ByVal strPath As String) As Node

  Dim strTmpPath As String
  Dim strNodeName As String
  Dim strNodeParent As String

  strTmpPath = ""

  While Len(strPath) > 0
    Dim splitPos As Long
    splitPos = InStr(1, strPath, ".")

    ' Grab the next '.'-delimited element in the given path
    If splitPos > 0 Then
      strNodeName = Left$(strPath, splitPos - 1)
      strPath = Mid$(strPath, splitPos + 1)
    Else
      strNodeName = strPath
      strPath = ""
    End If

    ' Remember below whom we will add the current node
    strNodeParent = strTmpPath

    ' Now build the full path of the current node
    If Len(strTmpPath) = 0 Then
      strTmpPath = strNodeName
    Else
      strTmpPath = strTmpPath + "." + strNodeName
    End If

    ' Add the current node to the tree
    On Error Resume Next            ' if the node already exists, ignore the ensueing error
    If Len(strNodeParent) = 0 Then  ' the node hangs off the root
      tvTree.Nodes.Add , , strTmpPath, StrDecode(strNodeName), "Folder Closed", "Folder Open"
    Else                            ' the node hangs off another node
      tvTree.Nodes.Add strNodeParent, tvwChild, strTmpPath, StrDecode(strNodeName), "Folder Closed", "Folder Open"
    End If
  Wend
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
