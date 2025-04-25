#include "Menu.h"
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>

using std::endl,std::string, std::cout, std::cin;

//ctor for menu
Menu::Menu() : _db(SqliteDataBase::getInstance())
{ 
	cout << "Welcome to my app :)\nCreating a dir for you to add your songs\n Note: the  songs should be in .wav (in the future you wont have to download them yourself)" << endl;
	std::filesystem::path dirPath(SONGS_PATH);
	if (!std::filesystem::exists(dirPath))
	{
		std::filesystem::create_directory(dirPath);
	}
	else
	{
		std::cout << "You already have a dir :) and you can add more songs to it" << endl;
	}
} 
//the function will serve the client 
void Menu::serve() 
{
	int choice = 0;
	cout << "Welcome to the menu!" << endl;
	while (choice != 3)
	{
		cout << "Select one option from below : \n1.Create a playlist\n2.Select a playlist\n3.Exit" << endl;
		cin >> choice;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max());
			cout << "Dont try to crash my program please! I've very worked hard" << endl;
			continue;
		}
		cin.ignore(); //ignore the \n that is left
		try
		{
			if (choice == 1)
			{
				this->createPlaylist();
			}
			else if (choice == 2)
			{
				this->selectPlaylist();
			}
		}
		catch (const std::invalid_argument& e)
		{
			cout << "Error: " << e.what() << endl;
		}
	}
	cout << "Goodbye! your playlists are saved" << endl;
}
//the function will create a playlist 
void Menu::createPlaylist()
{
	string name = "";
	cout << "Please enter the name for the playlist: " << endl;
	std::getline(cin, name);
	if (name.empty())
	{
		throw std::invalid_argument("Name can't be empty! ");
	}
	const auto& playlists = this->_db.getPlaylists();
	if (std::find(playlists.begin(), playlists.end(), name) != playlists.end())
	{
		throw std::invalid_argument("Playlist with that name already exists!");
	}
	
	this->_db.createPlaylist(name);
	cout << name << " - was created" << endl;
}
//the function will select a playlist and let the playlist serve the client
void Menu::selectPlaylist()
{
	cout << "List of your playlists: " << endl;
	const auto& playlists = this->_db.getPlaylists();
	for (const auto& playlist : playlists)
	{
		cout << playlist << endl;
	}
	cout << "Select one of the playlists by typing its name: " << endl;
	string selectedPlaylist = "";
	std::getline(cin, selectedPlaylist);
	if (selectedPlaylist.empty())
	{
		throw std::invalid_argument("Name cannot be empty");
	}
	auto playlist = std::find(playlists.begin(), playlists.end(), selectedPlaylist);
	if (playlist == playlists.end())
	{
		throw std::invalid_argument("Cannot find selected playlist!");
	}
	Playlist(selectedPlaylist,this->_db).serve(); // serve current playlist
}
