#include "SqlTables.h"

Sql::Users Sql::USERS;
Sql::Questions Sql::QUESTIONS;
Sql::Statistics Sql::STATS;

Sql::Table::Table(const std::string& name)
	: m_name(name)
{
}

const std::string& Sql::Table::tableName()
{
	return m_name;
}

Sql::Users::Users()
	: Table("USERS")
{
}

Sql::Questions::Questions() : Table("QUESTIONS")
{
}

Sql::Statistics::Statistics() : Table("STATISTICS")
{
}
