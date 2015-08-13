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
