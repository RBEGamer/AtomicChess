#include "ProcessWindows.hpp"
#include <atlstr.h>

namespace uciadapter {

void ProcessWindows::CreateChildProcess(std::string engine_path)
// Create a child process that uses the previously created pipes for STDIN and
// STDOUT.
{
  TCHAR szCmdline[1024];
  _tcscpy_s(szCmdline, CA2T(engine_path.c_str()));
  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  BOOL bSuccess = FALSE;

  // Set up members of the PROCESS_INFORMATION structure.

  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  // Set up members of the STARTUPINFO structure.
  // This structure specifies the STDIN and STDOUT handles for redirection.

  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = g_hChildStd_OUT_Wr;
  siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
  siStartInfo.hStdInput = g_hChildStd_IN_Rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  // Create the child process.

  bSuccess = CreateProcess(NULL,
                           szCmdline,    // command line
                           NULL,         // process security attributes
                           NULL,         // primary thread security attributes
                           TRUE,         // handles are inherited
                           0,            // creation flags
                           NULL,         // use parent's environment
                           NULL,         // use parent's current directory
                           &siStartInfo, // STARTUPINFO pointer
                           &piProcInfo); // receives PROCESS_INFORMATION

  // If an error occurs, exit the application.
  if (!bSuccess)
    throw FailedToStartEngine();
  else {
    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example.

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    // Close handles to the stdin and stdout pipes no longer needed by the child
    // process. If they are not explicitly closed, there is no way to recognize
    // that the child process has ended.

    CloseHandle(g_hChildStd_OUT_Wr);
    CloseHandle(g_hChildStd_IN_Rd);
  }
}

void ProcessWindows::ErrorExit(PTSTR lpszFunction)

// Format a readable error message, display a message box,
// and exit from the application.
{
  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                    FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR)&lpMsgBuf, 0, NULL);

  lpDisplayBuf =
      (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf) +
                                         lstrlen((LPCTSTR)lpszFunction) + 40) *
                                            sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf, LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                  TEXT("%s failed with error %d: %s"), lpszFunction, dw,
                  lpMsgBuf);
  MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
  ExitProcess(1);
}

ProcessWindows::ProcessWindows() {

  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;
  if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
    ErrorExit(TEXT("StdoutRd CreatePipe"));

  // Ensure the read handle to the pipe for STDOUT is not inherited.

  if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
    ErrorExit(TEXT("Stdout SetHandleInformation"));

  // Create a pipe for the child process's STDIN.

  if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
    ErrorExit(TEXT("Stdin CreatePipe"));

  // Ensure the write handle to the pipe for STDIN is not inherited.

  if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
    ErrorExit(TEXT("Stdin SetHandleInformation"));
}

void ProcessWindows::Kill() {}

void ProcessWindows::Start(std::string path) { CreateChildProcess(path); }

std::string ProcessWindows::ReadLine() {
  DWORD dwRead, dwWritten;
  CHAR chBuf[2014];
  BOOL bSuccess = FALSE;
  HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
  std::string line = "";
  for (;;) {
    CHAR c;
    bSuccess = ReadFile(g_hChildStd_OUT_Rd, &c, 1, &dwRead, NULL);
    line += c;
    if (!bSuccess || dwRead == 0)
      break;
    if (c == '\n')
      break;
  }
  return (line);
}

void ProcessWindows::Write(std::string data) {
  DWORD dwRead, dwWritten;
  CHAR chBuf[1024];
  BOOL bSuccess = FALSE;

  bSuccess =
      WriteFile(g_hChildStd_IN_Wr, data.c_str(), data.size(), &dwWritten, NULL);
}
} // namespace uciadapter