Attribute VB_Name = "modError"
'
' Win API Declares
'
Private Declare Function FormatMessage _
  Lib "kernel32" Alias "FormatMessageA" _
 (ByVal dwFlags As Long, _
  ByVal dwFlags As String, _
  ByVal dwMessageId As Long, _
  ByVal dwLanguageId As Long, _
  ByVal lpBuffer As String, _
  ByVal nSize As Long, _
  ByVal Arguments As Long) As Long

'
' WinApi constants
'
Const FORMAT_MESSAGE_ALLOCATE_BUFFER = &H100
Const FORMAT_MESSAGE_IGNORE_INSERTS = &H200
Const FORMAT_MESSAGE_FROM_STRING = &H400
Const FORMAT_MESSAGE_FROM_HMODULE = &H800
Const FORMAT_MESSAGE_FROM_SYSTEM = &H1000
Const FORMAT_MESSAGE_ARGUMENT_ARRAY = &H2000
Const FORMAT_MESSAGE_MAX_WIDTH_MASK = &HFF

'
' Reports an error
'
Public Sub ReportError(errMsg As String)
  MsgBox Err.Source & ":" & Chr$(13) & errMsg & Chr$(13) & "Error " & Err.Number & " - " & Err.Description, vbOKOnly + vbCritical, "VDMSound LaunchPad - Error"
End Sub

'
' Converts a Win32 error message into a descriptive string
'
Public Function StrError(ByVal ErrorID As Long) As String
  Dim strTmp As String
  strTmp = String$(1024, 0)

  Dim lngRet As Long

  lngRet = FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS + FORMAT_MESSAGE_FROM_SYSTEM, vbNullString, ErrorID, 0, strTmp, Len(strTmp), 0)

  StrError = Left$(strTmp, lngRet)
End Function
