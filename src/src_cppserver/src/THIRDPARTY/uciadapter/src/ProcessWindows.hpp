#include "Process.hpp"
#include <chrono>

#include <stdio.h>
#include <strsafe.h>
#include <tchar.h>
#include <windows.h>

namespace uciadapter {

class ProcessWindows : public Process {
  HANDLE g_hChildStd_IN_Rd = NULL;
  HANDLE g_hChildStd_IN_Wr = NULL;
  HANDLE g_hChildStd_OUT_Rd = NULL;
  HANDLE g_hChildStd_OUT_Wr = NULL;
  SECURITY_ATTRIBUTES saAttr;
  void ProcessWindows::ErrorExit(PTSTR lpszFunction);
  void ProcessWindows::CreateChildProcess(std::string);

public:
  ProcessWindows();
  void Kill();
  void Start(std::string);
  std::string ReadLine();
  void Write(std::string);
};
}; // namespace uciadapter