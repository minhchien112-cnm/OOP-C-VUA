#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "Piece.h"


class Board {
private:
    std::vector<std::vector<Piece*>> grid;

    void copyFrom(const Board& other);
    void freeMemory();

public:
    Board();
    Board(const Board& other);
    Board& operator=(const Board& other);
    ~Board();


    void initialize();


    void clear();

    bool isValidPosition(const Position& pos);


    Piece* getPiece(const Position& pos) const;
    Piece* getPiece(int row, int col) const;


    void setPiece(const Position& pos, Piece* piece);


    void executeMove(const Position& start, const Position& end);


    bool isPathClear(const Position& start, const Position& end) const;

    const std::vector<std::vector<Piece*>>& getGrid() const { return grid; }
};

#endif // BOARD_H
