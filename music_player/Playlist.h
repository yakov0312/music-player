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

constexpr const char* songs = "Songs";

class Playlist
{
public:
	Playlist(const std::string& name);
	Playlist(Playlist&& other) noexcept;
	inline bool operator<(const Playlist& other);
	void serve();
private:
	void addSong();
	void removeSong();
	void stopToPlay();
	void playAudio() const;
	std::string getSong(const bool songFromPlaylist);

	std::string _name;
	std::vector<std::string> _songs;
	bool _shuffled;
	std::atomic<bool> _running;
};