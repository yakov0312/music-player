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

class Playlist
{
public:
	Playlist(const std::string& name, SqliteDataBase& db);
	Playlist(Playlist&& other) noexcept;
	bool operator<(const Playlist& other) const;	
	bool operator==(const std::string& otherName) const; // to use with std::find
	friend std::ostream& operator<<(std::ostream& os, const Playlist& playlist);
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

