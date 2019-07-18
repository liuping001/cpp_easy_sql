#include <stdlib.h>
#include <iostream>

#include "cpp_sql.h"
#include "test/test_sql.h"

using namespace std;

int main(void)
{
    try {
        sql::Driver *driver;

        driver = get_driver_instance();
        std::unique_ptr<sql::Connection> con(driver->connect("tcp://192.168.101.200:3306", "user00", "Iron@vgo"));
        con->setSchema("test");
        CppSql cpp_sql;
        auto stmt = con->createStatement();
        cpp_sql.Init(std::unique_ptr<sql::Statement>(stmt));

        // 获取所有数据并删除
        auto user_info = vg_account::GetUserInfo(&cpp_sql, {});
        for (const auto &item : user_info) {
            std::cout << item.DebugString() <<"\n";
            std::cout << "del user_id:" << item.user_id << " result:"
            <<vg_account::DeleteUser(&cpp_sql, {item.user_id}) << "\n";
        }

        for (int i = 0; i < 10 ;i++) {
            auto name = "user00" + std::to_string(i);
            std::cout << "add user:" << name << " reslut:"<< vg_account::AddUser(&cpp_sql, {name, "passwd_old", 1}) <<"\n";
        }

        std::cout << "\n----------------\n";
        auto user_info1 = vg_account::GetUserInfo(&cpp_sql, {});
        for (const auto &item : user_info1) {
            std::cout << item.DebugString() <<"\n";
            std::cout << "update :" << vg_account::UpdateUserInfoByName(&cpp_sql, {"passwd_new", 100, item.name}) <<"\n";
        }

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