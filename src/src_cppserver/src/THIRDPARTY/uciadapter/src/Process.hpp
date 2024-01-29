#include <string>
#define ENGINE_TIMEOUT 5 // In seconds
#define BUFFER_SIZE 1024

namespace uciadapter {

class Process {

public:
  /// @brief Kill the engine
  virtual void Kill() = 0;
  /// @brief Start the engine from file path
  virtual void Start(std::string) = 0;
  /// @brief Read one line from the stdout of the engine (could raise a
  /// ReadTimeoutExpire)
  virtual std::string ReadLine() = 0;
  /// @brief Write to engine stdin
  virtual void Write(std::string) = 0;
  virtual ~Process(){};
};

struct FailedToStartEngine : public std::exception {
  const char *what() const throw() { return ("Could not start the engine"); }
};
struct ReadTimeoutExpire : public std::exception {
  const char *what() const throw() { return ("Engine is not responding"); }
};

} // namespace uciadapter