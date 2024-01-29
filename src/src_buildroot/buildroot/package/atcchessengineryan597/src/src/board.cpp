#include "board.hpp"

auto make_type_ptr(int i, char col, char file, char rank)
    -> std::unique_ptr<Piece> {
  switch (i) {
  case 0:
    return std::make_unique<Rook>(col, Square{file, rank});
  case 1:
    return std::make_unique<Knight>(col, Square{file, rank});
  case 2:
    return std::make_unique<Bishop>(col, Square{file, rank});
  case 3:
    return std::make_unique<Queen>(col, Square{file, rank});
  case 4:
    return std::make_unique<King>(col, Square{file, rank});
  case 5:
    return std::make_unique<Bishop>(col, Square{file, rank});
  case 6:
    return std::make_unique<Knight>(col, Square{file, rank});
  case 7:
    return std::make_unique<Rook>(col, Square{file, rank});
  default:
    std::cout << "Error, not a piece, exiting.";
    exit(1);
  }
}

Board::Board() {
  m_turn = 'w';
  // FEN - position, turn, castling rights, halfmove counter (for 50 move rule),
  // fullmove counter
  m_current_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

  const std::vector<char> back_rank{'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R'};
  m_pieces.reserve(32);

  for (std::size_t i = 0; i < back_rank.size(); i++) {
    char file = char('a' + i);
    m_pieces.emplace_back(std::make_unique<Pawn>('w', Square{file, '2'}));
    m_pieces.emplace_back(make_type_ptr(i, 'w', file, '1'));
    m_pieces.emplace_back(std::make_unique<Pawn>('b', Square{file, '7'}));
    m_pieces.emplace_back(make_type_ptr(i, 'b', file, '8'));
  }
}

Board::Board(int variant) {
  m_turn = 'w';

  switch (variant) {
  case 0:
    Board();
    break;
  case 960:
    // Fischer random - Chess960
    // Black will mirror White's randomised pieces
    // King must be between the two rooks so that castling both sides is valid
    // otherwise, randomise the backrank
    m_turn = 'w';
    // FEN - position, turn, castling rights, halfmove counter (for 50 move
    // rule), fullmove counter

    std::vector<char> back_rank{'R', 'N', 'B', 'K', 'Q', 'B', 'N', 'R'};
    std::shuffle(back_rank.begin(), back_rank.end(), std::random_device());
    m_pieces.reserve(32);

    for (std::size_t i = 0; i < back_rank.size(); i++) {
      char file = char('a' + i);
      m_pieces.emplace_back(std::make_unique<Pawn>('w', Square{file, '2'}));
      m_pieces.emplace_back(make_type_ptr(i, 'w', file, '1'));
      m_pieces.emplace_back(std::make_unique<Pawn>('b', Square{file, '7'}));
      m_pieces.emplace_back(make_type_ptr(i, 'b', file, '8'));
    }
  }
}

auto Board::get_fen() -> std::string {
  // needs to generate the fen from pieces
  return m_current_fen;
}

auto Board::set_fen(const std::string &fen_string) -> bool {
  if (check_fen_is_valid(fen_string)) {
    m_current_fen = fen_string;
    return true;
  }
  std::cout << "FEN is not valid.\n";
  return false;
}

auto Board::check_fen_is_valid(const std::string &fen) -> bool {
  // check the formatting is correct
  return fen == "todo";
}

auto Board::print_fen() -> void { std::cout << get_fen(); }

// This will print ASCII position from a given FEN, doesnt require an instance
// of the class
auto Board::print_position(std::string &fen) -> void {
  // printing from the FEN
  std::vector<char> pieces = {'p', 'r', 'n', 'b', 'q', 'k',
                              'P', 'R', 'N', 'B', 'Q', 'K'};
  std::string asci_position{};
  for (auto &i : fen) {
    if (std::find(pieces.begin(), pieces.end(), i) != std::end(pieces)) {
      asci_position += i;
    } else if (isdigit(i)) {
      asci_position += std::string(i - '0', '.');
    } else if (i == '/') {
      asci_position += '\n';
    } else if (i == ' ') {
      std::cout << asci_position << '\n' << "";
      return;
    }
  }
}

auto Board::print() -> void {
  std::vector<char> board_position;
  board_position.resize(64);
  std::fill(board_position.begin(), board_position.end(), '.');

  for (auto &piece : m_pieces) {
    auto file = piece->get_file();
    auto rank = piece->get_rank();
    // when printing we start at the top left of the board (a8), so rank=7 (0
    // index for the array) and file=0 (0 indexed for the array)
    int current_square = 64 - rank * 8 + file - 8;
    board_position.at(current_square) = piece->get_type();
    if (piece->get_colour() == 'b') {
      board_position.at(current_square) += 32; // convert to lowercase
    }
  }

  for (int i = 0; i < 64; i++) {
    std::cout << board_position.at(i);
    if ((i + 1) % 8 == 0) {
      std::cout << '\n';
    }
  }
}

auto Board::is_legal_move(const std::string &notation) -> bool {
  std::vector<std::string> legal_moves;
  for (auto &piece : m_pieces) {
    auto temp_moves = piece->generate_legal_moves(*this);
    for (auto move : temp_moves) {
      legal_moves.emplace_back(move);
    }
  }
  // TODO: check if two pieces can move to the same square
  // eg. R on d1 and R on d8 can both move to d5, so the moves must be
  // denoted as R1d5 and R8d5 respectively

  return true;
}

auto Board::move(const std::string &notation) -> bool {

  if (is_legal_move(notation)) {
    parse_move(notation);
    return true;
  }
  return false;
}

auto Board::parse_move(const std::string &move) -> void {
  // Square src;
  Square dst;
  int pad{};
  if (move.at(move.size() - 1) == '+' || move.at(move.size() - 1) == '#') {
    pad++;
  }
  dst.file = move.at(move.size() - 2 - pad);
  dst.rank = move.at(move.size() - 1 - pad);

  char piece = move[0];
  if (piece >= 'a') { // pawn move denoted by lower case letter eg. dxe5
    for (auto &i : m_pieces) {
      if (i->get_type() == 'P' && i->get_colour() == m_turn) {
        i->move(dst);
        return;
      }
    }
  } else { // piece move denoted by uppercase letter R,N,B,Q,K
  }
}

auto Board::get_pieces() -> std::vector<std::unique_ptr<Piece>> & {
  return m_pieces;
}