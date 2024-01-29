#include "board.cpp"
#include "player.cpp"

auto main(int argc, char **argv) -> int {

  Player player1{"White", 1500.0, 'w'};
  Player player2{"Black", 1600.0, 'b'};

  Board board{};

  board.print();

  board.move("d4");
  board.move("d5");
  auto &vec = board.get_pieces();
  auto moves = vec[1]->generate_legal_moves(board);
  for (auto i : moves) {
    std::cout << i << '\n';
  }

  board.print();
}