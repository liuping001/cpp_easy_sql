//
// Created by winter on 2019/7/17.
//

#include <memory>

#include "mysql_connection.h"
#include "cppconn/driver.h"
#include "cppconn/exception.h"
#include "cppconn/resultset.h"
#include "cppconn/statement.h"
#include "cppconn/prepared_statement.h"

#include "sql_handler.h"

struct CppSql : public ColumnLabelSqlHandler {
    void Init(std::unique_ptr<sql::Statement> _stmt) {
        stmt = std::move(_stmt);
    }
    virtual void ExecuteQuery(const std::string &sql) {
        res.reset(stmt->executeQuery(sql));
    };

    virtual int32_t ExecuteUpdate(const std::string &sql) {
        return stmt->executeUpdate(sql);
    }
    virtual bool Next() { return res->next(); }

    virtual size_t GetFetchSize() { return res->getFetchSize(); }
    virtual size_t GetRow() { return res->rowsCount(); }

    virtual bool GetBoolean(const std::string& column_label) const { return res->getBoolean(column_label); }

    virtual long double GetDouble(const std::string& column_label) const { return res->getDouble(column_label); }

    virtual int32_t GetInt(const std::string& column_label) const {return res->getInt(column_label);}

    virtual uint32_t GetUInt(const std::string& column_label) const {return res->getUInt(column_label);}

    virtual int64_t GetInt64(const std::string& column_label) const {return res->getInt64(column_label);}

    virtual uint64_t GetUInt64(const std::string& column_label) const {return res->getUInt64(column_label);}

    virtual std::string GetString(const std::string& column_label) const {return res->getString(column_label);}

    std::unique_ptr<sql::Statement> stmt;
    std::unique_ptr<sql::ResultSet> res;
};
