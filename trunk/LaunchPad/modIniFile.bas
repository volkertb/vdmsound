Attribute VB_Name = "modIniFile"
'
' Win API Declares
'
Private Declare Function WritePrivateProfileString _
  Lib "kernel32" Alias "WritePrivateProfileStringA" _
 (ByVal lpAppName As String, _
  ByVal lpKeyName As String, _
  ByVal lpString As String, _
  ByVal lpFileName As String) As Long

Private Declare Function GetPrivateProfileString _
  Lib "kernel32" Alias "GetPrivateProfileStringA" _
 (ByVal lpAppName As String, _
  ByVal lpKeyName As String, _
  ByVal lpDefault As String, _
  ByVal lpReturnedString As String, _
  ByVal nSize As Long, _
  ByVal lpFileName As String) As Long

Private Declare Function WritePrivateProfileSection _
  Lib "kernel32" Alias "WritePrivateProfileSectionA" _
 (ByVal lpAppName As String, _
  ByVal lpString As String, _
  ByVal lpFileName As String) As Long

Private Declare Function GetPrivateProfileSection _
  Lib "kernel32" Alias "GetPrivateProfileSectionA" _
 (ByVal lpAppName As String, _
  ByVal lpReturnedString As String, _
  ByVal nSize As Long, _
  ByVal lpFileName As String) As Long

'
' Creates a full path name given a directory name and a file name.
'  Takes care of any trailing '\'s in the directory name and of
'  missing .ini extensions.
'
Public Function MakeIniPathStr(ByVal strDir As String, _
  ByVal strFile As String) As String

  ' First make sure the directory finishes with a '\'
  If Right(strDir, 1) <> "\" Then
    strDir = strDir & "\"
  End If

  ' Then make sure the directory finishes with a '.ini'
  If Right(strFile, 4) <> ".ini" Then
    strFile = strFile & ".ini"
  End If

  ' Return the path
  MakeIniPathStr = strDir & strFile   ' return
End Function

'
' Verifies whether a file (and directory structure leading to
'  it) exists, and if it does not exist then the subroutine
'  creates it
'
Public Sub EnsurePathExists(ByVal strPath As String)
  Dim strTmpPath As String  ' holds the path we know exists; as we check for file
                            '  existence, strPath gets transferred into strTmpPath
  strTmpPath = ""

  While Len(strPath) > 0
    Dim splitPos As Long
    splitPos = InStr(1, strPath, "\")

    If splitPos > 0 Then    ' we found a '\', so whatever comes before is a directory name
      strTmpPath = strTmpPath + Left$(strPath, splitPos)
      strPath = Mid$(strPath, splitPos + 1)
      
      ' check if the DIRECTORY in strTmpPath exists, create it otherwise
      If Len(Dir(strTmpPath, vbNormal Or vbReadOnly Or vbHidden Or vbSystem Or vbDirectory)) = 0 Then
        MkDir strTmpPath
      End If
    Else                    ' did not find a '\', so we deal with a file name
      strTmpPath = strTmpPath + strPath
      strPath = ""
    
      ' check if the FILE in strTmpPath exists, create it otherwise
      If Len(Dir(strTmpPath, vbNormal Or vbReadOnly Or vbHidden Or vbSystem)) = 0 Then
        Dim hFile As Integer
        hFile = FreeFile
        Open strTmpPath For Output As hFile
        Close hFile
      End If
    End If
  Wend
End Sub

'
' Writes the given key/value pair in the given ini file under
'  the given section.  If the section iside the file does not
'  exist, then it is automatically created.  The file must exist,
'  otherwise an error may occur
'
Public Sub WriteIniString(strIni As String, _
  strSection As String, _
  strKey As String, _
  strValue As String)

  ' Write the key/value pair to strIni
  WritePrivateProfileString strSection, _
    strKey, strValue, strIni
End Sub

'
' Reads the value associated with the given key from the given
'  ini file from under the given section.  If the key does not
'  exist, return an empty string
'
Public Function ReadIniString(strIni As String, _
  strSection As String, _
  strKey As String) As String

  Dim strTmp As String
  strTmp = String$(1024, 0)

  Dim lngRet As String

  ' Read the key/value pair from strIni
  lngRet = GetPrivateProfileString(strSection, _
    strKey, "", strTmp, _
    Len(strTmp), strIni)

  ' Trim trailing '\0's
  strTmp = Left$(strTmp, lngRet)

  ' Trim comments
  Dim splitPos As Long
  splitPos = InStr(1, strTmp, ";")

  If (splitPos > 0) Then
    strTmp = Left$(strTmp, splitPos - 1)
  End If

  ReadIniString = strTmp  ' return
End Function

'
' Deletes any reference to the given key from under
'  the given section in the given ini file
'
Public Sub DeleteIniString(strIni As String, _
  strSection As String, _
  strKey As String)

  ' To delete the key just pass NULL as the value
  WritePrivateProfileString strSection, _
    strKey, vbNullString, strIni
End Sub

'
' Renames the given key
'
Public Sub RenameIniString(strIni As String, _
  strSection As String, _
  strOldKey As String, _
  strNewKey As String)

  If LCase(strOldKey) = LCase(strNewKey) Then
    Exit Sub  ' return, old and new names are identical
  End If

  Dim strValue As String
  strValue = String$(4096, 0)

  ' Get the value
  GetPrivateProfileString strSection, strOldKey, "", _
    strValue, Len(strValue), strIni

  ' Create the new key
  WritePrivateProfileString strSection, _
    strNewKey, strValue, strIni

  ' Delete the old key
  WritePrivateProfileString strSection, _
    strOldKey, vbNullString, strIni
End Sub

'
' Deletes a section in the given ini file
'
Public Sub DeleteIniSection(strIni As String, _
  strSection As String)

  ' To delete the section just pass NULL as the value
  WritePrivateProfileSection strSection, vbNullString, strIni
End Sub

'
' Renames a section in the given ini file
'
Public Sub RenameIniSection(strIni As String, _
  strOldSection As String, _
  strNewSection As String)

  If LCase(strOldSection) = LCase(strNewSection) Then
    Exit Sub  ' return, old and new names are identical
  End If

  Dim strKeys As String
  strKeys = String$(32768, 0)

  ' Get the keys from the old section
  GetPrivateProfileSection strOldSection, strKeys, Len(strKeys), strIni

  ' Create the new section
  WritePrivateProfileSection strNewSection, strKeys, strIni

  ' Delete the old section
  WritePrivateProfileSection strOldSection, vbNullString, strIni
End Sub
