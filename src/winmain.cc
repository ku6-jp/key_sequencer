#include <Windows.h>
#include <tchar.h>

#include <memory>

#include "constants.h"
#include "key_sequencer.h"
#include "scope_exit_mutex.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);

  // 2重起動はさせない(ホットキーを使うのでややこしくなる)
  HANDLE mutex_handle = CreateMutex(NULL, TRUE, _T("Key sequencer"));
  if (mutex_handle == nullptr) {
    return 11;
  } else if (GetLastError() == ERROR_ALREADY_EXISTS) {
    CloseHandle(mutex_handle);
    return 12;
  }
  keysequencer::ScopeExitMutex scope_exit_mutex(mutex_handle);

  std::unique_ptr<keysequencer::KeySequencer> key_sequencer(new keysequencer::KeySequencer(hInstance));
  return key_sequencer->Execute();
}
