#ifndef __DISKUTIL_H_
#define __DISKUTIL_H_

class CSequentialFile {
  public:
    CSequentialFile(void);
    ~CSequentialFile(void);

  public:
    BOOL Create(LPCTSTR szPrefix, LPCTSTR szExtension, DWORD dwDesiredAccess = GENERIC_WRITE, DWORD dwShareMode = FILE_SHARE_DELETE, DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL);
    BOOL Truncate(DWORD newSize = 0);
    BOOL Seek(LONG lDistanceToMove, DWORD dwMoveMethod = FILE_CURRENT);
    DWORD Write(LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite);
    BOOL Close(void);

    BOOL IsOpen(void);
    CString GetFileName(void);

  protected:
    UINT m_lastSequence;
    HANDLE m_hFile;
    CString m_strFileName;
};

#endif //__DISKUTIL_H_
