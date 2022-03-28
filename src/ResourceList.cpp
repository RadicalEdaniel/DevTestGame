#include "ResourceList.h"

//--------------------------------------------------------------------------------
//declaring the var for external use
ResourceList resourceList;
//--------------------------------------------------------------------------------
//deconstructor for the resource list class
ResourceList::~ResourceList()
{
	for(auto i: itemTextures)	
		delete i;	
}
//--------------------------------------------------------------------------------
//constructor for the resource list class
ResourceList::ResourceList()
{
	createCustomShapes();
	//getting a random seed - one time for the whole game
	srand(time(NULL));

	//filling up sprite vector
	spritePaths.push_back("../Resources/blue.png");
	spritePaths.push_back("../Resources/green.png");
	spritePaths.push_back("../Resources/orange.png");
	spritePaths.push_back("../Resources/red.png");
	spritePaths.push_back("../Resources/violet.png");

	spritePaths.push_back("../Resources/bomb.png");
	spritePaths.push_back("../Resources/h_bomb.png");
	spritePaths.push_back("../Resources/v_bomb.png");

	spritePaths.push_back("../Resources/tile_1.png");
	spritePaths.push_back("../Resources/tile_2.png");

	stringToItemType["Blue gem"] = GEM_BLUE;
	stringToItemType["Green gem"] = GEM_GREEN;
	stringToItemType["Orange gem"] = GEM_ORANGE;
	stringToItemType["Red gem"] = GEM_RED;
	stringToItemType["Violet gem"] = GEM_VIOLET;
	
	//loading textures from files inside a texture vector
	for (int i = 0; i < 10; i++)
	{		
		itemTextures.push_back(new Texture());
		itemTextures.at(i)->loadFromFile(spritePaths[i]);
	}		

	//loading up default options
	defaultOptions.boardRow=8;
	defaultOptions.boardColumn = 9;
	defaultOptions.moveAmount=15;
	defaultOptions.objectiveValues[GEM_BLUE] = 15;
	defaultOptions.objectiveValues[GEM_RED] = 15;
	defaultOptions.objectiveValues[GEM_GREEN] = 5;
	defaultOptions.maxGem=3;	
	//setNewOptions(defaultOptions);
}
//--------------------------------------------------------------------------------
//setting up the var options for the game
void ResourceList::setNewOptions(GameOptions &newOptions)
{
	options = newOptions;
	reloadOptions();
}
//--------------------------------------------------------------------------------
//player clicked on a tile or bomb
bool ResourceList::playerMadeAMove()
{
	movesLeft--;
	return movesLeft>0;
}
//--------------------------------------------------------------------------------
//player clicked on a tile or bomb
void ResourceList::createCustomShapes()
{
	//square-shape
	board_t board1;
	board1.push_back({ 1, 1 });
	board1.push_back({ 1, 1 });
	customShapes.push_back( CustomShape(board1,BOMB_HORIZONTAL));

	//Г-shape
	board_t  board2;
	board2.push_back({ 1, 1 });
	board2.push_back({ 1, 0 });
	board2.push_back({ 1, 0 });
	customShapes.push_back(CustomShape(board2, BOMB_ROUND));
}
//--------------------------------------------------------------------------------
//Reloads objective and amount of moves - bascially resetting the game
void ResourceList::reloadOptions()
{
	currentObjectiveValues = options.objectiveValues;
	movesLeft = options.moveAmount;
}
//--------------------------------------------------------------------------------
//Shape-check by default
CustomShape::CustomShape()
{
	customShape.push_back({ 1, 0});
	customShape.push_back({ 0, 1});	
	customShape.push_back({ 1, 0 });
	bombType = BOMB_ROUND;
}
//--------------------------------------------------------------------------------
//because of the std::vector<std::vector<int>> - column and row are swapped. To prevent shenanigans we flip the custom shape as well
inline CustomShape::CustomShape(board_t customShape,int bombType) :customShape(customShape), bombType(bombType)
{
	flipShape(this->customShape);
}
//--------------------------------------------------------------------------------
//Rotates a map of a shape
void rotateShape(board_t & shape)
{
	board_t buShape;
	buShape.resize(shape[0].size(), std::vector<int>(shape.size(), 0));
	for (unsigned int row = 0; row < shape[0].size(); row++)	
		for (unsigned int column = 0; column < shape.size(); column++)
			buShape[row][column] = shape[shape.size()-1 - column][row];		
	shape = buShape;
}
//--------------------------------------------------------------------------------
//Flips shape
void flipShape(board_t& shape)
{
	board_t swapShape;
	swapShape.resize(shape[0].size(), std::vector<int>(shape.size(), 0));
	for (unsigned int column = 0; column < shape.size(); column++)
		for (unsigned int row = 0; row < shape[0].size(); row++)
		{
			swapShape[row][column] = shape[column][row];
		}
	shape = swapShape;
}


//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------
//--------------------------------------------------------------------------------