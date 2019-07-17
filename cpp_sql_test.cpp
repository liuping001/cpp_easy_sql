#include <stdlib.h>
#include <iostream>

#include "cpp_sql.h"
#include "test_sql.h"

using namespace std;

int main(void)
{
    try {
        sql::Driver *driver;
        sql::Connection *con;

        driver = get_driver_instance();
        con = driver->connect("tcp://192.168.101.200:3306", "user00", "Iron@vgo");
        con->setSchema("test");
        CppSql cpp_sql;
        auto stmt = con->createStatement();
        cpp_sql.Init(std::unique_ptr<sql::Statement>(stmt));
        for (int i = 3; i < 10 ;i++) {
            std::cout << vg_account::AddUserResultByName(&cpp_sql, {"liuping" + std::to_string(i), "passwd1", 1});
        }

        delete con;
    } catch (sql::SQLException &e) {
        cout << "# ERR: SQLException in " << __FILE__;
        cout << "(" << __FUNCTION__ << ") on line "
             << __LINE__ << endl;
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << endl;

    return EXIT_SUCCESS;
}