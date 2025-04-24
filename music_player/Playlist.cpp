#include "Playlist.h"
#define NOMINMAX
#include <limits>
#include <fstream>
#include <filesystem>
#include <thread>
#include "Windows.h"

#pragma comment(lib, "winmm.lib")

using std::cout;
using std::endl;
using std::string;
using std::cin;

Playlist::Playlist(const std::string& name) : _name(name), _songs(), _shuffled(false), _running(true)
{
	std::ifstream file( this->_name + ".txt");
	std::string song = "";
	if (file.is_open())
	{

		while (std::getline(file, song))
		{
			this->_songs.push_back(song);
		}
	}
	file.close();
}

Playlist::Playlist(Playlist&& other) noexcept :
	_name(std::move(other._name)), _songs(std::move(other._songs))
{
	_shuffled = other._shuffled;
	_running.store(other._running.load());
}

bool Playlist::operator<(const Playlist& other)
{
	return this->_name < other._name;
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
			std::thread stopPlay(&Playlist::stopToPlay, this);
			std::thread playAudio(&Playlist::playAudio, this);
			playAudio.join();
			stopPlay.join();
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
	std::string song = "";
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
		this->_songs.push_back(song);
		std::ofstream playlist(song + ".txt");
		if (!playlist.is_open())
		{
			throw std::exception("Cant open a file");
		}
		playlist << song << endl;
	} while (true);
}

void Playlist::removeSong()
{
	std::string song = "";
	do
	{
		try
		{
			song = this->getSong(true);
		}
		catch (const std::invalid_argument& e)
		{
			cout << e.what() << endl;
		}
		if (song == "exit")
		{
			break;
		}

		this->_songs.erase(std::find(this->_songs.begin(), this->_songs.end(), song));
		//to do: earse it from the file too
	} while (true);
}

void Playlist::stopToPlay()
{
	string stop = "";
	cout << "enter exit to stop the audio" << endl;
	while (this->_running)
	{
		std::getline(cin, stop);
		if (stop == "exit")
		{
			this->_running = false;
		}
	}
}

void Playlist::playAudio() const
{
	std::srand(std::time(0));
	int size = this->_songs.size();
	int i = 0;
	int lastChoice = 0;
	while (this->_running)
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
			mciSendStringA(std::string("open \"" + this->_songs[i] + "\" type waveaudio alias audiofile").c_str(), NULL, 0, NULL);
			mciSendStringA("play audiofile wait", NULL, 0, NULL);
			mciSendStringA("close audiofile", NULL, 0, NULL);
		}
	}
}

std::string Playlist::getSong(const bool songFromPlaylist)
{
	cout << "choose one song from the list: " << endl;
	int choice = 0;
	int numOfSongs = 1;
	std::vector<std::string> songList = {};
	if (songFromPlaylist)
	{
		for (const auto& song : this->_songs)
		{
			cout << numOfSongs << ". " + song << endl;
			numOfSongs++;
		}
	}
	else
	{
		for (const auto& file : std::filesystem::directory_iterator(songs))
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
	if ((songFromPlaylist && this->_songs.size() < choice) || (!songFromPlaylist && songList.size() < choice) || choice <= 0)
	{
		throw std::invalid_argument("Error: wrong input!! song does not exist");
	}
	return (songFromPlaylist) ? this->_songs[choice -1] : songList[choice - 1]; // -1 because we want to convert it to an index
}
