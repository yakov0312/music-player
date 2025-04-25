#pragma once

#include <string>

#include "SqlConstants.h"


namespace Sql
{
	class Table 
	{
	public:
		Table(const std::string& name);

		const std::string& tableName();

	private:
		const std::string m_name;
	};

	class Playlist : public Table 
	{
	public:
		Playlist();
		
		static CS NAME = "NAME";
		static CS ID = "ID";
	};

	class Songs : public Table
	{
	public:
		Songs();

		static CS NAME = "NAME";
		static CS PLAYLIST_ID = "PLAYLIST_ID";
	};

	CS ALL_FIELDS = "*";

	extern Playlist PLAYLIST;
	extern Songs SONGS;
}