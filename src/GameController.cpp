#include "GameController.hpp"

#include <iostream>
//--------------------------------------------------------------------------------
GameController::GameController() 
{
    updateGameStatus(NotStarted);
}
//--------------------------------------------------------------------------------
//For every new there's a delete.
GameController::~GameController()
{
     
    CHECK_ALL
        if (items[column][row] != nullptr)
            delete items[column][row];
}
//--------------------------------------------------------------------------------
void GameController::updateGameStatus(const GameStatus &status) 
{
    _gameStatus = status;
}
//--------------------------------------------------------------------------------
//Getting game status
GameStatus GameController::getGameStatus()
{
    return _gameStatus;
}
//--------------------------------------------------------------------------------
//if we can't open json file or there's errors - we load defaults 
void GameController::readJsonOptions()
{         
    try 
    {   
        std::ifstream stream(STARTING_OPTIONS); 
        if (stream.is_open())
        {
            json jsonOptions = json::parse(stream);
            ResourceList::GameOptions options;

            options.boardRow = jsonOptions["Board row"];
            if(options.boardRow<MIN_ROW|| options.boardRow > MAX_ROW)
                throw std::runtime_error("Error in row options\n");

            options.boardColumn = jsonOptions["Board column"];
            if (options.boardColumn<MIN_COLUMN || options.boardColumn > MAX_COLUMN)
                throw std::runtime_error("Error in column options\n");

            options.moveAmount = jsonOptions["Amount of moves"];
            if (options.moveAmount< MIN_AMOUNT_OF_MOVES)
                throw std::runtime_error("Amount of moves is too little\n"); 

            options.maxGem = jsonOptions["Amount of gem types"];
            if (options.maxGem < MIN_GEM || options.maxGem > MAX_GEM)
                throw std::runtime_error("Amount gems in options is wrong\n");

            json objectivesJson = jsonOptions["Objective"];
            if (objectivesJson.is_object())
            {
                auto object = objectivesJson.get<json::object_t>();
                if (jsonOptions["Objective"].size()< MIN_OBJECTIVES || jsonOptions["Objective"].size() > MAX_OBJECTIVES)
                    throw std::runtime_error("Amount of objectives is wrong \n");
                for (auto& singleObjective : object)   
                {   
                    options.objectiveValues[resourceList.stringToItemType[singleObjective.first]] = singleObjective.second;                    
                }
            }
            else
                throw std::runtime_error("Objective part in json file is wrong\n");

            if (options.objectiveValues.begin()->second==0)
                throw std::runtime_error("Gem amount in objective is wrong\n");

            resourceList.setNewOptions(options);
        }
    }
    catch (std::exception e)
    {
        printf(e.what());
        resourceList.setNewOptions(resourceList.defaultOptions);
    }    
}
//--------------------------------------------------------------------------------
//Checking for matches that takes default argument
bool GameController::checkForMatches(bool playerCaused)
{
    board_t boardState;
    boardState.resize(resourceList.options.boardColumn, std::vector<int>(resourceList.options.boardRow, 0));
    CHECK_ALL
    {
        if(items[column][row] !=nullptr)

            boardState[column][row] = items[column][row]->itemType;
        else
            boardState[column][row] = NO_INFO;
    }
    return checkForMatches(boardState, playerCaused);
}
//--------------------------------------------------------------------------------
//Checking if we matched something. If yeah - destroying it and returning "true"
bool GameController::checkForMatches(board_t &boardState, bool playerCaused )
{    
    //first checking custom, because it could include general shapes
    if (checkCustom(boardState, playerCaused))
        return true;
    //and then well general shapes
    if (checkVH(boardState, playerCaused))
        return true;    
    return false;
}
//--------------------------------------------------------------------------------
//function to refill empty spaces in the grid. 
void GameController::refillEmptySpaces()
{
    //let them fall
    bool fall;
    do //while something still falls - we continue the loop
    {
        fall = false;
        for (int column = resourceList.options.boardColumn - 1; column >= 0; column--)        
        {
            for (int row = resourceList.options.boardRow - 1; row >= 0; row--)
            {
                if (items[column][row] == nullptr)
                {

                    int i = row - 1;
                    while (i >= 0)
                    {                        
                        if (items[column][i] != nullptr)
                        {
                            items[column][i]->row = row;
                            std::swap(items[column][i], items[column][row]);
                            fall = true;//if there's a block above the empty one - it, well, falls
                            break;
                        }
                        i--;
                    }
                    if(i<0)
                        items[column][row] = new ItemClass(column,row, true);//if no items above tha empty block - we create one                                 
                }
            }
        }
    } while (fall);    
    return;
}
//--------------------------------------------------------------------------------
//Checking if after we clicked on 2 gems there's a new pattern 
bool GameController::swapCheck(ItemClass * item, bool playerCaused)
{
    //Is this a second click?
    if (ItemClass::previousPosition.isClicked() == true)
    {
        int it;
        
        board_t boardState;

        boardState.resize(resourceList.options.boardColumn, std::vector<int>(resourceList.options.boardRow, 0));
        int newRow, oldRow, newColumn, oldColumn;
        newRow = item->row;
        newColumn = item->column;
        oldRow = ItemClass::previousPosition.getRow();
        oldColumn = ItemClass::previousPosition.getColumn();

        //checking if gem was swapped with a neighbor. If not - we remember new position
        int rowCheck = ((newRow - oldRow) < 0) ? oldRow - newRow : newRow - oldRow;
        int columnCheck = ((newColumn - oldColumn) < 0) ? oldColumn - newColumn : newColumn - oldColumn;
        if ((rowCheck + columnCheck) == 1)
        {     
            auto swap = [&]()
            {
                it = items[newColumn][newRow]->itemType;
                items[newColumn][newRow]->convertItemType(items[oldColumn][oldRow]->itemType);
                items[oldColumn][oldRow]->convertItemType(it);
            };
            swap();
            CHECK_ALL   //filling up boardstate
            {
                if (items[column][row] == nullptr) //failsafe. It should not be that way.
                    boardState[column][row] = NO_INFO;
                else
                    boardState[column][row] = items[column][row]->itemType;
            }
            
            if (checkForMatches(boardState, playerCaused))
            {
                ItemClass::previousPosition.setClickedOnItem(false);        
                while (checkForMatches(boardState, playerCaused)); //one is enough for "true", but let's clear things up
                return true;
            }
            else
            {
                swap();
            }
        }
    }
    //if there was no successful swap or just a first click
    ItemClass::previousPosition.setClickedOnItem(true);
    ItemClass::previousPosition.setOldPos(item->column, item->row);
     
    return false;
}
//--------------------------------------------------------------------------------
//Drawing some text in the middle of the screen. 
void GameController::drawInTheMiddleOfTheScreen(const std::string& string)
{
    //Just a lot of lines for endscreen
    Text text;
    Font font;  
    
    font.loadFromFile(FONT);
    text.setFont(font);
    text.setFillColor(Color::White);
    text.setOutlineThickness(5);
    
    text.setString(string);
    text.setCharacterSize(30 * (WINDOW_SCALE * 2.0));
    
    float charSize = (text.getCharacterSize() + 5 );
    float charAmount = (float) string.length();
    std::cout << charAmount<<" "<<charSize<<" "<< std::endl;
    
    text.setPosition(((windowW / 2.0 - ((float)((charSize ) / 2.0 * charAmount )) / 2.0)), (windowH / 2.0));
    
    _app->draw(text);
}
//--------------------------------------------------------------------------------
//If the player is stuck for good we need to fix it
bool GameController::checkIfNoPossibleMovesAreLeft()
{    
    /*
    * 
    * check for + + 0 +  and +       at all rotations
    *                           + +  
    *                         
    */

    
    for (int row = 0; row < resourceList.options.boardRow; row++)
    for (int column = 0; column < resourceList.options.boardColumn - 1; column++)
    
    {      
        std::vector<std::pair<int, int>> rectangleMap = {       /*0*/    {column - 1,row - 1},/*0*/ /*0*/{column + 2,row - 1} ,
                                                        {column - 2,row - 0}, /*0*/    /*were here - 0 0 + 1 0*/  /*0*/ {column + 3,row - 0} ,
                                                                /*0*/   {column - 1,row + 1},/*0*/ /*0*/ {column +2,row +1 } };
     
        if (items[column][row] == nullptr|| items[column + 1][row] == nullptr)
            continue;
        if (items[column][row]->itemType >= BOMB_ROUND)//if there's a bomb - the user still can handle it!
            return true;
        int itemType = items[column][row]->itemType;
        if (items[column + 1][row]->itemType != itemType)
            continue;
        
        for (unsigned int i = 0; i < rectangleMap.size(); i++)
        {
            int x = rectangleMap[i].first;
            int y = rectangleMap[i].second;
            if (x < 0 ||
                y < 0 ||
                x >= resourceList.options.boardRow ||
                y >= resourceList.options.boardColumn)
                continue;

            if (items[x][y] == nullptr)
                continue;
            else if (items[x][y]->itemType == itemType)
                return true;
        }
    }

    for (int column = 0; column < resourceList.options.boardColumn; column++)
    for (int row = 0; row < resourceList.options.boardRow - 1; row ++)
    {
        std::vector<std::pair<int, int>> rectangleMap = { {row - 1,column - 1},
                                                            {row + 2,column - 1} ,
                                                            {row - 2,column - 0},  
                                                            {row + 3,column - 0} ,
                                                            {row - 1,column + 1},
                                                            {row + 2,column + 1 }};

        if (items[column][row] == nullptr || items[column ][row + 1] == nullptr)
            continue;
        if (items[column][row]->itemType >= BOMB_ROUND)
            return true;
        int itemType = items[column][row]->itemType;
        if (items[column ][row + 1]->itemType != itemType)
            continue;

        for (unsigned int i = 0; i < rectangleMap.size(); i++)
        {
            int x = rectangleMap[i].first;
            int y = rectangleMap[i].second;
            if (x < 0 ||
                y < 0 ||
                x >= resourceList.options.boardRow ||
                y >= resourceList.options.boardColumn)
                continue;

            if (items[x][y] == nullptr)
                continue;
            else if (items[x][y]->itemType == itemType)
                return true;
        }
    }    

    /*
    * check for + - +  and all 4 rotations. This thakes more time than the previous one, but that's just another search option
    *             +
    */
    CustomShape shapeCheck;
    board_t boardState;
    boardState.resize(resourceList.options.boardColumn, std::vector<int>(resourceList.options.boardRow, 0));
    CHECK_ALL
    {
        if (items[column][row] != nullptr)

            boardState[column][row] = items[column][row]->itemType;
        else
            boardState[column][row] = NO_INFO;
    }
    for (int i = 0; i < 4; i++)
    {
        if (checkIfShapeOnBoard(shapeCheck.getCustomShape(), boardState, false, shapeCheck.getBombShape(), true))
            return true;
        rotateShape(shapeCheck.getCustomShape());
    }
    int i;
    

    return false;
}
//--------------------------------------------------------------------------------
//and pop goes the weasel!
void GameController::bombPop(int column, int row)
{
    int BombType = items[column][row]->itemType;

    delete items[column][row];
    items[column][row] = nullptr;
    switch (BombType)
    {

    case(BOMB_HORIZONTAL):
        for (int y = 0; y < resourceList.options.boardColumn; y++) //horizontal bomb hates all things y!
        {
            if (items[y][row] == nullptr)
                continue;
            else if (items[y][row]->itemType >= BOMB_ROUND)//last gem. After that only bombs basically
                bombPop(y, row);//yay for recurtion!
            else
            {
                int checkGem = items[y][row]->itemType;
                if (resourceList.currentObjectiveValues.find(items[y][row]->itemType) != resourceList.currentObjectiveValues.end()
                    && resourceList.currentObjectiveValues[checkGem] > 0)
                        resourceList.currentObjectiveValues[checkGem]--;                    
                
                delete items[y][row];
                items[y][row] = nullptr;
            }
        }
        break;
    case(BOMB_VERTICAL): //vertical bomb hates all thing vertical!
        for (int x = 0; x < resourceList.options.boardRow; x++)
        {
            if (items[column][x] == nullptr)
                continue;
            else if (items[column][x]->itemType >= BOMB_ROUND)
                bombPop(column, x);
            else
            {
                int checkGem = items[column][x]->itemType;
                if (resourceList.currentObjectiveValues.find(items[column][x]->itemType) != resourceList.currentObjectiveValues.end()
                    && resourceList.currentObjectiveValues[checkGem] > 0)
                    resourceList.currentObjectiveValues[checkGem]--;
                delete items[column][x];
                items[column][x] = nullptr;
            }
        }
        break;
    case(BOMB_ROUND)://round bomb just rolling with it. And explodes everything around it.
    
        int m_row = 0;
        int m_column=0;
        //we don't want to go out of scope
        std::vector<std::pair<int, int>> roundMap = { {row - 1,column - 1},{row - 1,column - 0},{row - 1,column + 1}, 
                                                      {row - 0,column - 1},   /*We're here!*/   {row - 0,column + 1}, 
                                                      {row + 1,column - 1},{row + 1,column - 0},{row + 1,column + 1} };
                                                      
                                                      
        for (unsigned int i = 0; i < roundMap.size(); i++)
        {
            int x = roundMap[i].first;
            int y = roundMap[i].second;
            if (x < 0 || 
                y < 0 ||
                x >= resourceList.options.boardRow || 
                y >= resourceList.options.boardColumn)
                continue;

            if (items[y][x] == nullptr)
                continue;
            else if (items[y][x]->itemType >= BOMB_ROUND)// bombs basically
                bombPop(y, x);//yay for recurtion!
            else
            {
                int checkGem = items[y][x]->itemType;
                if (resourceList.currentObjectiveValues.find(items[y][x]->itemType) != resourceList.currentObjectiveValues.end()
                    && resourceList.currentObjectiveValues[checkGem] > 0)
                    resourceList.currentObjectiveValues[checkGem]--;
                delete items[y][x];
                items[y][x] = nullptr;
            }
        } 
    }
}
//--------------------------------------------------------------------------------
//Check if we got 3-5 gems in a row
bool GameController::checkVH(board_t & boardState, bool playerCaused)
{
    int currentItem = NO_INFO;
    int firstItemRow, firstItemColumn;
    int combo = 0;  
    auto objectiveCheck = [&]() //If the deletion was caused by player's action - we need to account for that with this lambda
    {
        if (playerCaused)
        {
            int checkGem = items[firstItemColumn][firstItemRow]->itemType;
            if (resourceList.currentObjectiveValues.find(checkGem) != resourceList.currentObjectiveValues.end() && resourceList.currentObjectiveValues[checkGem] > 0)
            {
                resourceList.currentObjectiveValues[checkGem] = resourceList.currentObjectiveValues[checkGem] - combo;
                if (resourceList.currentObjectiveValues[checkGem] < 0)
                    resourceList.currentObjectiveValues[checkGem] = 0;
            }
        }
    };
    //VERTICAL LINE CHECK===========================================================================
    //Creates horizontal bomb
    for (int column = 0; column < resourceList.options.boardColumn; column++)
    {
        for (int row = 0; row < resourceList.options.boardRow; row++)
        {    
            if (((combo < 3) && (boardState[column][row] >= BOMB_ROUND))|| items[column][row] == nullptr) //we can't have combos of bombs or empty spaces.   
            {
                combo = 0;
                currentItem = NO_INFO;
                continue;
            }   
            if (currentItem == boardState[column][row])
                combo++;
            else if (combo < 3)
            {
                firstItemRow = row;
                firstItemColumn = column;
                currentItem = boardState[column][row];
                combo = 1;
            }
            else            
                goto exit_checkHorizontal_loop;  //as we can't really break out of 2 loops at once we do a little goto       
        }
        if (combo < 3)
        {
            combo = 0;
            currentItem = NO_INFO;
        }
        else
            break;
    }
    exit_checkHorizontal_loop:
    if (combo >= 3) //if there's 3 gems in a row
    {
        if (playerCaused)
        {
            int checkGem = items[firstItemColumn][firstItemRow]->itemType;
            if (resourceList.currentObjectiveValues.find(checkGem) != resourceList.currentObjectiveValues.end() && resourceList.currentObjectiveValues[checkGem] > 0)
            {
                resourceList.currentObjectiveValues[checkGem] = resourceList.currentObjectiveValues[checkGem] - combo;
                if (resourceList.currentObjectiveValues[checkGem] < 0)
                    resourceList.currentObjectiveValues[checkGem] = 0;
            }
        }
        int row = firstItemRow;
        if (combo >= 4&& playerCaused)
        {
            items[firstItemColumn][firstItemRow]->convertItemType(BOMB_HORIZONTAL);
            row++;
        }
        for (; row < combo + firstItemRow; row++)
        {
            if (items[firstItemColumn][row] != nullptr)
            {              

                delete items[firstItemColumn][row];
                boardState[firstItemColumn][row] = NO_INFO;
            }
            items[firstItemColumn][row] = nullptr;
        }
        return true;
    }

    //HORIZONTAL LINE CHECK=========================================================================
    //Creates vertical bomb
    combo = 0;
    currentItem = NO_INFO;
    for (int row = 0; row < resourceList.options.boardRow; row++)
    {
        for (int column = 0; column < resourceList.options.boardColumn; column++)
        {
            if ((combo < 3 && boardState[column][row] >= BOMB_ROUND) || items[column][row] == nullptr)
            {
                combo = 0;
                continue;
            }
            
            if (currentItem == boardState[column][row])
                combo++;
            else if (combo < 3)
            {
                currentItem = boardState[column][row];
                firstItemRow = row;
                firstItemColumn = column;
                combo = 1;
            }
            else
                goto exit_checkVertical_loop;
        }
        if (combo < 3)
        {
            combo = 0;
            currentItem = NO_INFO;
        }
        else
            break;
    }
    
    exit_checkVertical_loop:
    if (combo >= 3)
    {
        objectiveCheck();
        int column = firstItemColumn;
        if (combo >= 4&& playerCaused)
        {
            items[firstItemColumn][firstItemRow]->convertItemType(BOMB_VERTICAL);
            column++;
        }
        for ( ; column < combo + firstItemColumn ; column++)
        {       
            if (items[column][firstItemRow] != nullptr)
            {
                delete items[column][firstItemRow];
                boardState[column][firstItemRow] = NO_INFO;
            }
            items[column][firstItemRow] = nullptr;
        }
        
        return true;
    }
    return false;
}
//--------------------------------------------------------------------------------
//checks if gems are formed into the custom shape;
bool GameController::checkCustom(board_t & boardState, bool playerCaused)
{    
    for (auto shape : resourceList.customShapes)
    {       
        if (checkIfShapeOnBoard(shape.getCustomShape(), boardState, playerCaused,shape.getBombShape()))
            return true;
    }
    return false;
}
//--------------------------------------------------------------------------------
//checks if we won or lost a game
bool GameController::checkIfGameFinished()
{  
    bool win = true;                                //winning by default! That's the spirit!
    bool lose = (resourceList.movesLeft > 0) ? false : true;  //if we're out of moves - then we lose  

    for (auto it = resourceList.currentObjectiveValues.begin(); it != resourceList.currentObjectiveValues.end(); ++it)
    {       
        if (it->second != 0) //if even one of them is not 0 - we didn't win
            win = false;  
    }    
    if (win) //if we have no objectives left - we win! Even if we have no moves left - we still win!
    {
        updateGameStatus(Won);
        return 0;
    }
    if (lose)//no moves left but objectives are a-plenty? Too bad.
    {
        updateGameStatus(Failed);
        return 0;
    }
    return 1;
}
//--------------------------------------------------------------------------------
//Draws objective graphics (above play board)
void GameController::drawObjectives()
{
    std::vector < ItemClass> objective;
    std::vector < Text> text;
    std::string num;
    Font font;
    font.loadFromFile(FONT);

    //Font for the objectives    
    int i=0;
    Sprite tileObjectives, tileMovesLeft;
    tileObjectives.setTexture(*resourceList.itemTextures.at(TILE_1));
    tileObjectives.setTextureRect(IntRect(0, 0, windowW/2,WINDOW_OFFSET* WINDOW_SCALE));
    tileObjectives.setPosition(0, 0);
    _app->draw(tileObjectives);

    tileMovesLeft.setTexture(*resourceList.itemTextures.at(TILE_2));
    tileMovesLeft.setTextureRect(IntRect(0, 0, windowW /2, WINDOW_OFFSET * WINDOW_SCALE));
    tileMovesLeft.setPosition(windowW/2, 0);
    _app->draw(tileMovesLeft);
    //
    //creating itterator for going through the objectives
    
    for (auto it = resourceList.currentObjectiveValues.begin(); it != resourceList.currentObjectiveValues.end(); ++it)
    {           
        objective.push_back(ItemClass());
        text.push_back(Text());
        text.at(i).setFont(font);
        text.at(i).setFillColor(Color::White);
        text.at(i).setOutlineThickness(5);
        text.at(i).setScale(WINDOW_SCALE, WINDOW_SCALE);
        num = std::to_string(it->second);
        text.at(i).setString(num);
        objective.at(i).convertItemType(it->first);
        float offsetX, offsetY;
        offsetX = (float)(((SPRITE_SIZE - (float)objective.at(i).getTexture()->getSize().x) / 2.0) * WINDOW_SCALE);
        offsetY = (float)(((SPRITE_SIZE - (float)objective.at(i).getTexture()->getSize().y) / 2.0) * WINDOW_SCALE);
       
        objective.at(i).setPosition((float)(((SPRITE_SIZE / 2.0) + SPRITE_SIZE * i + offsetX) * WINDOW_SCALE),
            (float)((offsetY + (SPRITE_SIZE / 2.0)) * WINDOW_SCALE));
     
        text.at(i).setPosition((float)((SPRITE_SIZE * i + SPRITE_SIZE*0.8) * WINDOW_SCALE), 
            (float)(( (SPRITE_SIZE * 1.5)) * WINDOW_SCALE));
        _app->draw(objective.at(i));
        _app->draw(text.at(i));
        i++;
    }
    text.push_back(Text());
    text.at(i).setFont(font);
    text.at(i).setFillColor(Color::White);
    text.at(i).setOutlineThickness(5);
    text.at(i).setScale(WINDOW_SCALE*1.5, WINDOW_SCALE * 1.5);
    num = std::to_string(resourceList.movesLeft);
    text.at(i).setString(num);
    text.at(i).setPosition(windowW/4+(windowW / 2-20), (WINDOW_OFFSET/2-20)*WINDOW_SCALE);
    _app->draw(text.at(i));   
}
//--------------------------------------------------------------------------------
//reading options, setting up window and launching the game!
void GameController::startGame() 
{    
    readJsonOptions();
    windowW = (float)(( SPRITE_SIZE * resourceList.options.boardColumn + SPRITE_SIZE) * WINDOW_SCALE);
    windowH = (float)((WINDOW_OFFSET  + SPRITE_SIZE * resourceList.options.boardRow + SPRITE_SIZE) * WINDOW_SCALE);
    _app = new RenderWindow(VideoMode((unsigned int)windowW, (unsigned int)windowH ), "Game", Style::Close);
    _app->setFramerateLimit(60);
    run();
}
//--------------------------------------------------------------------------------
//The "main" game function. Drawing sprites, clicking on things
void GameController::run() 
{ 
    updateGameStatus(Started);
    //vectors for items and a backgroud    
    items.resize(resourceList.options.boardColumn, std::vector<ItemClass *>(resourceList.options.boardRow, nullptr));
    std::vector<std::vector<Sprite>> background;
    background.resize(resourceList.options.boardColumn, std::vector<Sprite>(resourceList.options.boardRow, Sprite()));
    int moving;//flag to check if things are falling from above
    //for tile's background to be checkered we have to flip it a bit   
    bool flip = false;      
    for (int column = 0; column < resourceList.options.boardColumn; column++)
    {
        for (int row = 0; row < resourceList.options.boardRow; row++)
        {
            items[column][row] = new ItemClass(column, row,true);

            background[column][row].setPosition((float)((SPRITE_SIZE * (float) column + SPRITE_SIZE / 2) * WINDOW_SCALE),
                (float)((SPRITE_SIZE * (float)row + WINDOW_OFFSET +  SPRITE_SIZE / 2) * WINDOW_SCALE));
            background[column][row].setTexture(*resourceList.itemTextures.at(TILE_1 + flip));
            background[column][row].setScale(WINDOW_SCALE, WINDOW_SCALE);
                     
            flip = !flip;
        }
        if(!(resourceList.options.boardColumn % 2))
            flip = !flip;
    }
    do { refillEmptySpaces(); } while (checkForMatches());
    while (_app->isOpen()) 
    {
        _app->clear(Color(60, 40, 90));
        //if no possible moves are left - we resetting the board

        if (!checkIfNoPossibleMovesAreLeft())
        {
            CHECK_ALL
            {
                delete items[column][row];
                items[column][row] = nullptr;
            }
            do { refillEmptySpaces(); } while (checkForMatches());
        }
        //erasing all the matches that randomy were created        
        //checking if animation is in process
        moving = 0;
        CHECK_ALL
        {
            if (items[column][row] != nullptr)
                moving += items[column][row]->fallingAnimation();//if anyting is falling - we need to deal with it
        }
        if (moving==0) //we don't want player clicking things when animation is in process
        {        
            //Catching player's clicking
            sf::Event event;
            while (_app->pollEvent(event)) 
            {
                Vector2i pos = Mouse::getPosition(*_app) ;
                if (event.type == Event::MouseButtonPressed)
                    if (event.key.code == Mouse::Left)
                    {
                        //if the game has ended - after user's input we reset the game
                        if (getGameStatus() != Started)
                        {
                            //loading up options
                            resourceList.reloadOptions();
                            updateGameStatus(Started);
                            //reseting the field                            
                            
                            CHECK_ALL
                            {                             
                                //and randomly assighning the gem type to everything. No need to erase and create the whole board.
                                items[column][row]->createAGem(column, row,false);
                            }    
                            //refilling all the empty spaces
                            do { refillEmptySpaces(); } while (checkForMatches());
                            continue;
                        }
                        CHECK_ALL //if the game's still playes we track were the player clicked
                        {                        
                            if (items[column][row] != nullptr && items[column][row]->getGlobalBounds().contains(pos.x, pos.y))
                            {               
                                if (items[column][row]->itemType >= BOMB_ROUND)          //basically if it's a bomb
                                {
                                    ItemClass::previousPosition.setClickedOnItem(false);//and if it is we reset previos click (if it was on gem)
                                    bombPop(column, row);                               //and popping the bomb
                                    resourceList.playerMadeAMove();                               //and reducing the amount of moves by 1
                                }
                                else if (swapCheck(items[column][row], 1)) //if not a bomb - we're checking if that was a second click and swapping gems
                                    resourceList.playerMadeAMove();                  //if the move was legal - we decrease the amount of moves    
                            }
                        }
                        //refillingspaces after the erasure
                        do { refillEmptySpaces(); } while (checkForMatches());
                    }
                // "close requested" event: we close the window
                if (event.type == sf::Event::Closed)
                    _app->close();                    
            }   
        }
        if (checkIfGameFinished())//if the game's neither won or lost after checking the objectives - we draw our board and gems
        { 
            //drawing background first or it'll pop out
            CHECK_ALL
                _app->draw(background[column][row]);
        
            //drawing all non-empty gem spaces
            CHECK_ALL        
                if (items[column][row] != nullptr)
                    _app->draw(*items[column][row]);
            
        }
        else//if not - we're just showing the endscreen
        {        
            drawInTheMiddleOfTheScreen((getGameStatus() == Won) ? "GAME COMPLETE" : "GAME FAILED");        
        }
        drawObjectives();//drawing it last so it's: 1 - a curtain for falling gems,
                         //                         2 - even after the game's completed - the score's on the screen
        _app->display();
    }   
}
//--------------------------------------------------------------------------------
//Checks custom shape, deletes it if not just checking and adds to score if player caused
 bool GameController::checkIfShapeOnBoard(board_t & shape, board_t & board, bool playerCaused,int bombType, bool justChecking)
{
    std::vector<ItemClass**> pointersToItemsToDelete;
    int itemType;
    bool dontKnowItemType;
    for (unsigned int column = 0; column < board.size() - shape.size()+1; column++)\
    for (unsigned int row = 0; row < board[0].size() - shape[0].size()+1; row++)
    {
        auto checkShapeFromCurrentPosition = [&]()
        {
            pointersToItemsToDelete.clear();
            dontKnowItemType = true;
            for (unsigned int s_column = 0; s_column < shape.size(); s_column++)
            for (unsigned int s_row = 0; s_row < shape[0].size(); s_row++)
            {
                if (shape[s_column][s_row] == 0)
                    continue;
                if (dontKnowItemType)
                {
                    if (items[column + s_column][row + s_row] == nullptr)//if needed point is missing - we're failed
                        return false;
                    itemType = items[column + s_column][row + s_row]->itemType;
                    if (itemType >= BOMB_ROUND) //we don't want combos of bombs
                        return false;;
                    dontKnowItemType = false;                    
                }
                if (board[column + s_column][row + s_row] == itemType)                
                    pointersToItemsToDelete.push_back(&items[column + s_column][row + s_row]);                
                else
                    return false;
            }
            return true;
        };

        if (checkShapeFromCurrentPosition())
        {
            if (justChecking == false)
            {
                bool once = true;
                for (unsigned int i = 0; i < pointersToItemsToDelete.size(); i++)
                {
                    if (once && playerCaused)
                    {

                        int checkGem = (*pointersToItemsToDelete[0])->itemType;
                        if (resourceList.currentObjectiveValues.find(checkGem) != resourceList.currentObjectiveValues.end() && resourceList.currentObjectiveValues[checkGem] > 0)
                        {
                            resourceList.currentObjectiveValues[checkGem] = resourceList.currentObjectiveValues[checkGem] - pointersToItemsToDelete.size();
                            if (resourceList.currentObjectiveValues[checkGem] < 0)
                                resourceList.currentObjectiveValues[checkGem] = 0;
                        }

                        once = false;
                        (*pointersToItemsToDelete[i])->convertItemType(bombType);
                    }
                    else
                    {
                        if (*pointersToItemsToDelete[i] != nullptr)
                            delete* pointersToItemsToDelete[i];
                        *pointersToItemsToDelete[i] = nullptr;
                    }
                }
            }
            else
            {
                return true;

            }
            return true;
        }        
    }
    return false;
}
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------

