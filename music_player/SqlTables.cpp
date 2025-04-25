#include "SqlTables.h"


Sql::Playlist Sql::PLAYLIST;
Sql::Songs Sql::SONGS;

Sql::Table::Table(const std::string& name)
	: m_name(name)
{
}

const std::string& Sql::Table::tableName()
{
	return m_name;
}

Sql::Playlist::Playlist() : Table("PLAYLIST")
{

}

Sql::Songs::Songs() : Table("SONGS")
{
}
