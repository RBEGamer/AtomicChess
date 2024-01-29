#include "ProcessLinux.hpp"

namespace uciadapter {

ProcessLinux::ProcessLinux() {

  pipe(in_fd);
  pipe(out_fd);
}

void ProcessLinux::Kill() { kill(pid, SIGTERM); }

void ProcessLinux::Start(std::string path) {
  pid = fork();
  if (pid == 0) {
    // Connect output of child process
    close(out_fd[0]);
    dup2(out_fd[1], STDOUT_FILENO);
    close(out_fd[1]);
    // Connect input of child process
    close(in_fd[1]);
    dup2(in_fd[0], STDIN_FILENO);
    close(in_fd[0]);

    const char *args[] = {path.c_str(), NULL};
    execvp(args[0], const_cast<char *const *>(args));

    _exit(1);
  } else if (pid < 0) {
    throw FailedToStartEngine();
  }

  // Parent do not read the in of the child
  close(in_fd[0]);
  // The parent do not write on the out of the child
  close(out_fd[1]);
  //  Set descriptor to non-blocking (for the read syscall)
  int flags = fcntl(out_fd[0], F_GETFL, 0);
  fcntl(out_fd[0], F_SETFL, flags | O_NONBLOCK);
}

std::string ProcessLinux::ReadLine() {
  std::string line;
  auto start = std::chrono::system_clock::now();
  // Read char by char
  while (true) {
    char c;
    int status = read(out_fd[0], &c, 1);
    if (status > 0) {
      line += c;
      if (c == '\n')
        break;
    } else {
      // Check for timeout
      auto end = std::chrono::system_clock::now();
      auto elapsed =
          std::chrono::duration_cast<std::chrono::seconds>(end - start);
      if (elapsed.count() > ENGINE_TIMEOUT) {
        throw ReadTimeoutExpire();
      }
    }
  }
  return (std::string(line));
}

void ProcessLinux::Write(std::string data) {
  for (unsigned int i = 0; i < data.size(); i++) {
    buffer[i] = data[i];
  }
  write(in_fd[1], buffer, data.size());
}
} // namespace uciadapter