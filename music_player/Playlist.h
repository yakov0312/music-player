//this class and file will represent a playlist 
// the class will contain all the songs as a command that can be runed for the song to be played(can be changed)
//it will have a method to add a song - the method will show the user all the songs and he will choose what to add to the playlist
//it will have another method to play the playlist and will play random songs from the playlist if the user choose so
//for it to work after the pc was shut down write the name and the songs to a file in an hidden dir 
//then in a loop create new playlist with its name that was writen to the file
#pragma once

//includes
#include <iostream>
#include <vector>
#include <atomic>
#include "SqliteDataBase.h"

constexpr const char* SONGS_PATH = "Songs";
constexpr const char* EXIT = "exit";
constexpr const char* PLAY_SONG = "play audiofile";
constexpr const char* CLOSE_SONG = "close audiofile";
constexpr const char* OPEN_SONG = "open \"{}\" type waveaudio alias audiofile";

class Playlist
{
public:
	Playlist(const std::string& name, SqliteDataBase& db);
	void serve();
private:
	void addSong();
	void removeSong();
	void stopToPlay();
	void playSong(const std::string& song) const;
	void playPlaylist() const;
	std::string getSong(const bool songFromPlaylist);

	std::string _name;
	bool _shuffled;
	SqliteDataBase& _db;
	std::atomic<bool> _running;
};

