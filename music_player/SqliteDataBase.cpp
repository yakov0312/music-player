#include "SqliteDataBase.h"
#include "io.h"
#include "SqlTables.h"
#include <iostream>
#include "SqlConstants.h"

constexpr const char* DB_NAME = "DB.sqlite";
constexpr const char* ENABLE_FOREIGN_KEY = "PRAGMA foreign_keys = ON;";
constexpr int FILE_EXISTS = 0;

SqliteDataBase::SqliteDataBase()
    : m_db(nullptr),
    m_createPlaylistTable(Sql::Cmd::createTable(Sql::PLAYLIST.tableName(),
        { {Sql::PLAYLIST.NAME, Sql::Types::String, {Sql::Constrains::NOT_NULL}},
        {Sql::PLAYLIST.ID, Sql::Types::Int, {Sql::Constrains::PRIMARY_KEY}} }, std::nullopt)),

    m_createSongTable(Sql::Cmd::createTable(Sql::SONGS.tableName(),
        { {Sql::SONGS.NAME, Sql::Types::String, {Sql::Constrains::NOT_NULL}},
        {Sql::SONGS.PLAYLIST_ID, Sql::Types::String, {Sql::Constrains::NOT_NULL}} },
        std::optional<Sql::Key>({ Sql::SONGS.PLAYLIST_ID, Sql::PLAYLIST.ID, Sql::PLAYLIST.tableName() }))),

    m_addSong(Sql::Cmd::insert(Sql::SONGS.tableName(), { Sql::SONGS.NAME, Sql::SONGS.PLAYLIST_ID }).select(Sql::PLAYLIST.ID + std::string("{}"), Sql::PLAYLIST.tableName()).withWhere(Sql::PLAYLIST.NAME + std::string(" = '{}'"))),

    m_removeSong(Sql::Cmd::deleteQuery(Sql::SONGS.tableName()).withWhere(Sql::SONGS.PLAYLIST_ID + std::string(" = (") + Sql::Cmd::select(Sql::PLAYLIST.ID, Sql::PLAYLIST.tableName()).withWhere(Sql::PLAYLIST.NAME + std::string(" = '{}'")).getCommand() + ") ").withAnd(Sql::SONGS.NAME + std::string(" = '{}'"))),

    m_getSongsOfPlaylist(Sql::Cmd::select(Sql::SONGS.NAME, Sql::SONGS.tableName()).innerJoin(Sql::PLAYLIST.tableName(), Sql::PLAYLIST.ID, Sql::SONGS.PLAYLIST_ID).withWhere(Sql::PLAYLIST.tableName() + '.' + Sql::PLAYLIST.NAME + std::string(" = '%s'"))),

    m_getPlaylists(Sql::Cmd::select(Sql::PLAYLIST.NAME, Sql::PLAYLIST.tableName())),

    m_createPlaylist(Sql::Cmd::insert(Sql::PLAYLIST.tableName(), {"'{}'"})),

    m_deletePlaylist(Sql::Cmd::deleteQuery(Sql::PLAYLIST.tableName()).withWhere(Sql::PLAYLIST.NAME + std::string(" = '{}'")))

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
        std::cout << errMsg << std::endl;
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

SqliteDataBase& SqliteDataBase::getInstance()
{
    static SqliteDataBase instance;

    return instance;
}

SqliteDataBase::~SqliteDataBase()
{
    sqlite3_close(m_db);

    m_db = nullptr;
}

void SqliteDataBase::addSongToPlaylist(const std::string& playlistName, const std::string& song)
{
    m_addSong.runFormat(m_db, song, playlistName);
}

void SqliteDataBase::removeSongFromPlaylist(const std::string& playlistName, const std::string& song)
{
    m_removeSong.runFormat(m_db, playlistName, song);
}

void SqliteDataBase::createPlaylist(const std::string& playlistName)
{
    m_createPlaylist.runFormat(m_db, playlistName);
}

void SqliteDataBase::deletePlaylist(const std::string& playlistName)
{
    m_deletePlaylist.runFormat(m_db, playlistName);
}

std::vector<std::string> SqliteDataBase::getSongsOfPlaylist(const std::string& playlistName)
{
    std::vector<std::string> songs = {};
    m_getSongsOfPlaylist.runFormat(m_db, Sql::getNamesCallback, &songs, playlistName);
}

std::vector<std::string> SqliteDataBase::getPlaylists()
{
    std::vector<std::string> playlists = {};
    m_getPlaylists.run(m_db, Sql::getNamesCallback, &playlists);
}

bool SqliteDataBase::initDB()
{
    return (m_createPlaylistTable.run(m_db)) ? m_createSongTable.run(m_db) : false;
}
