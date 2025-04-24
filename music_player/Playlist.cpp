#include "Playlist.h"
#include <limits>
#include <fstream>
#define NOMINMAX
#include "Windows.h"

using std::cout;
using std::endl;
using std::string;
using std::cin;

Playlist::Playlist(const std::string& name) : _name(name), _songs(), _shuffled(false), _running(true)
{
	std::wifstream file(this->_name);
	std::wstring song = L"";
	if (file.is_open())
	{

		while (std::getline(file, song))
		{
			this->_songs.insert(song);
		}
	}
	file.close();
}

Playlist::~Playlist()
{
	//here write the play list to a file 
	std::wofstream file(this->_name);
	if(file.is_open())
	{
		
		for (const auto& it : this->_songs)
		{
			file << it << endl;
		}
	}
	else
	{
		cout << "cannot open the file playlist will not be saved" << endl;
	}
	file.close();
}

Playlist::Playlist(Playlist&& other) noexcept :
	_name(std::move(other._name)), _songs(std::move(other._songs))
{
	_shuffled = other._shuffled;
	_running.store(other._running.load());
}

void Playlist::serve()
{
	cout << "playlist name: " << this->_name << "\nChoose one of the options below:\n1.Add a song\n2.Remove a song\n3.Play the playlist" << endl;
	cout << (this->_shuffled) ? "4.Turn shuffled mode off\n5.exit\n" : "4.Turn shuffled mode on\n5.exit\n";
	int choice = 0;
	while (choice != 5)
	{
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
	std::wstring song = L"";
	do
	{
		try
		{
			song = this->getSong();
		}
		catch (const std::invalid_argument& e)
		{
			cout << e.what() << endl;;
		}
		if (song == L"exit")
		{
			break;
		}
		std::wstring songCommand = L"open \"" + song + L"\" type waveaudio alias audiofile";
		this->_songs.insert(songCommand);
	} while (true);
}

void Playlist::removeSong()
{
	std::wstring song = L"";
	do
	{
		try
		{
			song = this->getSong();
		}
		catch (const std::invalid_argument& e)
		{
			cout << e.what() << endl;;
		}
		if (song == L"exit")
		{
			break;
		}
		std::wstring songCommand = L"open \"" + song + L"\" type waveaudio alias audiofile";
		this->_songs.erase(songCommand);
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
			const auto& it = this->_songs.begin();
			std::advance(it, i);
			mciSendStringW((*it).c_str(), NULL, 0, NULL);
			mciSendStringW(L"play audiofile wait", NULL, 0, NULL);
			mciSendStringW(L"close audiofile", NULL, 0, NULL);
		}
	}
}

std::wstring Playlist::getSong()
{
	cout << "choose one song from the list: " << endl;
	std::map<int, std::wstring> list = {};
	int choice = 0;
	int i = 1;
	for (const auto& file : std::filesystem::directory_iterator(songs))
	{
		cout << i << file.path().filename().string() << endl;
		list.insert(std::make_pair(i, file.path().filename().wstring()));
		i++;
	}
	cout << std::to_string(i) << ". exit" << endl;
	cout << "your choice: ";
	cin >> choice;
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(std::numeric_limits<std::streamsize>::max());
		throw std::invalid_argument("Error: wrong input!!");
	}
	if (choice == i)
	{
		return L"exit";
	}
	if (list.find(choice) == list.end())
	{
		throw std::invalid_argument("Error: wrong input!! song does not exist");
	}
	return list.find(choice)->second;
}
