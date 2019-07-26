cmake_minimum_required(VERSION 3.13)
project(cpp_easy_sql)

set(CMAKE_CXX_STANDARD 11)

include_directories(..)

add_executable(cpp_sql_test cpp_sql_test.cpp)
target_link_libraries(cpp_sql_test mysqlcppconn)

add_executable(c_sql_test c_sql_test.cpp)
target_link_libraries(c_sql_test mysqlclient)