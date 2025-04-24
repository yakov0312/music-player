//here will be the class for the menu 
//the menu will serve the user and send his input to the right function 

#pragma once

//includes

//headers
#include "Playlist.h"
#include <map>

//consts 
constexpr const char* PLAYLIST_DATA = "playlistData";

class Menu
{
public:
	Menu();
	void serve();

private:
	void createPlaylist();
	void selectPlaylist();
	std::map<std::string, Playlist> _playlists; // - will restore it self(the way is written in Playlist.h)
};