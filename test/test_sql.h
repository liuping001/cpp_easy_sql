#pragma once
#include <string>
#include <sstream>
#include <vector>
#include "sql_handler.h"
using namespace std;

namespace vg_account {

struct GetUserInfoParam {
	std::string name;
	std::string passwd;
	int32_t state_id;
};

struct GetUserInfoByNameParam {
	std::vector<std::string> name;
};

struct UpdateUserInfoByNameParam {
	std::string passwd;
	int32_t state_id;
	std::string name;
};

struct InsertUserParam {
	std::string name;
	std::string passwd;
	int32_t state_id;
};

struct DeleteUserParam {
	uint64_t user_id;
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

struct GetUserInfoByNameResult {
	uint64_t user_id;
	std::string name;
	std::string passwd;
	int32_t state_id;
	string DebugString() const {
		std::ostringstream ss;
		ss << "GetUserInfoByNameResult {";
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

static std::string GetUserInfoByNameSql (CommonSqlHandler *sql_handler, const GetUserInfoByNameParam &param) {
	ostringstream ss;
	if (param.name.size() > 0) {
		ss << "  select `user_id`,`name`,`passwd`,`state_id`";
		ss << "  from `user_info`";
		ss << "  where `name` in";
		ss << " (";
		for (size_t i = 0; i < param.name.size(); i++) {
			if (i != 0) {
				ss << " ,";
			}
			ss << " '" + sql_handler->EscapeString(param.name[i]) + "'";
		}
		ss << " )";
	}
	return ss.str();
}

static std::string UpdateUserInfoByNameSql (CommonSqlHandler *sql_handler, const UpdateUserInfoByNameParam &param) {
	ostringstream ss;
	ss << "  update `user_info`";
	ss << "  set `user_id` = `user_id`";
	if (!param.passwd.empty()) {
		ss << "  ,`passwd` = '" + sql_handler->EscapeString(param.passwd) + "'";
	}
	if (param.state_id > 0) {
		ss << "  ,`state_id` = '" + to_string(param.state_id) + "'";
	}
	ss << "  where `name` = '" + sql_handler->EscapeString(param.name) + "'";
	return ss.str();
}

static std::string AddUserSql (CommonSqlHandler *sql_handler, const InsertUserParam &param) {
	ostringstream ss;
	ss << "  insert into `user_info` (`name`,`passwd`,`state_id`)";
	ss << "  values('" + sql_handler->EscapeString(param.name) + "', '" + sql_handler->EscapeString(param.passwd) + "', '" + to_string(param.state_id) + "')";
	return ss.str();
}

static std::string DeleteUserSql (CommonSqlHandler *sql_handler, const DeleteUserParam &param) {
	ostringstream ss;
	ss << "  delete from `user_info`";
	ss << "  where `user_id` = '" + to_string(param.user_id) + "'";
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

static std::vector<GetUserInfoByNameResult> GetUserInfoByName (ColumnLabelSqlHandler *sql_handler, const GetUserInfoByNameParam &param) {
	std::vector<GetUserInfoByNameResult> ret;
	auto sql = GetUserInfoByNameSql(sql_handler, param);
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

static std::vector<GetUserInfoByNameResult> GetUserInfoByName (ColumnIndexSqlHandler *sql_handler, const GetUserInfoByNameParam &param) {
	std::vector<GetUserInfoByNameResult> ret;
	auto sql = GetUserInfoByNameSql(sql_handler, param);
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

static int32_t UpdateUserInfoByName (ColumnLabelSqlHandler *sql_handler, const UpdateUserInfoByNameParam &param) {
	auto sql = UpdateUserInfoByNameSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}

static int32_t UpdateUserInfoByName (ColumnIndexSqlHandler *sql_handler, const UpdateUserInfoByNameParam &param) {
	auto sql = UpdateUserInfoByNameSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}

static int32_t AddUser (ColumnLabelSqlHandler *sql_handler, const InsertUserParam &param) {
	auto sql = AddUserSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}

static int32_t AddUser (ColumnIndexSqlHandler *sql_handler, const InsertUserParam &param) {
	auto sql = AddUserSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}

static int32_t DeleteUser (ColumnLabelSqlHandler *sql_handler, const DeleteUserParam &param) {
	auto sql = DeleteUserSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}

static int32_t DeleteUser (ColumnIndexSqlHandler *sql_handler, const DeleteUserParam &param) {
	auto sql = DeleteUserSql(sql_handler, param);
	return sql_handler->ExecuteUpdate(sql);
}
}
