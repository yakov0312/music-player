#pragma once
#include "sqlite3.h"
#include "SqlCmd.h"
#include <vector>



class SqliteDataBase
{
public:
	SqliteDataBase(SqliteDataBase const&) = delete;
	void operator=(SqliteDataBase const&) = delete;

	static SqliteDataBase& getInstance();

	~SqliteDataBase();
	void addSongToPlaylist(const std::string& playlistName, const std::string& song);
	void removeSongFromPlaylist(const std::string& playlistName, const std::string& song);
	void createPlaylist(const std::string& playlistName);
	void deletePlaylist(const std::string& playlistName);
	std::vector<std::string> getSongsOfPlaylist(const std::string& playlistName);
	std::vector<std::string> getPlaylists();

private:
	sqlite3* m_db;

	const Sql::Cmd m_addSong;
	const Sql::Cmd m_removeSong;

	const Sql::Cmd m_getSongsOfPlaylist;
	const Sql::Cmd m_getPlaylists;

	const Sql::Cmd m_createPlaylist;
	const Sql::Cmd m_deletePlaylist;

	const Sql::Cmd m_createPlaylistTable;
	const Sql::Cmd m_createSongTable;

	SqliteDataBase();
	bool initDB();

};