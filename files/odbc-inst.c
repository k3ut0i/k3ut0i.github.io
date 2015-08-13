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


    direction = SQL_FETCH_FIRST;
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
