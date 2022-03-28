#pragma once
/*
* 
* Here we store the options and custom shapes for the game
* as well as macros and libs
*/
//libs
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Main.hpp>
#include "single_include/nlohmann/json.hpp"
#include <random>
#include <thread>
#include <stdio.h>
#include <functional>
#include <fstream>

typedef  std::vector<std::vector<int>> board_t; //the only downside is that [row][column] is swapped - [column][row] 
using namespace sf;
using json = nlohmann::json;

//macros
#define SPRITE_SIZE  87 //basically tile size
#define MIN_COLUMN 7
#define MAX_COLUMN 10
#define MIN_ROW 7
#define MAX_ROW 10
#define MIN_GEM 3
#define MAX_GEM 5
#define MIN_AMOUNT_OF_MOVES 1
#define MIN_OBJECTIVES 1
#define MAX_OBJECTIVES 3
#define NO_INFO 1337 //Replacer for an "empty" cell in a positional array 
#define WINDOW_OFFSET 200//offset for the score part
#define WINDOW_SCALE 0.5//Without rescaling the window is waaay too big
#define FONT "../resources/ABeeZee-Regular.otf" //font 
#define STARTING_OPTIONS "../resources/startingOptions.json"//file with starting options

#define CHECK_ALL for (int column = 0; column < resourceList.options.boardColumn; column++)\
				  for (int row    = 0 ; row    < resourceList.options.boardRow   ; row++   )//Just to not copy-paste it
               
enum itemType
{
	GEM_BLUE,
	GEM_GREEN,
	GEM_ORANGE,
	GEM_RED,
	GEM_VIOLET,

	BOMB_ROUND ,
	BOMB_HORIZONTAL,
	BOMB_VERTICAL,

	TILE_1,
	TILE_2
};
//--------------------------------------------------------------------------------
class CustomShape
{
public:
	CustomShape();
	CustomShape(board_t customShape,int bombType);


	//just returns our shape
	board_t getCustomShape()
	{
		return customShape;
	}
	int getBombShape()
	{
		return bombType;
	}
private:
	board_t customShape;
	int bombType;
};
//--------------------------------------------------------------------------------

struct ResourceList
{
	ResourceList();
	~ResourceList();


	struct GameOptions
	{
		int boardRow = 8;						//7-10
		int boardColumn =9;						//-
		unsigned int moveAmount = 15;					//whatever		
		std::map<int, int >  objectiveValues;	//1-3 objectiveCount, color/amount
		unsigned int maxGem = 3;							//figures color count 3-5
	};
	void setNewOptions(GameOptions &newOptions);
	
	bool playerMadeAMove(); //check if out of moves
	void createCustomShapes();
	void reloadOptions();
public:
	GameOptions options;
	GameOptions defaultOptions;
	std::vector< std::string> spritePaths;
	std::vector< sf::Texture*> itemTextures;
	std::map<int, int >  currentObjectiveValues;
	std::map<std::string, int > stringToItemType;
	int movesLeft;
	std::vector<CustomShape> customShapes;
	
};

//--------------------------------------------------------------------------------
void rotateShape(board_t& shape);
//--------------------------------------------------------------------------------
void flipShape(board_t& shape);
//--------------------------------------------------------------------------------
extern ResourceList resourceList;
//--------------------------------------------------------------------------------
