//
// Created by liuping on 2019/7/21.
//

#include <stdlib.h>
#include <iostream>

#include "c_sql.h"
#include "test/test_sql.h"

using namespace std;

int main(void) {

    MYSQL conn;
    mysql_init(&conn);
    mysql_real_connect(&conn, "127.0.0.1", "root", "", "test", 3306, nullptr, 0);

    CSql c_sql(conn);

    // 获取所有数据并删除
    auto user_info = vg_account::GetUserInfo(&c_sql, {});
    for (const auto &item : user_info) {
        std::cout << item.DebugString() << "\n";
        std::cout << "del user_id:" << item.user_id << " result:"
                  << vg_account::DeleteUser(&c_sql, {item.user_id}) << "\n";
    }

    for (int i = 0; i < 10; i++) {
        auto name = "user00" + std::to_string(i);
        std::cout << "add user:" << name << " reslut:" << vg_account::AddUser(&c_sql, {name, "passwd_old", 1})
                  << "\n";
    }

    std::cout << "\n----------------\n";
    auto user_info1 = vg_account::GetUserInfo(&c_sql, {});
    for (const auto &item : user_info1) {
        std::cout << item.DebugString() << "\n";
        std::cout << "update :" << vg_account::UpdateUserInfoByName(&c_sql, {"passwd_new", 100, item.name}) << "\n";
    }

    cout << endl;

    return EXIT_SUCCESS;
}
