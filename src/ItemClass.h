#pragma once

#include "ResourceList.h"

//creating an item class that inherits sprite
class ItemClass : 
	public Sprite
{
public:
	//creating our item as a gem by default
	ItemClass();
	
	ItemClass(int column, int row, bool spawnAbove = false);

	void createAGem(int column, int row,bool spawnAbove);
	//a function to change item's position on the grid and create the right sprite shape for when it changes into a bomb
	void fixSpritePostition(bool spawnAbove = false);

	//Converting an item type into the new one. Used for switching and turning gems into bombs
	void convertItemType(int newType);

	//returns true if the tile is moving
	bool fallingAnimation();

	struct PreviousPosition
	{
		PreviousPosition()
		{
			row = 0;
			column = 0;
			clicked = false;
		}
		void setOldPos(int posX, int posY)
		{
			row = posY;
			column = posX;
		}
		
		void setClickedOnItem(bool newClicked)
		{
			clicked = newClicked;
		}
		bool isClicked()
		{
			return clicked;
		}
		int getRow()
		{
			return row;
		}
		int getColumn()
		{
			return column;
		}
	protected:
		bool clicked;
		int  row;
		int column;
		
	};

public:
	//bombs and gems use different mouse press functions
	virtual void mousePressed(){};		
public:
	static PreviousPosition previousPosition;
	int itemType = 0;
	int row = 0;
	int column = 0;	

	bool thisTileIsMoving = false;
	float speed = 2.0;
};
//--------------------------------------------------------------------------------

