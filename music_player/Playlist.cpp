#include "Playlist.h"
#define NOMINMAX
#include <limits>
#include <fstream>
#include <string>
#include <filesystem>
#include <thread>
#include "Windows.h"

#pragma comment(lib, "winmm.lib")

using std::string, std::endl, std::cout, std::cin;

//ctor for playlist
Playlist::Playlist(const string& name, SqliteDataBase& db) : _name(name), _shuffled(false), _running(true), _db(db)
{
	//to do: get shuffled from db
}
//the function will serve the client
void Playlist::serve()
{
	int choice = 0;
	while (choice != 5)
	{
		cout << "Playlist's name : " << this->_name << "\nChoose one of the options below : \n1.Add a song\n2.Remove a song\n3.Play the playlist" << endl;
		cout << ((this->_shuffled) ? "4.Turn shuffled mode off\n5.exit\n" : "4.Turn shuffled mode on\n5.exit\n");

		cin >> choice;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max());
			cout << "Wrong input!!!" << endl;
			continue;
		}
		if (choice == 1)
		{
			this->addSong();
		}
		else if (choice == 2)
		{
			this->removeSong();
		}
		else if (choice == 3)
		{
			this->_running = true;
			std::thread stopPlay(&Playlist::stopToPlay, this);
			stopPlay.detach();
			this->playPlaylist();
		}
		else if (choice == 4)
		{
			this->_shuffled = !this->_shuffled;
		}
		else if (choice == 5)
		{
			return;
		}
		else
		{
			cout << "Wrong input! " << endl;
		}
	}
}
//the function will add a song to the playlist
void Playlist::addSong()
{
	string song = "";
	do
	{
		try
		{
			song = this->getSong(false);
		}
		catch (const std::invalid_argument& e)
		{
			cout << e.what() << endl;
		}
		if (song == EXIT)
		{
			break;
		}
		this->_db.addSongToPlaylist(this->_name, song);
	} while (true);
}
//the function will remove a song from the playlist
void Playlist::removeSong()
{
	string song = "";
	do
	{
		try
		{
			song = this->getSong(true);
		}
		catch (const std::invalid_argument& e)
		{
			cout << e.what() << endl;
			return;
		}
		if (song == EXIT)
		{
			break;
		}

		this->_db.removeSongFromPlaylist(this->_name, song);
	} while (true);
}
//the function will stop the song 
void Playlist::stopToPlay()
{
	string stop = "";
	cout << "Note: if no audio is playing insure the song exist in the song dir\nEnter exit to stop the audio" << endl;
	while (this->_running)
	{
		std::getline(cin, stop);
		if (stop == "exit")
		{
			mciSendStringA("stop audiofile", NULL, 0, NULL);
			this->_running = false;
		}
	}
}

// the function will play a given song
void Playlist::playSong(const string& song) const
{
	// Open the audio file
	string openCommand = std::format(OPEN_SONG, song);
	mciSendStringA(openCommand.c_str(), NULL, 0, NULL);

	mciSendStringA(PLAY_SONG, NULL, 0, NULL); 

	while (this->_running)
	{
		Sleep(100);  // Sleep for a short period to avoid high cpu usage
	}

	mciSendStringA(CLOSE_SONG, NULL, 0, NULL);
}
//the function will play the playlist
void Playlist::playPlaylist() const
{
	std::srand(std::time(0));
	const auto& songs = this->_db.getSongsOfPlaylist(this->_name);
	int size = songs.size();
	int i = 0;
	int lastChoice = 0;
	while (true)
	{
		for(i=0;i<size;i++)
		{
			if (this->_shuffled)
			{
				do
				{
					i = std::rand() % size;
				} while (lastChoice == i);
				lastChoice = i;
			}
			this->playSong("./" + string(SONGS_PATH) + "/" + songs[i]);
			if (!_running)
			{
				return;
			}
		}
	}
}
//the function will get a song from the user
string Playlist::getSong(const bool songFromPlaylist)
{
	cout << "choose one song from the list: " << endl;
	int choice = 0;
	int numOfSongs = 1;
	std::vector<string> songList = {};
	if (songFromPlaylist)
	{
		songList = this->_db.getSongsOfPlaylist(this->_name);
		for (const auto& song : songList)
		{
			cout << numOfSongs << ". " + song << endl;
			numOfSongs++;
		}
	}
	else
	{
		for (const auto& file : std::filesystem::directory_iterator(SONGS_PATH))
		{
			cout << numOfSongs << ". " + file.path().filename().string() << endl;
			songList.push_back(file.path().filename().string());
			numOfSongs++;
		}
	}

	cout << numOfSongs << ".Exit" << endl;
	cout << "Your choice: ";
	cin >> choice;
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max());
		throw std::invalid_argument("Error: wrong input!!");
	}
	if (choice == numOfSongs)
	{
		return EXIT;
	}
	if (songList.size() < choice || choice <= 0)
	{
		throw std::invalid_argument("Error: wrong input!! song does not exist");
	}
	return songList[choice - 1]; // -1 because we want to convert it to an index
}

