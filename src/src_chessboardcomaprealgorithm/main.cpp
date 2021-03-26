#include <iostream>

#define  BOARD_WIDTH (8+4)
#define BOARD_HEIGHT 8

enum class BOARD_TPYE
{
    REAL_BOARD = 0,
    TARGET_BOARD = 1,
    TEMP_BOARD = 2
};

struct FigureField
{
    ChessField::CHESS_FILEDS field;
    ChessPiece::FIGURE figure;
    FigureField()
    {
    }
    FigureField(ChessField::CHESS_FILEDS _field, ChessPiece::FIGURE _figure)
    {
        field = _field;
        figure = _figure;
    }
};



int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
