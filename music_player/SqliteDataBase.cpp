#include "SqliteDataBase.h"
#include "io.h"
#include "SqlTables.h"
#include <iostream>
#include "SqlConstants.h"

using std::string, std::endl;
//ctor for sqliteDatabaase - will create and store the commands that will be used
SqliteDataBase::SqliteDataBase()
    : m_db(nullptr),
    m_createPlaylistTable(Sql::Cmd::createTable(Sql::PLAYLIST.tableName(),
        { {Sql::PLAYLIST.NAME, Sql::Types::String, {Sql::Constrains::NOT_NULL}},
        {Sql::PLAYLIST.ID, Sql::Types::Int, {Sql::Constrains::PRIMARY_KEY, Sql::Constrains::UNIQUE}} }, std::nullopt)),

    m_createSongTable(Sql::Cmd::createTable(Sql::SONGS.tableName(),
        { {Sql::SONGS.NAME, Sql::Types::String, {Sql::Constrains::NOT_NULL}},
        {Sql::SONGS.PLAYLIST_ID, Sql::Types::String, {Sql::Constrains::NOT_NULL}} },
        std::optional<Sql::Key>({ Sql::SONGS.PLAYLIST_ID, Sql::PLAYLIST.ID, Sql::PLAYLIST.tableName() }))),

    m_addSong(Sql::Cmd::select(string("'{}', ") + Sql::PLAYLIST.ID, Sql::PLAYLIST.tableName(), Sql::Cmd::insert(Sql::SONGS.tableName(), { Sql::SONGS.NAME, Sql::SONGS.PLAYLIST_ID }, {}).getCommand()).withWhere(Sql::PLAYLIST.NAME + string(" = '{}'"))),

    m_removeSong(Sql::Cmd::deleteQuery(Sql::SONGS.tableName()).withWhere(Sql::SONGS.PLAYLIST_ID + string(" = (") + Sql::Cmd::select(Sql::PLAYLIST.ID, Sql::PLAYLIST.tableName(), std::nullopt).withWhere(Sql::PLAYLIST.NAME + string(" = '{}'")).getCommand() + ") ").withAnd(Sql::SONGS.NAME + string(" = '{}'"))),

    m_getSongsOfPlaylist(Sql::Cmd::select(Sql::SONGS.tableName() + '.' + Sql::SONGS.NAME, Sql::SONGS.tableName(), std::nullopt).innerJoin(Sql::PLAYLIST.tableName(), Sql::PLAYLIST.ID, Sql::SONGS.PLAYLIST_ID).withWhere(Sql::PLAYLIST.tableName() + '.' + Sql::PLAYLIST.NAME + string(" = '{}'"))),

    m_getPlaylists(Sql::Cmd::select(Sql::PLAYLIST.NAME, Sql::PLAYLIST.tableName(), std::nullopt)),

    m_createPlaylist(Sql::Cmd::insert(Sql::PLAYLIST.tableName(), { Sql::PLAYLIST.NAME } , { "'{}'" })),

    m_deletePlaylist(Sql::Cmd::deleteQuery(Sql::PLAYLIST.tableName()).withWhere(Sql::PLAYLIST.NAME + string(" = '{}'")))

{
    bool doesFileExists = _access(DB_NAME, 0) == FILE_EXISTS;

    int res = sqlite3_open(DB_NAME, &m_db);

    if (res != SQLITE_OK)
    {
        return;
    }

    char* errMsg = nullptr;

    if (sqlite3_exec(m_db, ENABLE_FOREIGN_KEY, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::cout << errMsg << endl;
        sqlite3_free(errMsg);
        return;
    }

    if (!doesFileExists)
    {
        if (!initDB())
        {
            return;
        }
    }
}
//the function will return the instance of the class
SqliteDataBase& SqliteDataBase::getInstance()
{
    static SqliteDataBase instance;

    return instance;
}
//dtor for sqliteDatabase
SqliteDataBase::~SqliteDataBase()
{
    sqlite3_close(m_db);

    m_db = nullptr;
}
//the function will add a song to the db
void SqliteDataBase::addSongToPlaylist(const string& playlistName, const string& song)
{
    m_addSong.runFormat(m_db, nullptr, nullptr, song, playlistName);
}
//the function will delete song from a playlist 
void SqliteDataBase::removeSongFromPlaylist(const string& playlistName, const string& song)
{
    m_removeSong.runFormat(m_db, nullptr, nullptr, playlistName, song);
}
//the function will create a playlist
void SqliteDataBase::createPlaylist(const string& playlistName)
{
    m_createPlaylist.runFormat(m_db, nullptr, nullptr, playlistName);
}
//the function will delete a playlist
void SqliteDataBase::deletePlaylist(const string& playlistName)
{
    m_deletePlaylist.runFormat(m_db, nullptr, nullptr, playlistName);
}
//the function will return the songs from a playlist
std::vector<string> SqliteDataBase::getSongsOfPlaylist(const string& playlistName)
{
    std::vector<string> songs = {};
    m_getSongsOfPlaylist.runFormat(m_db, Sql::getNamesCallback, &songs, playlistName);
    return songs;
}
//the function will return all the playlists of the user
std::vector<string> SqliteDataBase::getPlaylists()
{
    std::vector<string> playlists = {};
    m_getPlaylists.run(m_db, Sql::getNamesCallback, &playlists);
    return playlists;
}
//the function will init the db
bool SqliteDataBase::initDB()
{
    return (m_createPlaylistTable.run(m_db)) ? m_createSongTable.run(m_db) : false;
}
