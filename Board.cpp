#include "Board.h"
#include "Pieces.h"
#include <algorithm>

Board::Board() {
    grid.resize(8, std::vector<Piece*>(8, nullptr));
}

Board::Board(const Board& other) {
    grid.resize(8, std::vector<Piece*>(8, nullptr));
    copyFrom(other);
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        freeMemory();
        copyFrom(other);
    }
    return *this;
}

Board::~Board() {
    freeMemory();
}

void Board::freeMemory() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (grid[r][c] != nullptr) {
                delete grid[r][c];
                grid[r][c] = nullptr;
            }
        }
    }
}

void Board::copyFrom(const Board& other) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = other.grid[r][c];
            if (p != nullptr) {
                switch (p->getType()) {
                    case PieceType::PAWN:
                        grid[r][c] = new Pawn(p->getColor(), p->getPosition());
                        break;
                    case PieceType::ROOK:
                        grid[r][c] = new Rook(p->getColor(), p->getPosition());
                        break;
                    case PieceType::KNIGHT:
                        grid[r][c] = new Knight(p->getColor(), p->getPosition());
                        break;
                    case PieceType::BISHOP:
                        grid[r][c] = new Bishop(p->getColor(), p->getPosition());
                        break;
                    case PieceType::QUEEN:
                        grid[r][c] = new Queen(p->getColor(), p->getPosition());
                        break;
                    case PieceType::KING:
                        grid[r][c] = new King(p->getColor(), p->getPosition());
                        break;
                }
            } else {
                grid[r][c] = nullptr;
            }
        }
    }
}

void Board::clear() {
    freeMemory();
}

void Board::initialize() {
    clear();

    // Initialize Pawns (Tot)
    for (int col = 0; col < 8; ++col) {
        grid[1][col] = new Pawn(Color::WHITE, Position(1, col));
        grid[6][col] = new Pawn(Color::BLACK, Position(6, col));
    }

    // Initialize White Rooks, Knights, Bishops, Queen, King
    grid[0][0] = new Rook(Color::WHITE, Position(0, 0));
    grid[0][1] = new Knight(Color::WHITE, Position(0, 1));
    grid[0][2] = new Bishop(Color::WHITE, Position(0, 2));
    grid[0][3] = new Queen(Color::WHITE, Position(0, 3)); // White Queen on D1
    grid[0][4] = new King(Color::WHITE, Position(0, 4));  // White King on E1
    grid[0][5] = new Bishop(Color::WHITE, Position(0, 5));
    grid[0][6] = new Knight(Color::WHITE, Position(0, 6));
    grid[0][7] = new Rook(Color::WHITE, Position(0, 7));

    // Initialize Black Rooks, Knights, Bishops, Queen, King
    grid[7][0] = new Rook(Color::BLACK, Position(7, 0));
    grid[7][1] = new Knight(Color::BLACK, Position(7, 1));
    grid[7][2] = new Bishop(Color::BLACK, Position(7, 2));
    grid[7][3] = new Queen(Color::BLACK, Position(7, 3)); // Black Queen on D8
    grid[7][4] = new King(Color::BLACK, Position(7, 4));  // Black King on E8
    grid[7][5] = new Bishop(Color::BLACK, Position(7, 5));
    grid[7][6] = new Knight(Color::BLACK, Position(7, 6));
    grid[7][7] = new Rook(Color::BLACK, Position(7, 7));
}

bool Board::isValidPosition(const Position& pos) {
    return pos.isValid();
}

Piece* Board::getPiece(const Position& pos) const {
    if (!pos.isValid()) return nullptr;
    return grid[pos.row][pos.col];
}

Piece* Board::getPiece(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) return nullptr;
    return grid[row][col];
}

void Board::setPiece(const Position& pos, Piece* piece) {
    if (!pos.isValid()) return;
    if (grid[pos.row][pos.col] != nullptr && grid[pos.row][pos.col] != piece) {
        delete grid[pos.row][pos.col];
    }
    grid[pos.row][pos.col] = piece;
    if (piece != nullptr) {
        piece->setPosition(pos);
    }
}

void Board::executeMove(const Position& start, const Position& end) {
    if (!start.isValid() || !end.isValid()) return;

    Piece* movingPiece = grid[start.row][start.col];
    if (movingPiece == nullptr) return;

    // Delete enemy piece at destination if any
    if (grid[end.row][end.col] != nullptr) {
        delete grid[end.row][end.col];
        grid[end.row][end.col] = nullptr;
    }

    // Move piece to end
    grid[end.row][end.col] = movingPiece;
    movingPiece->setPosition(end);

    // Empty start square
    grid[start.row][start.col] = nullptr;

    // Standard pawn promotion: Auto promote to Queen if it reaches the end row
    if (movingPiece->getType() == PieceType::PAWN) {
        if ((movingPiece->getColor() == Color::WHITE && end.row == 7) ||
            (movingPiece->getColor() == Color::BLACK && end.row == 0)) {
            Color color = movingPiece->getColor();
            delete grid[end.row][end.col];
            grid[end.row][end.col] = new Queen(color, end);
        }
    }
}

bool Board::isPathClear(const Position& start, const Position& end) const {
    int dRow = end.row - start.row;
    int dCol = end.col - start.col;

    int stepRow = (dRow == 0) ? 0 : (dRow > 0 ? 1 : -1);
    int stepCol = (dCol == 0) ? 0 : (dCol > 0 ? 1 : -1);

    int r = start.row + stepRow;
    int c = start.col + stepCol;

    while (r != end.row || c != end.col) {
        if (grid[r][c] != nullptr) {
            return false;
        }
        r += stepRow;
        c += stepCol;
    }

    return true;
}
