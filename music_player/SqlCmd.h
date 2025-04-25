#pragma once

#include <string>
#include <vector>
#include "sqlite3.h"
#include <optional>

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
		template <typename... Types>
		bool runFormat(sqlite3* db, Types... args) const;

		template <typename... Types>
		bool runFormat(sqlite3* db, int (*callback)(void*, int, char**, char**), Types... args) const;

		template <typename... Types>
		bool runFormat(sqlite3* db, int (*callback)(void*, int, char**, char**), void* data, Types... args) const;

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
		static Cmd select(const std::string& what, const std::string& from);


		// create "INSERT INTO into VALUES(values...)" cmd
		static Cmd insert(const std::string& into, const std::vector<std::string>& values);

		static Cmd deleteQuery(const std::string& fromWhere);

		std::string getCommand() const;

	private:
		// the current cmd string
		std::string m_cmd;

		static bool executeCmd(sqlite3* db, const std::string& cmd,
			int (*callback)(void*, int, char**, char**) = nullptr, void* data = nullptr);
	};

	template<typename... Types>
	inline bool Cmd::runFormat(sqlite3* db, Types... args) const
	{
		return runFormat(db, (int(*)(void*, int, char**, char**))nullptr, (void*)nullptr, args...);
	}

	template<typename... Types>
	inline bool Cmd::runFormat(sqlite3* db, int(*callback)(void*, int, char**, char**), Types... args) const
	{
		return runFormat(db, callback, nullptr, args...);
	}

	template<typename... Types>
	inline bool Cmd::runFormat(sqlite3* db, int(*callback)(void*, int, char**, char**), void* data, Types... args) const
	{
		return executeCmd(db, string_format(m_cmd, args...), callback, data);
	}

	int getNamesCallback(void* data, int argc, char** argv, char** azColName);
}