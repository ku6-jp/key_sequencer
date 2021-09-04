# Key sequencerが使用するWin32 API一覧

動作可能なバージョンを定めるためにMicrosoft公式の「Minimum supported client」を調べたもの。<br>
(ただし、この参照した公式のマニュアルがかなり疑わしい)

Windows Vista 以降のAPIを使用しているのでWindows Vista 以降でのみ動作すると思われる。<br>
(動作を保証するものではない)

### Windows 2000 Professional 以降

- AdjustWindowRect
- CloseHandle
- CreateWindow
- CreateWindowEx
- DefWindowProc
- DestroyWindow
- DispatchMessage
- EnableWindow
- GetFileTitle
- GetMessage
- GetPrivateProfileInt
- GetPrivateProfileString
- GetSysColorBrush
- GetSystemMetrics
- LoadCursor
- LoadImage
- LoadString
- MapVirtualKey
- MessageBox
- midiInClose
- midiInGetNumDevs
- midiInOpen
- midiInReset
- midiInStart
- midiInStop
- midiOutClose
- midiOutOpen
- midiOutReset
- midiOutShortMsg
- PostMessage
- RegisterClassEx
- SendInput
- SetActiveWindow
- SetBkMode
- SetFocus
- SetWindowLongPtr
- SetWindowText
- ShowWindow
- timeBeginPeriod
- timeGetDevCaps
- timeGetTime
- UnregisterHotKey
- WritePrivateProfileString

### Windows XP 以降

- CreateEvent
- CreateFile
- CreateMutex
- DeleteCriticalSection
- DragAcceptFiles
- DragFinish
- DragQueryFile
- GetFileSizeEx
- GetLastError
- GetModuleFileName
- ImmAssociateContext
- InitializeCriticalSection
- PathFileExists
- PathRenameExtension
- ReadFile
- SetEvent
- WaitForSingleObject

### Windows Vista 以降

- RegisterHotKey
- ScrollWindowEx
- SetScrollInfo

