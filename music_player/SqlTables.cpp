#include "SqlTables.h"


Sql::Playlist Sql::PLAYLIST;
Sql::Songs Sql::SONGS;

//ctor for table
Sql::Table::Table(const std::string& name)
	: m_name(name)
{
}
//the function will return the table name
const std::string& Sql::Table::tableName()
{
	return m_name;
}
//ctor for playlist table
Sql::Playlist::Playlist() : Table("PLAYLIST")
{

}
//ctor foor songs table
Sql::Songs::Songs() : Table("SONGS")
{
}
