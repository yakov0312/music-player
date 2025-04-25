#pragma once

#include <string>

#define CS constexpr const char*

#define NameField(name) CS name = #name
namespace Sql
{
	namespace Constrains 
	{
		CS NOT_NULL = "NOT NULL";
		CS PRIMARY_KEY = "PRIMARY KEY";
		CS UNIQUE = "UNIQUE";
		CS FOREIGN_KEY = "FOREIGN KEY";
		NameField(AUTOINCREMENT);

	}
	namespace Types 
	{
		CS Int = "INTEGER";
		CS String = "TEXT";
	}
}