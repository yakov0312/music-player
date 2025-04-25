#pragma once

#include <string>
#include <vector>
#include <format>
#include "sqlite3.h"
#include <optional>


template<typename... Types>
inline std::string string_format(const std::string& format, Types&&... args)
{
	return std::vformat(format, std::make_format_args(std::forward<Types>(args)...));
}

namespace Sql
{
	// represnt an sql field 
	// (e.g: name = "ID", type = "INTEGER", constrains = {"PRIMARY KEY", "AUTOINCREMENT", ext...})
	struct Field 
	{
		std::string name;
		std::string type;
		std::vector<std::string> constrains;
	};

	struct Key
	{
		std::string key;
		std::string reference;
		std::string table;
	};

	// a class that represent an sql command to the database
	class Cmd
	{
	public:
		Cmd(const std::string& cmd);

		// execute the command
		bool run(sqlite3* db, int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr) const;

		// execute the command, with format parametrs (e.g %s, %d, %f, ext.)
		template<typename... Types>
		inline bool runFormat(sqlite3* db, int(*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr, Types&&... args) const;

		// append a where cmd to the end of this cmd
		Cmd& withWhere(const std::string& cond);
		//inner join table
		Cmd& innerJoin(const std::string& table, const std::string& firstCol, const std::string& secondCol);
		// and cond
		Cmd& withAnd(const std::string& cond);
		// or cond
		Cmd& withOr(const std::string& cond);
		
		// create "CREATE TABLE tableName(fields...)" cmd
		static Cmd createTable(const std::string& tableName, const std::vector<Sql::Field>& fields, const std::optional<Key>& key);

		// create "SELECT (what) FROM from" cmd
		static Cmd select(const std::string& what, const std::string& from, const std::optional<std::string>& appendCommand);


		// create "INSERT INTO into VALUES(values...)" cmd
		static Cmd insert(const std::string& into, const std::vector<std::string>& valuesName, const std::vector<std::string>& values);

		static Cmd deleteQuery(const std::string& fromWhere);

		std::string getCommand() const;

	private:
		// the current cmd string
		std::string m_cmd;

		static bool executeCmd(sqlite3* db, const std::string& cmd,
			int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr);
	};


	template<typename... Types>
	inline bool Cmd::runFormat(sqlite3* db, int(*callback)(void*, int, char**, char**), void* data, Types&&... args) const
	{
		std::string query = string_format(m_cmd, std::forward<Types>(args)...); // Format the command
		return executeCmd(db, query, callback, data); // Execute the query with the formatted string
	}

	int getNamesCallback(void* data, int argc, char** argv, char** azColName);
}

