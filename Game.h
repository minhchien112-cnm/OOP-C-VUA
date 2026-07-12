#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include <string>
#include <vector>


class Game {
private:
    Board board;                        // Ban co
    Color currentTurn;                  // Luot hien tai (TRANG hoac DEN)
    std::vector<std::string> moveHistory; // Lich su cac nuoc di

public:
    Game();


    void reset();


    const Board& getBoard() const { return board; }
    Color getCurrentTurn() const { return currentTurn; }
    const std::vector<std::string>& getMoveHistory() const { return moveHistory; }


    void playMove(const Position& start, const Position& end);


    bool isKingCaptured(Color kingColor) const;


    void switchTurn();


    void saveToFile(const std::string& filepath) const;
    void loadFromFile(const std::string& filepath);
};

#endif // GAME_H
