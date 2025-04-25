//here will be the class for the menu 
//the menu will serve the user and send his input to the right function 

#pragma once

//includes

//headers
#include "Playlist.h"
#include <map>

class Menu
{
public:
	Menu();
	void serve();

private:
	void createPlaylist();
	void selectPlaylist();
	SqliteDataBase& _db;
};