---
layout: post
title: ODBC Setup in Linux
date: 2015-08-09
categories: linux
---

ODBC setup and simple examples in linux
=======================================

####Installation ArchLinux packages

Installing odbc

```sh
sudo pacman -S unixodbc
```

Installing drivers
1. mysql - mariadb version

```sh
sudo pacman -S myodbc
```


2. postgres

```sh
sudo pacman -S psqlodbc
```


3. sqlite from AUR
```sh
yaourt -S sqliteodbc
```

####Local ODBC setup
configurations for odbc drivers are written in /etc/odbcinst.ini file

```ini
[PostgreSQL]
Description     = ODBC driver for postgres
Driver          = /usr/lib/psqlodbcw.so
FileUsage       = 1

[MySQL]
Description     = ODBC driver for mariaDB
Driver          = /usr/lib/libmyodbc.so
Setup           = /usr/lib/libmyodbc5S.so
FileUsage       = 1

[FreeTDS]
Description     = ODBC driver for Microsoft SQL
Driver          = /usr/lib/libtdsodbc.so
UsageCount      = 1
```

configuration for odbc sources are in /etc/odbc.ini file

```ini
[mariadb-connector]
Description 	= connection to mariadb version of mysql
Driver 		= MySQL
Database 	= test
Server 		= 127.0.0.1
UserName 	= keutoi
Trace 		= No
Port 		= 3306

[psql-connector]
Description 	= connection to postgres
Driver 		= PostgreSQL
Database 	= test
Server 		= 127.0.0.1
Port 		= 5432
UserName 	= keutoi
Trace 		= No
```

####Testing the installation
isql is a CLI for unixodbc

*_Postgres_*

```sh
isql -v psql-connector
```

connects using psql-connector source specification in /etc/odbc.ini or in local ~/.odbc.ini(i'm not
sure of overrides) and opens a sql like prompt to execute sql commands.

```sh
echo "select * from mytable" |isql -v psql-connector
```

gives output

    +---------------------------------------+
    | Connected!                            |
    |                                       |
    | sql-statement                         |
    | help [tablename]                      |
    | quit                                  |
    |                                       |
    +---------------------------------------+
    SQL> select * from mytable
    +-----------------------------------------+------------+
    | name                                    | id         |
    +-----------------------------------------+------------+
    | keutoi0                                 | 0          |
    | keutoi1                                 | 1          |
    +-----------------------------------------+------------+
    SQLRowCount returns 2
    2 rows fetched
    SQL>

*_MySQL_*

myodbc driver was not working in my system. [some segmentation errors in driver
lib](http://www.howtobuildsoftware.com/index.php/how-do/bkD6/mysql-odbc-asterisk-mariadb-isql-mariadb-odbc-throws-invalid-pointer-error).
I had to downgrade myodbc pkg from 5.3.4-1 to 5.2.6-1 to get it working

```sh
echo "select * from mytable" | isql -v mariadb-connector
```

TODO: freetds, sqlite testing

####Sample C++ Interfaces

Get list of drivers and datasources.
[reference][easysoft]

```c

#include <stdio.h>
#include <sql.h>
#include <sqlext.h>

int main() {
    SQLHENV env;
    SQLCHAR driver[256];
    SQLCHAR attr[256];
    SQLSMALLINT driver_ret;
    SQLSMALLINT attr_ret;
    SQLUSMALLINT direction;
    SQLRETURN ret;

    /*
     *  Initializing lib vars
     */
    SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);

    direction = SQL_FETCH_FIRST;
    /*
     *  Get the list of available drivers.
     *  This is equivalent to odbcinst -q -d
     */
    while(SQL_SUCCEEDED(ret = SQLDrivers(env, direction, driver, sizeof(driver), &driver_ret,  attr, sizeof(attr), &attr_ret)))
    {
        direction = SQL_FETCH_NEXT;
        printf("%s - %s\n", driver, attr);
        if (ret == SQL_SUCCESS_WITH_INFO) printf("\tdata truncation\n");
    }

    SQLCHAR dsn[256];
    SQLCHAR desc[256];
    SQLSMALLINT dsn_ret;
    SQLSMALLINT desc_ret;


    /*
     * Get the list of available data sources.
     * This is equivalent to odbcinst -q -s
     */
    while(SQL_SUCCEEDED(ret = SQLDataSources(env, direction, dsn, sizeof(dsn), &dsn_ret, desc, sizeof(desc), &desc_ret)))
    {
        direction = SQL_FETCH_NEXT;
        printf("%s - %s\n", dsn, desc);
        if (ret == SQL_SUCCESS_WITH_INFO) printf("\tdata truncation\n");
    }
    return 0;
}
```

Simple Query Example

```c

#include<stdio.h>
#include<sql.h>
#include<sqlext.h>
#include<exception>
#include<iostream>

#define NAME_LEN 50
#define ID_LEN 20

void show_error() {
    printf("SQL fetch Error");
}
/**
 * @brief error handling for SQL* functions
 */
void check_error(SQLRETURN ret_val, std::string act_str)
{
    switch (ret_val)
    {
        case SQL_SUCCESS:
        case SQL_SUCCESS_WITH_INFO:
            break;
        case SQL_NO_DATA:
            std::cerr << "end of data\n";
            break;
        default:
            throw std::runtime_error(act_str);
    }
}

int main() {
    SQLHENV henv;
    SQLHDBC hdbc;
    SQLHSTMT hstmt = 0;
    SQLRETURN retcode;
    SQLCHAR outstr[1024];
    SQLSMALLINT outstrlen;

    SQLCHAR szName[NAME_LEN], szID[ID_LEN];
    SQLLEN cbName = 0, cbID = 0;

    try {
        //Allocate environment handle
        retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
        check_error(retcode, "Allocate Environment Handle");
        //Set the ODBC version environment attribute
        retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0);
        check_error(retcode, "Set ODBC version enviroment attribute");
        //Allocate connection handle
        retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
        check_error(retcode, "Allocate connection Handle");
        //Set login timeout to 5 seconds
        SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0);
        check_error(retcode, "set login timeout attribute");

        /**
         * Connect to data source
         */
        retcode = SQLDriverConnect(hdbc, NULL, (SQLCHAR*)"DSN=mariadb-connector;", SQL_NTS, outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE);
        check_error(retcode, "connect to the data source");

        //Allocate statement handle
        retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
        check_error(retcode, "allocate a statement handle");

        /**
         * statement to be executed.
         */
        retcode = SQLExecDirect (hstmt, (SQLCHAR *) "select * from mytable", SQL_NTS);
        check_error(retcode, "execute the statement");

        /**
         * Bind a column to a variable
         */
        retcode = SQLBindCol(hstmt, 1, SQL_C_CHAR, szName, NAME_LEN, &cbName);
        check_error(retcode, "bind 1 column to the statement");
        retcode = SQLBindCol(hstmt, 2, SQL_C_CHAR, szID, ID_LEN, &cbID);
        check_error(retcode, "bind 2 column to the statement");

        /**
         * fetch sql hstmt untill there is no data and print
         */
        for (int i=0 ; ; i++)
        {
            retcode = SQLFetch(hstmt);
            if(retcode == SQL_NO_DATA)break;
            else printf( "%d: %s %s %s\n", i + 1, szID, szName);
        }

        SQLCancel(hstmt);
        SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

        SQLDisconnect(hdbc);
        SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
        SQLFreeHandle(SQL_HANDLE_ENV, henv);
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }
    return 0;
}
```

###Reference Links
[siva]: https://compscinotes.wordpress.com/2010/04/18/unixodbc-mysql-sample-program/
[easysoft]: http://www.easysoft.com/developer/languages/c/odbc_tutorial.html
[raosoft]: http://www.raosoft.com/ezsurvey/help/2007/odbc_in_unix.html


