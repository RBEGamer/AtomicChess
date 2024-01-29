#include "Process.hpp"
#include <chrono>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

namespace uciadapter {

class ProcessLinux : public Process {
  /// @brief Create pipe to engine stdin
  int in_fd[2];
  /// @brief Create pipe to the engine stdout
  int out_fd[2];
  /// @brief Writing buffer
  char buffer[BUFFER_SIZE];
  /// @brief Contains engine pid
  pid_t pid;

public:
  ProcessLinux();
  void Kill();
  void Start(std::string);
  std::string ReadLine();
  void Write(std::string);
};
}; // namespace uciadapter