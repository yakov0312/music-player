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

Playlist::Playlist(const string& name, SqliteDataBase& db) : _name(name), _shuffled(false), _running(true), _db(db)
{
	//to do: get shuffled from db
}

Playlist::Playlist(Playlist&& other) noexcept :
	_name(std::move(other._name)), _db(other._db)
{
	_shuffled = other._shuffled;
	_running.store(other._running.load());
}

bool Playlist::operator<(const Playlist& other) const
{
	return (this->_name < other._name);
}

bool Playlist::operator==(const string& otherName) const
{
	return this->_name == otherName;
}

void Playlist::serve()
{
	int choice = 0;
	while (choice != 5)
	{
		cout << "playlist name: " << this->_name << "\nChoose one of the options below:\n1.Add a song\n2.Remove a song\n3.Play the playlist" << endl;
		cout << ((this->_shuffled) ? "4.Turn shuffled mode off\n5.exit\n" : "4.Turn shuffled mode on\n5.exit\n");

		cin >> choice;
		if (cin.fail())
		{
			cin.clear();
			cin.ignore(std::numeric_limits<std::streamsize>::max());
			cout << "wrong input!!!" << endl;
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
			continue;
		}
		else
		{
			cout << "wrong input! " << endl;
		}
	}
}

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
		if (song == "exit")
		{
			break;
		}
		this->_db.addSongToPlaylist(this->_name, song);
	} while (true);
}

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
		if (song == "exit")
		{
			break;
		}

		this->_db.removeSongFromPlaylist(this->_name, song);
	} while (true);
}

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

// Function to play a song asynchronously
void Playlist::playSong(const string& song) const
{
	// Open the audio file
	string openCommand = "open \"" + song + "\" type waveaudio alias audiofile";
	mciSendStringA(openCommand.c_str(), NULL, 0, NULL);

	mciSendStringA("play audiofile", NULL, 0, NULL); 

	while (this->_running)
	{
		Sleep(100);  // Sleep for a short period to avoid high cpu usage
	}

	mciSendStringA("close audiofile", NULL, 0, NULL);
}

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

	cout << std::to_string(numOfSongs) << ". exit" << endl;
	cout << "your choice: ";
	cin >> choice;
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max());
		throw std::invalid_argument("Error: wrong input!!");
	}
	if (choice == numOfSongs)
	{
		return "exit";
	}
	if (songList.size() < choice || choice <= 0)
	{
		throw std::invalid_argument("Error: wrong input!! song does not exist");
	}
	return songList[choice - 1]; // -1 because we want to convert it to an index
}

std::ostream& operator<<(std::ostream& os, const Playlist& playlist)
{
	return os << playlist._name;
}
