
#ifndef GameController_hpp
#define GameController_hpp

#include "ItemClass.h"
using namespace sf;
/*
* 
* Here's the main class of the game. Drawing sprites, clicking on things
* 
*/



enum GameStatus {
    NotStarted,
    Started,
    Won,
    Failed
};

class GameController {

public :
    GameController();
    ~GameController();
    void startGame();

private:

    void updateGameStatus(const GameStatus& status);
    GameStatus getGameStatus();
    void readJsonOptions();//reads startup options from pc

    bool checkForMatches(bool playerCaused = 0);//returns if any matches were found and we have to create new gems for empty spaces
    bool checkForMatches(board_t& boardState, bool playerCaused = 0);//same as checkForMatches() but after a swap

    void refillEmptySpaces();   //checking items vector

    bool swapCheck(ItemClass* item, bool playerCaused = 0);//check if swapping gems is allowed and if the swap leads to completing objectives
    void drawInTheMiddleOfTheScreen(const std::string& string);
    bool checkIfNoPossibleMovesAreLeft();
    void bombPop(int column, int row);
    bool checkVH(board_t& boardState, bool playerCaused = 0);    //checks if gems are formed into lines of >3
    bool checkCustom(board_t& boardState, bool playerCaused = 0);//checks if gems are formed into the custom shape;
    bool checkIfGameFinished();
    void drawObjectives();//returns 1 if game should be continued and 0 if it's over;
    void run();
    bool checkIfShapeOnBoard(board_t& shape, board_t& board, bool playerCaused, int bombType = BOMB_ROUND, bool justChecking = false);

private:
    GameStatus _gameStatus = NotStarted;
    RenderWindow* _app;
    ResourceList::GameOptions jsonOptions;
    float windowH, windowW;
    std::vector<std::vector<ItemClass*>> items; //2d matrix of items on the board

};
#endif /* GameController_hpp */