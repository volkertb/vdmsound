Attribute VB_Name = "modIniFile"
'
' Win API Declares
'
Private Declare Function WritePrivateProfileString _
  Lib "kernel32" Alias "WritePrivateProfileStringA" ( _
    ByVal lpAppName As String, _
    ByVal lpKeyName As String, _
    ByVal lpString As String, _
    ByVal lpFileName As String _
  ) As Long

Private Declare Function GetPrivateProfileString _
  Lib "kernel32" Alias "GetPrivateProfileStringA" ( _
    ByVal lpAppName As String, _
    ByVal lpKeyName As String, _
    ByVal lpDefault As String, _
    ByVal lpReturnedString As String, _
    ByVal nSize As Long, _
    ByVal lpFileName As String _
  ) As Long

Private Declare Function WritePrivateProfileSection _
  Lib "kernel32" Alias "WritePrivateProfileSectionA" ( _
    ByVal lpAppName As String, _
    ByVal lpString As String, _
    ByVal lpFileName As String _
  ) As Long

Private Declare Function GetPrivateProfileSection _
  Lib "kernel32" Alias "GetPrivateProfileSectionA" ( _
    ByVal lpAppName As String, _
    ByVal lpReturnedString As String, _
    ByVal nSize As Long, _
    ByVal lpFileName As String _
  ) As Long

'
' Creates a full path name given a directory name and a file name.
'  Takes care of any trailing '\'s in the directory name and of
'  missing .ini extensions.
'
Public Function MakeIniPathStr( _
  ByVal strDir As String, _
  ByVal strFile As String _
) As String

  ' First make sure the directory finishes with a '\'
  If Right$(strDir, 1) <> "\" Then
    strDir = strDir & "\"
  End If

  ' Then make sure the directory finishes with a '.ini'
  If Right$(strFile, 4) <> ".ini" Then
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
Public Function EnsurePathExists( _
  ByVal strPath As String _
) As Long

  Dim strTmpPath As String  ' holds the path we know exists; as we check for file
                            '  existence, strPath gets transferred into strTmpPath
  Dim splitPos As Long

  On Error GoTo Failure

  If Left$(strPath, 2) = "\\" Then  ' take care of UNC paths
    splitPos = InStr(3, strPath, "\")
    strTmpPath = Left$(strPath, splitPos)
    strPath = Mid$(strPath, splitPos + 1)
  Else
    strTmpPath = ""
  End If

  While Len(strPath) > 0
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

  EnsurePathExists = 1
  Exit Function

Failure:
  EnsurePathExists = 0
End Function

'
' Writes the given key/value pair in the given ini file under
'  the given section.  If the section iside the file does not
'  exist, then it is automatically created.  The file must exist,
'  otherwise an error may occur
'
Public Sub WriteIniString( _
  strIni As String, _
  strSection As String, _
  strKey As String, _
  strValue As String _
)

  ' Write the key/value pair to strIni
  If WritePrivateProfileString(strSection, strKey, strValue, strIni) = 0 Then
    Err.Raise Err.LastDllError, "WriteIniString", StrError(Err.LastDllError)  ' throw
  End If
End Sub

'
' Reads the value associated with the given key from the given
'  ini file from under the given section.  If the key does not
'  exist, return an empty string
'
Public Function ReadIniString( _
  strIni As String, _
  strSection As String, _
  strKey As String _
) As String

  Dim strTmp As String
  strTmp = String$(4096, 0)

  Dim lngRet As Long

  ' Read the key/value pair from strIni
  lngRet = GetPrivateProfileString(strSection, strKey, "", strTmp, Len(strTmp), strIni)

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
Public Sub DeleteIniString( _
  strIni As String, _
  strSection As String, _
  strKey As String _
)

  ' To delete the key just pass NULL as the value
  If WritePrivateProfileString(strSection, strKey, vbNullString, strIni) = 0 Then
    Err.Raise Err.LastDllError, "DeleteIniString", StrError(Err.LastDllError)  ' throw
  End If
End Sub

'
' Renames the given key
'
Public Sub RenameIniString( _
  strIni As String, _
  strSection As String, _
  strOldKey As String, _
  strNewKey As String _
)

  If LCase$(strOldKey) = LCase$(strNewKey) Then
    Exit Sub  ' return, old and new names are identical
  End If

  Dim strValue As String
  strValue = String$(4096, 0)

  ' Get the value
  GetPrivateProfileString strSection, strOldKey, "", strValue, Len(strValue), strIni

  ' Create the new key
  If WritePrivateProfileString(strSection, strNewKey, strValue, strIni) Then
    Err.Raise Err.LastDllError, "RenameIniString", StrError(Err.LastDllError)  ' throw
  End If

  ' Delete the old key
  If WritePrivateProfileString(strSection, strOldKey, vbNullString, strIni) Then
    Err.Raise Err.LastDllError, "RenameIniString", StrError(Err.LastDllError)  ' throw
  End If
End Sub

'
' Deletes a section in the given ini file
'
Public Sub DeleteIniSection( _
  strIni As String, _
  strSection As String _
)

  ' To delete the section just pass NULL as the value
  If WritePrivateProfileSection(strSection, vbNullString, strIni) = 0 Then
    Err.Raise Err.LastDllError, "DeleteIniSection", StrError(Err.LastDllError)  ' throw
  End If
End Sub

'
' Reads the contents of a section from the given ini file.
'  If the section does not exist, return an empty string.
'
Public Function ReadIniSection( _
  strIni As String, _
  strSection As String _
) As String

  Dim strKeys As String
  strKeys = String$(32768, 0)

  Dim lngRet As Long

  ' Get the keys from the old section
  lngRet = GetPrivateProfileSection(strSection, strKeys, Len(strKeys), strIni)

  ' Trim trailing '\0's
  strKeys = Left$(strKeys, lngRet)

  ReadIniSection = strKeys  ' return
End Function

'
' Renames a section in the given ini file
'
Public Sub RenameIniSection( _
  strIni As String, _
  strOldSection As String, _
  strNewSection As String _
)

  If LCase$(strOldSection) = LCase$(strNewSection) Then
    Exit Sub  ' return, old and new names are identical
  End If

  Dim strKeys As String
  strKeys = String$(32768, 0)

  ' Get the keys from the old section
  GetPrivateProfileSection strOldSection, strKeys, Len(strKeys), strIni

  ' Create the new section
  If WritePrivateProfileSection(strNewSection, strKeys, strIni) = 0 Then
    Err.Raise Err.LastDllError, "RenameIniSection", StrError(Err.LastDllError)  ' throw
  End If

  ' Delete the old section
  If WritePrivateProfileSection(strOldSection, vbNullString, strIni) = 0 Then
    Err.Raise Err.LastDllError, "RenameIniSection", StrError(Err.LastDllError)  ' throw
  End If
End Sub
