#include <iostream>
#include <string>
#include "UCI.hpp"

using namespace uciadapter;
using namespace std;

int main() {

  UCI u("/home/loic/Stockfish-sf_14.1/src/stockfish");

  std::cout << u.GetBuffer() << std::endl;

  return (0);
}
