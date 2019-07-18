# cpp_easy_sql
c++ operation database like mybatis

* cpp_xml.py 将xml定义的sql语句转成c++代码
* sql_handler.h 定义了两组执行sql、获取结果的接口。分别是ColumnIndexSqlHandler通过index获取结果、
ColumnLabelSqlHandler通过字段名获取结果。
* cpp_sql.h 实现了mysql-connection-c++的
## 使用
以查询下面这个user_info表为例：
```sql
CREATE TABLE `user_info` (
  `user_id` int(11) AUTO_INCREMENT PRIMARY KEY,
  `name` varchar(50) NOT NULL,
  `passwd` varchar(50) NOT NULL,
  `state_id` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
```
1. 定义xml
```xml
<?xml version="1.0" encoding="UTF-8"?>
<mapper namespace="get_user">
    <!-- 查询 -->
    <GetUserInfoParam>
        <field name="name" type="string" ></field>
        <field name="passwd" type="string" ></field>
        <field name="state_id" type="int32" ></field>
    </GetUserInfoParam>
    <GetUserInfoEntity>
        <field name="user_id" type="uint64"></field>
        <field name="name" type="string"></field>
        <field name="passwd" type="string" ></field>
        <field name="state_id" type="int32" ></field>
    </GetUserInfoEntity>

    <select id="GetUserInfo" param_type="GetUserInfoParam" result_type="GetUserInfoEntity">
        select `user_id`,`name`,`passwd`,`state_id`
        from `user_info`
        where 1 = 1
        <if test="!#{name}.empty()">
            and `name` = #{name}
        </if>
        <if test="!#{passwd}.empty()">
            and `passwd` = #{passwd}
        </if>
        <if test="#{state_id} > 0">
            and `state_id` = #{state_id}
        </if>
    </select>
</mapper>

```
将其保存为```get_user.xml```
2. 转cpp代码
```shell
python cpp_xml.py get_user.xml #生成相应cpp代码
```
* 作为查询条件出入的参数类
```cpp
struct GetUserInfoParam {
	std::string name;
	std::string passwd;
	int32_t state_id;
};
```
* 获取一行结果的类
```cpp
struct GetUserInfoEntity {
	uint64_t user_id;
	std::string name;
	std::string passwd;
	int32_t state_id;
};
```

* 根据查询条件类拼sql的函数
```cpp
std::string GetUserInfoSql (const GetUserInfoParam &param) {
	ostringstream ss;
	ss << "  select `user_id`,`name`,`passwd`,`state_id`";
	ss << "  from `user_info`";
	ss << "  where 1 = 1";
	if (!param.name.empty()) {
		ss << "  and `name` = '" + to_string(param.name) + "'";
	}
	if (!param.passwd.empty()) {
		ss << "  and `passwd` = '" + to_string(param.passwd) + "'";
	}
	if (param.state_id > 0) {
		ss << "  and `state_id` = '" + to_string(param.state_id) + "'";
	}
	return ss.str();
}
```

* 执行sql并获取结果集
```cpp
std::vector<GetUserInfoEntity> 
GetUserInfoResultByName (ColumnLabelSqlHandler *sql_handler, const GetUserInfoParam &param) {
	std::vector<GetUserInfoEntity> ret;
	auto sql = GetUserInfoSql(param);
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
```

