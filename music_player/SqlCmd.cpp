#include "SqlCmd.h"
#include <iostream>


Sql::Cmd::Cmd(const std::string& cmd)
	: m_cmd(cmd)
{
}

bool Sql::Cmd::run(sqlite3* db, int (*callback)(void*, int, char**, char**), void* data)  const
{
	return executeCmd(db, m_cmd, callback, data);
}

Sql::Cmd& Sql::Cmd::withWhere(const std::string& cond)
{
	m_cmd += " WHERE " + cond;

	return *this;
}

Sql::Cmd& Sql::Cmd::innerJoin(const std::string& table, const std::string& firstCol, const std::string& secondCol)
{
	m_cmd += " INNER JOIN " + table + " ON " + firstCol + " = " + secondCol;
	return *this;
}

Sql::Cmd& Sql::Cmd::withAnd(const std::string& cond)
{
	m_cmd += " AND ";
	m_cmd += cond;

	return *this;
}

Sql::Cmd& Sql::Cmd::withOr(const std::string& cond)
{
	m_cmd += " OR ";
	m_cmd += cond;

	return *this;
}

Sql::Cmd Sql::Cmd::createTable(const std::string& tableName, const std::vector<Sql::Field>& fields, const std::optional<Key>& key)
{
	std::string cmdStr = "CREATE TABLE " + tableName;
	cmdStr += "(";

	cmdStr.reserve(1024);

	for (const Sql::Field& field : fields)
	{
		cmdStr += field.name;
		cmdStr += " " + field.type;

		for (const std::string& constrain : field.constrains)
		{
			cmdStr += " ";
			cmdStr += constrain;
		}

		cmdStr += ",";
	}
	if (key.has_value())
	{
		cmdStr += "FOREIGN KEY (" + key->key + ") REFERENCES " + key->table + '(' + key->reference + ')';
	}
	else
	{
		cmdStr = cmdStr.substr(0, cmdStr.find_last_of(","));
	}
	cmdStr += ");";

	return Sql::Cmd(cmdStr);
}

Sql::Cmd Sql::Cmd::select(const std::string& what, const std::string& from)
{
	std::string cmdStr = "SELECT ";
	cmdStr += what;

	cmdStr += " FROM ";
	cmdStr += from;

	return Sql::Cmd(cmdStr);
}

Sql::Cmd Sql::Cmd::insert(const std::string& into, const std::vector<std::string>& values)
{
	std::string cmdStr = "INSERT INTO ";
	cmdStr += into;
	cmdStr += " ";

	cmdStr += "VALUES(";

	for(const std::string& value : values)
	{
		cmdStr += value;
		cmdStr += ", ";
	}

	if(not values.empty())
	{
		// remove last ", "
		cmdStr.pop_back();
		cmdStr.pop_back();
	}

	cmdStr += ")";

	return Sql::Cmd(cmdStr);
}

Sql::Cmd Sql::Cmd::deleteQuery(const std::string& fromWhere)
{
	std::string cmdStr = "DELETE FROM " + fromWhere;
	return Sql::Cmd(cmdStr);
}

std::string Sql::Cmd::getCommand() const
{
	return this->m_cmd;
}

bool Sql::Cmd::executeCmd(sqlite3* db, const std::string& cmd, int(*callback)(void*, int, char**, char**), void* data)
{
	char* err = nullptr;

	if (sqlite3_exec(db, (cmd + ";").c_str(), callback, data, &err) != SQLITE_OK)
	{
		std::cout << err << std::endl;
		sqlite3_free(err);
		return false; 
	} 

	return true;
}

int Sql::getNamesCallback(void* data, int argc, char** argv, char** azColName)
{
	std::vector<std::string>* playlistNames = reinterpret_cast<std::vector<std::string>*>(data);

	if (argc > 0 && argv[0] != nullptr) {
		playlistNames->push_back(argv[0]);  // Add playlist name to vector
	}

	return 0;
}
