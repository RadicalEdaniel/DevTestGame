#include "ItemClass.h"
/*
* 
* Item class to easily manipulate textures and sprites. Here we declare falling animation
*  
*/

ItemClass::PreviousPosition ItemClass::previousPosition;

//creating our item as a gem by default

 ItemClass::ItemClass() : Sprite()
{
	column = 0;
	row = 0;
	itemType = 0;
	thisTileIsMoving = 0;
	speed = 2;
}
 ItemClass::ItemClass(int column, int row, bool spawnAbove) :sf::Sprite()
{
	createAGem(column, row, spawnAbove);
}
//--------------------------------------------------------------------------------
 //Crates the gem at the given spot. If "spawn about" - the gem spawns at the top
void ItemClass::createAGem(int column, int row, bool spawnAbove)
{
	//a place in a grid
	this->column = column;
	this->row = row;
	
	/* we need to create a pool of gems that we'll gonna use,
	* the ones that are in the objective are obviously required
	*/
	int gemNumber = rand() % resourceList.options.maxGem;				//getting a random number that'll dictate the gem's color 
	std::vector<int> gemPool;
	for (auto& requiredGems : resourceList.options.objectiveValues)	//inserting required gems in the gem pool
		gemPool.push_back(requiredGems.first);
	int gemToSearch = GEM_BLUE;								//Blue gem is the first one, later we'll iterate on it
	while (gemPool.size() < resourceList.options.maxGem &&
		gemToSearch <= GEM_VIOLET)							//filling up gemPool and placing a safeguard for the last gem
	{
		if (std::find(gemPool.begin(), gemPool.end(), gemToSearch) == gemPool.end())
			gemPool.push_back(gemToSearch);					//when we can't find a gem - we plase it in a gem pool
		gemToSearch++;
	}

	int gemType = gemPool[gemNumber];						//assigning the itemType as a random valid gem
	itemType = gemType;
	this->setTexture(*resourceList.itemTextures.at(itemType));
	fixSpritePostition(spawnAbove);//spawn above

}
//--------------------------------------------------------------------------------
//a function to change item's position on the grid and create the right sprite shape for when it changes into a bomb
 void ItemClass::fixSpritePostition(bool spawnAbove)
{
	float offsetX, offsetY;
	offsetX = float(((SPRITE_SIZE - this->getTexture()->getSize().x) / 2) * WINDOW_SCALE);
	
	offsetY = float(((SPRITE_SIZE - this->getTexture()->getSize().y) / 2)* WINDOW_SCALE);
	float y = (spawnAbove) ? 0 : float(((SPRITE_SIZE / 2) + SPRITE_SIZE * row +    offsetY + WINDOW_OFFSET) * WINDOW_SCALE);
	float x =					   float(((SPRITE_SIZE / 2) + SPRITE_SIZE * column + offsetX )			    * WINDOW_SCALE);
	this->setPosition(x,y);
	this->setTextureRect(IntRect(0, 0, resourceList.itemTextures.at(itemType)->getSize().x, resourceList.itemTextures.at(itemType)->getSize().y));
	this->setScale(WINDOW_SCALE, WINDOW_SCALE);
}
 //--------------------------------------------------------------------------------
//Converting an item type into the new one. Used for switching and turning gems into bombs
 void ItemClass::convertItemType(int newType)
{
	itemType = newType;
	this->setTexture(*resourceList.itemTextures.at(newType));
	fixSpritePostition();//fixing sprite after conversion
}
 //--------------------------------------------------------------------------------
 //animation for falling gems and bombs
 bool ItemClass::fallingAnimation()
 {
	 speed =float( 4 * (row+1));
	 float offsetY = float(((SPRITE_SIZE - this->getTexture()->getSize().y) / 2) * WINDOW_SCALE);
	 float currentY = float(this->getPosition().y);
	 float newY = float(((SPRITE_SIZE / 2) + SPRITE_SIZE * row + offsetY + WINDOW_OFFSET) * WINDOW_SCALE);
	
	 if (currentY == newY) //if sprite is in the right place - leaveing it as is and returning false
	 {
		 thisTileIsMoving = false;
	 }
	 else if ( (newY - currentY) > speed) //the sprite is falling so we only need to see if it's currently "above" the right spot
	 {		
		 currentY = currentY + speed;//animation speed

		 this->setPosition(float(this->getPosition().x), currentY);//animation speed
		 thisTileIsMoving = true;
	 }
	 else//fixing sprite's position just in case. Basically teleporting it to the right spot
	 {
		 fixSpritePostition(); 
		 thisTileIsMoving = false;
	 }
	 
	 return thisTileIsMoving;
 }
 //--------------------------------------------------------------------------------
 //--------------------------------------------------------------------------------
 //--------------------------------------------------------------------------------

