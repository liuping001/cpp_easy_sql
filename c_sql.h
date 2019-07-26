//
// Created by liuping on 2019/7/21.
//

#pragma once

#include <memory>
#include <string>
#include "sql_handler.h"
#include "mysql/mysql.h"

class CSql : public ColumnIndexSqlHandler {
    MYSQL &mysql_;
    std::shared_ptr<MYSQL_RES> res;
    MYSQL_ROW row;
public:
    CSql(MYSQL &mysql) : mysql_(mysql) {}
    virtual void        ExecuteQuery(const std::string &sql) {
        auto ret = mysql_query(&mysql_, sql.c_str());
        if (!ret) {
            res = std::shared_ptr<MYSQL_RES>(mysql_store_result(&mysql_), mysql_free_result);
        }
    };
    virtual int32_t     ExecuteUpdate(const std::string &sql) {
        auto ret = mysql_query(&mysql_, sql.c_str());
        if (!ret) {
            return 0;
        }
        return mysql_affected_rows(&mysql_);
    };
    virtual bool        Next() {
        if (res.get() == nullptr) {
            return false;
        }
        row = mysql_fetch_row(res.get());
        if (row) {
            return true;
        } else {
            return false;
        }
    };
    virtual size_t      GetFetchSize() {
        return mysql_num_fields(res.get());
    };
    virtual size_t      GetRow() {
        return mysql_num_rows(res.get());
    };
    virtual bool        GetBoolean(uint32_t column_index) const {
        return std::stoi(row[column_index - 1]);
    };
    virtual long double GetDouble(uint32_t column_index) const {
        return std::stold(row[column_index - 1]);
    };
    virtual int32_t     GetInt(uint32_t column_index) const {
        return std::stoi(row[column_index - 1]);
    };
    virtual uint32_t    GetUInt(uint32_t column_index) const {
        return std::stoul(row[column_index - 1]);
    };
    virtual int64_t     GetInt64(uint32_t column_index) const {
        return std::stol(row[column_index - 1]);
    };
    virtual uint64_t    GetUInt64(uint32_t column_index) const {
        return std::stoull(row[column_index - 1]);
    };
    virtual std::string GetString(uint32_t column_index)  const {
        return std::string(row[column_index - 1]);
    };

};


