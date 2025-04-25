#include "Menu.h"
#include <limits>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <map>

using std::cout;
using std::string;
using std::endl;
using std::cin;

Menu::Menu()
{ 
	cout << "welcome to my app :)\ncreating a dir for you to add your songs" << endl;
	std::filesystem::path dirPath(songs);
	if (!std::filesystem::exists(dirPath))
	{
		if (std::filesystem::create_directory(dirPath))
		{
			std::filesystem::path dirPath(PLAYLIST_DATA);
			std::filesystem::create_directory(dirPath);
			string command = "attrib +h \"" + dirPath.string() + "\"";
			system(command.c_str());
			cout << "your Directory was created and its name is: Songs" << endl;
		}
	}
	else
	{
		std::cout << "you already have a dir :) and you can add more songs to it" << std::endl;
	}

	//restore playlists
	for (const auto& playlist : std::filesystem::directory_iterator(PLAYLIST_DATA))
	{
		std::string playlistName = playlist.path().filename().string();
		this->_playlists.push_back(std::move(Playlist(playlistName.substr(0, playlistName.find_last_of('.')))));
	}
} 

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
		std::cin.ignore(); //ignore the \n that is left
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
	cout << "goodbye! your playlists are saved" << endl;
}

void Menu::createPlaylist()
{
	std::string name = "";
	cout << "Please enter the name for the playlist: " << endl;
	std::getline(cin, name);
	if (name.empty())
	{
		throw std::invalid_argument("name can't be empty! ");
	}
	if (std::find(this->_playlists.begin(), this->_playlists.end(), name) != this->_playlists.end())
	{
		throw std::invalid_argument("playlist with that name already exists!");
	}

	std::ofstream playlist(std::format(PLAYLIST_PATH, name));
	if (!playlist.is_open())
	{
		throw std::invalid_argument("cannot create this playlist");
	}
	playlist.close();
	this->_playlists.push_back(std::move(Playlist(name)));
	cout << name << " - was created" << endl;
}

void Menu::selectPlaylist()
{
	cout << "here is a list of your playlists: " << endl;
	for (const auto& playlist : this->_playlists)
	{
		cout << playlist << endl;
	}
	cout << "select one of the playlists by typing its name: " << endl;
	string selectedPlaylist = "";
	std::getline(cin, selectedPlaylist);
	auto playlist = std::find(this->_playlists.begin(), this->_playlists.end(), selectedPlaylist);
	if (playlist == this->_playlists.end())
	{
		throw std::invalid_argument("cannot find selected playlist!");
	}
	else if (selectedPlaylist.empty())
	{
		throw std::invalid_argument("name cannot be empty");
	}
	playlist->serve();
}
