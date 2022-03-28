#include "GameController.hpp"
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")//hide console

int main() {
    GameController* gc = new GameController();
    gc->startGame();
    return 0;
}
