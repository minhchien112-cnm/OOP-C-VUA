#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "Game.h"
#include "GameView.h"


class GameController {
private:
    Game game;
    GameView view;
    bool isRunning;


    void gameLoop();


    bool parseMove(const std::string& input, Position& start, Position& end) const;


    void waitForEnter() const;

public:
    GameController();


    void run();
};

#endif // GAMECONTROLLER_H
