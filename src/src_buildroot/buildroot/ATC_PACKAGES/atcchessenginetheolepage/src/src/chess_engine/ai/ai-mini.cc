#include <algorithm>
#include <vector>
#include <cmath>

#include "ai-mini.hh"
#include "evaluation.hh"
#include "uci.hh"
#include "chess_engine/board/entity/color.hh"
#include "chess_engine/board/board.hh"
#include "utils/bits-utils.hh"

namespace ai
{
     using evalAndMove = std::pair<int16_t, std::optional<board::Move>>;

     class BasicDepthAdapter
     {
     public:
          BasicDepthAdapter() = delete;

          static int16_t adapte_depth(const board::Board& board,
                                        const int16_t depth)
          {
               const uint8_t board_value = get_basic_value(board);
               return updated_depth(board_value, depth);
          }
     private:
          // First basic board evaluation, just number of piece
          static int get_basic_value(const board::Board& board)
          {
               return utils::bits_count(board());
          }

          static int16_t updated_depth(const int board_value,
                                        const int16_t depth)
          {

               if (board_value > 20)
                    return depth;
               if (board_value > 12)
                    return depth + 1;
               return depth + 2;
          }
     };

     // Hardcoded depth adaptation
     // Based on the speed of our implementation
     // Increase depth based on the board
     static int16_t adapte_depth(const board::Board& board, const int16_t depth)
     {
          return BasicDepthAdapter::adapte_depth(board, depth);
     }

     static evalAndMove minimax(board::Chessboard& chessboard,
                                   int16_t depth,
                                   const int16_t depth_q,
                                   int16_t alpha,
                                   int16_t beta,
                                   const bool isMaxPlayer)
     {
          if (depth <= 0 || depth_q == 0)
               return evalAndMove(evaluate(chessboard), std::nullopt);

          const std::vector<board::Move> legal_moves =
                    chessboard.generate_legal_moves();

          bool is_check = chessboard.is_check();
          if (chessboard.is_draw(legal_moves, is_check))
               return evalAndMove(0, std::nullopt);
          if (chessboard.is_checkmate(legal_moves, is_check))
               return evalAndMove(isMaxPlayer ? INT16_MIN : INT16_MAX,
                                  std::nullopt);
          if (isMaxPlayer)
          {
               int16_t bestValue = INT16_MIN;
               size_t bestIndex = 0;
               for (size_t i = 0; i < legal_moves.size(); i++)
               {
                    board::Chessboard chessboard_ = chessboard;
                    chessboard_.do_move(legal_moves[i]);
                    int16_t eval;
                    if (depth <= 1 && (legal_moves[i].get_capture()
                                       || legal_moves[i].get_promotion()))
                    {
                         eval = minimax(chessboard_, depth, depth_q - 1,
                                        alpha, beta,
                                        !isMaxPlayer).first;
                    }
                    else
                    {
                         eval = minimax(chessboard_, depth - 1, depth_q,
                                        alpha, beta,
                                        !isMaxPlayer).first;
                    }
                    if (eval > bestValue)
                    {
                         bestValue = eval;
                         bestIndex = i;
                         alpha = std::max(alpha, eval);
                         if (beta <= alpha)
                              break;
                    }
               }
               return evalAndMove(bestValue, legal_moves[bestIndex]);
          }
          // else
          int16_t bestValue = INT16_MAX;
          size_t bestIndex = 0;
          for (size_t i = 0; i < legal_moves.size(); i++)
          {
               board::Chessboard chessboard_ = chessboard;
               chessboard_.do_move(legal_moves[i]);
               int16_t eval;
               if (depth <= 1 && (legal_moves[i].get_capture()
                                  || legal_moves[i].get_promotion()))
               {
                    eval = minimax(chessboard_, depth - 1, depth_q - 1,
                                   alpha, beta,
                                   !isMaxPlayer).first;
               }
               else
               {
                    eval = minimax(chessboard_, depth - 1, depth_q,
                                   alpha, beta,
                                   !isMaxPlayer).first;
               }
               if (eval < bestValue)
               {
                    bestValue = eval;
                    bestIndex = i;
                    beta = std::min(beta, eval);
                    if (beta <= alpha)
                         break;
               }
          }
          return evalAndMove(bestValue, legal_moves[bestIndex]);
     }

     std::optional<board::Move>  AiMini::search(board::Chessboard& chessboard,
                                int16_t depth) const
     {
          depth = adapte_depth(chessboard.get_board(), depth);
          auto eval_move = minimax(chessboard, depth, 6,
                                  INT16_MIN, INT16_MAX,
                                  chessboard.get_white_turn());
          uci::info(depth, eval_move.first);
          return eval_move.second;
     }
}
