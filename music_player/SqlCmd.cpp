#include "SqlCmd.h"
#include <iostream>

using std::endl, std::cout, std::string;

//ctor for cmd
Sql::Cmd::Cmd(const string& cmd)
	: m_cmd(cmd)
{
}
//the function will run the command
bool Sql::Cmd::run(sqlite3* db, int (*callback)(void*, int, char**, char**), void* data)  const
{
	return executeCmd(db, m_cmd, callback, data);
}
//the function will append where to the command
Sql::Cmd& Sql::Cmd::withWhere(const string& cond)
{
	m_cmd += " WHERE " + cond;

	return *this;
}
//the function will append inner join to the command
Sql::Cmd& Sql::Cmd::innerJoin(const string& table, const string& firstCol, const string& secondCol)
{
	m_cmd += " INNER JOIN " + table + " ON " + firstCol + " = " + secondCol;
	return *this;
}
//the function will append and to the command 
Sql::Cmd& Sql::Cmd::withAnd(const string& cond)
{
	m_cmd += " AND ";
	m_cmd += cond;

	return *this;
}
//the funtion will append or to the command
Sql::Cmd& Sql::Cmd::withOr(const string& cond)
{
	m_cmd += " OR ";
	m_cmd += cond;

	return *this;
}
//the function will create a cmd for create table 
Sql::Cmd Sql::Cmd::createTable(const string& tableName, const std::vector<Sql::Field>& fields, const std::optional<Key>& key)
{
	string cmdStr = "CREATE TABLE " + tableName;
	cmdStr += "(";

	cmdStr.reserve(1024);

	for (const Sql::Field& field : fields)
	{
		cmdStr += field.name;
		cmdStr += " " + field.type;

		for (const string& constrain : field.constrains)
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
//the function will create a select command 
Sql::Cmd Sql::Cmd::select(const string& what, const string& from, const std::optional<string>& appendCommand)
{
	string cmdStr = "";
	if (appendCommand.has_value())
		cmdStr = appendCommand.value() + " ";
	cmdStr += "SELECT ";
	cmdStr += what;

	cmdStr += " FROM ";
	cmdStr += from;

	return Sql::Cmd(cmdStr);
}
//the function will create an insert command
Sql::Cmd Sql::Cmd::insert(const string& into, const std::vector<string>& valuesName, const std::vector<string>& values)
{
	string cmdStr = "INSERT INTO ";
	cmdStr += into;
	cmdStr += " (";
	for (const string& name : valuesName)
	{
		cmdStr += name;
		cmdStr += ", ";
	}
	cmdStr.pop_back();
	cmdStr.pop_back();
	cmdStr += ") ";

	if(values.size() == 0)
		return Sql::Cmd(cmdStr);// this check is used to make advanced insert queries

	cmdStr += "VALUES(";

	for(const string& value : values)
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
//the function will create a del command
Sql::Cmd Sql::Cmd::deleteQuery(const string& fromWhere)
{
	string cmdStr = "DELETE FROM " + fromWhere;
	return Sql::Cmd(cmdStr);
}
//this function will return the command that is stored 
string Sql::Cmd::getCommand() const
{
	return this->m_cmd;
}
//the function will exec the command
bool Sql::Cmd::executeCmd(sqlite3* db, const string& cmd, int(*callback)(void*, int, char**, char**), void* data)
{
	char* err = nullptr;

	if (sqlite3_exec(db, (cmd + ";").c_str(), callback, data, &err) != SQLITE_OK)
	{
		std::cout << err << endl;
		sqlite3_free(err);
		return false; 
	} 

	return true;
}
//callback function from the select command - will return the names
int Sql::getNamesCallback(void* data, int argc, char** argv, char** azColName)
{
	std::vector<string>* playlistNames = reinterpret_cast<std::vector<string>*>(data);

	for (int i = 0; i < argc; i++)
	{
		playlistNames->push_back(argv[i]);
	}

	return 0;
}
