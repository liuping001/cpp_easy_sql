#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "sql_handler.h"
using namespace std;

namespace get_user {

struct GetUserInfoParam {
	std::string name;
	std::string passwd;
	int32_t state_id;
};


struct GetUserInfoEntity {
	uint64_t user_id;
	std::string name;
	std::string passwd;
	int32_t state_id;
	string DebugString() const {
		std::ostringstream ss;
		ss << "GetUserInfoEntity {";
		ss << " user_id:"<< user_id;
		ss << " name:"<< name;
		ss << " passwd:"<< passwd;
		ss << " state_id:"<< state_id;
		ss << "}";
		return ss.str();
	}
};


static std::string GetUserInfoSql (CommonSqlHandler *sql_handler, const GetUserInfoParam &param) {
	ostringstream ss;
	ss << "  select `user_id`,`name`,`passwd`,`state_id`";
	ss << "  from `user_info`";
	ss << "  where 1 = 1";
	if (!param.name.empty()) {
		ss << "  and `name` = '" + sql_handler->EscapeString(param.name) + "'";
	}
	if (!param.passwd.empty()) {
		ss << "  and `passwd` = '" + sql_handler->EscapeString(param.passwd) + "'";
	}
	if (param.state_id > 0) {
		ss << "  and `state_id` = '" + to_string(param.state_id) + "'";
	}
	return ss.str();
}



static std::vector<GetUserInfoEntity> GetUserInfo (ColumnLabelSqlHandler *sql_handler, const GetUserInfoParam &param) {
	std::vector<GetUserInfoEntity> ret;
	auto sql = GetUserInfoSql(sql_handler, param);
	sql_handler->ExecuteQuery(sql);
	while(sql_handler->Next()) {
		ret.emplace_back();
		ret.back().user_id = sql_handler->GetUInt64("user_id");
		ret.back().name = sql_handler->GetString("name");
		ret.back().passwd = sql_handler->GetString("passwd");
		ret.back().state_id = sql_handler->GetInt("state_id");
	}
	return ret;
}

static std::vector<GetUserInfoEntity> GetUserInfo (ColumnIndexSqlHandler *sql_handler, const GetUserInfoParam &param) {
	std::vector<GetUserInfoEntity> ret;
	auto sql = GetUserInfoSql(sql_handler, param);
	sql_handler->ExecuteQuery(sql);
	while(sql_handler->Next()) {
		ret.emplace_back();
		ret.back().user_id = sql_handler->GetUInt64(1);
		ret.back().name = sql_handler->GetString(2);
		ret.back().passwd = sql_handler->GetString(3);
		ret.back().state_id = sql_handler->GetInt(4);
	}
	return ret;
}
}
