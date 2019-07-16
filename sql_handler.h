#pragma once
#include <string>

// 需要实现的接口
struct SqlHandler {
    virtual void ExecuteSql(const std::string &sql) = 0;
    virtual bool Next() = 0;

    virtual size_t GetFetchSize() = 0;
    virtual size_t GetRow() = 0;

    virtual bool GetBoolean(uint32_t column_index) const = 0;
    virtual bool GetBoolean(const std::string& column_label) const = 0;

    virtual long double GetDouble(uint32_t column_index) const = 0;
    virtual long double GetDouble(const std::string& column_label) const = 0;

    virtual int32_t GetInt(uint32_t column_index) const = 0;
    virtual int32_t GetInt(const std::string& column_label) const = 0;

    virtual uint32_t GetUInt(uint32_t column_index) const = 0;
    virtual uint32_t GetUInt(const std::string& column_label) const = 0;

    virtual int64_t GetInt64(uint32_t column_index) const = 0;
    virtual int64_t GetInt64(const std::string& column_label) const = 0;

    virtual uint64_t GetUInt64(uint32_t column_index) const = 0;
    virtual uint64_t GetUInt64(const std::string& column_label) const = 0;

    virtual std::string GetString(uint32_t column_index)  const = 0;
    virtual std::string GetString(const std::string& column_label) const = 0;
};
