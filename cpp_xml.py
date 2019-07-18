# -*- coding: UTF-8 -*-
#!/usr/local/bin/python

from xml.dom import *
import xml.dom.minidom
import re
import sys
import os

type_convert = {"int32":"int32_t",          "array[int32]":"std::vector<int32_t>",
                "uint32":"uint32_t",        "array[uint32]":"std::vector<uint32_t>",
                "string": "std::string",    "array[string]":"std::vector<std::string>",
                "uint64":"uint64_t",        "array[uint64]":"std::vector<uint64_t>",
                "int64":"int64_t",          "array[int64]":"std::vector<int64_t>",
                "double":"double",          "array[double]":"std::vector<double>",
                "bool":"bool",
                }

type_convert_sql_handler = {"int32":"Int", "uint32":"UInt",
                            "int64":"Int64", "uint64":"UInt64",
                            "string":"String", "double":"Double",
                            "bool":"Boolean"}

def make_struct(tag, debug_str = False):
    str = "struct " + tag.nodeName + " {\n"
    for item in tag.getElementsByTagName("field"):
        str += "\t" + type_convert[item.getAttribute("type")] + " " + item.getAttribute("name") + ";\n"
    if debug_str == True:
        str += "\tstring DebugString() const {\n"
        str += "\t\tstd::ostringstream ss;\n"
        str += "\t\tss << \"%s {\";\n" %(tag.nodeName)
        for item in tag.getElementsByTagName("field"):
            str += "\t\tss << \" %s:\"<< %s;\n"%(item.getAttribute("name"), item.getAttribute("name"))
        str += "\t\tss << \"}\";\n"
        str += "\t\treturn ss.str();\n\t}\n"
    str += "};\n\n"

    return str

def print_ss(str, head = "\t"):
    return head + "ss << \" " + str + "\";\n"

def print_one_value(str):
    return "\'\" + to_string(" + str + ") + \"\'"

def print_value(str):
    return "\'\" + to_string(" + str + ") + \"\'"

class PreFix:
    def prefix_str(self, matched):
        value = matched.group('value')
        return print_value(self.prefix + value[2:len(value)-1])

    def replace_field(self, data, prefix):
        self.prefix = prefix
        return (re.sub('(?P<value>#{[0-9a-zA-Z_]{1,}})', self.prefix_str, data))
def re_prefix(data, prefix):
    return PreFix().replace_field(data, prefix)

class PreFixCppField:
    def prefix_str(self, matched):
        value = matched.group('value')
        return self.prefix + value[2:len(value)-1]

    def replace_field(self, data, prefix):
        self.prefix = prefix
        return (re.sub('(?P<value>#{[0-9a-zA-Z_]{1,}})', self.prefix_str, data))
def re_prefix_cpp_field(data, prefix):
    return PreFixCppField().replace_field(data, prefix)

def deal_text_to_sql(data, field_prefix, head = ""):
    str = ""
    t = data.split("\n") # 每一行处理成一行
    for item in t:
        item = item.replace('\n', ' ')
        item = re.sub(' +', ' ', item)
        item = re_prefix(item, field_prefix)
        if (item != "" and item.isspace() == False):
            str += print_ss(item, head)
    return str

# 解析具体标签 if、foreach、text
def parse_condation(tag, level = 1):
    head=""
    for count in range(0, level):
        head += "\t"
    str = ""
    if tag.nodeName == "if":
        str += head + "if ("
        str += re_prefix_cpp_field(tag.getAttribute("test"), "param.")
        str += ") {\n"
        for if_tag in tag.childNodes:
            str += parse_condation(if_tag, level + 1)
        str += head + "}\n"
    elif tag.nodeName == "foreach":
        collection = tag.getAttribute("collection")
        collection = re_prefix_cpp_field(collection, "param.")
        open = tag.getAttribute("open")
        close = tag.getAttribute("close")
        separator = tag.getAttribute("separator")

        str += print_ss(open, head)
        str += head + "for (size_t i = 0; i < " + collection + ".size(); i++) {\n"
        str += head + "\tif (i != 0) {" + print_ss(separator, "\n\t\t" + head) + head + "\t}\n"
        str += print_ss(print_one_value(collection + "[i]"), head + "\t")
        str += head + "}\n"
        str += print_ss(close, head)

    elif tag.nodeType == 3:
        if (tag.data != "" and tag.data.isspace() == False):
            str += deal_text_to_sql(tag.data, "param.", head)
    return str

def cpp_header_include(include, open="<", close=">"):
    return "#include %s%s%s\n"%(open, include, close)
def cpp_using_namespace(namespace):
    return "using namespace " + namespace + ";\n"

class ParseXml:
    def __init__(self, xml_file):
        self.need_deal_action = ("select", "update", "insert", "delete")
        self.param_class_sql = ""
        self.result_class_sql = ""
        self.make_sql_func = ""
        self.make_result_func = ""
        self.include_header_string = ""
        self.dom = xml.dom.minidom.parse(xml_file)
        self.root = self.dom.documentElement
        self.get_namespace()
        self.cpp_header()
        self.parse_xml()
    def draw(self, out_file):
        with open(out_file, 'w+') as f:
            f.write(self.include_header_string)
            f.write("\nnamespace " + self.namespace +" {\n\n")
            f.write(self.param_class_sql)
            f.write("\n")
            f.write(self.result_class_sql)
            f.write("\n")
            f.write(self.make_sql_func)
            f.write("\n")
            f.write(self.make_result_func)
            f.write("}\n")

    def get_namespace(self):
        tag = self.root.getAttribute("namespace")
        self.namespace = tag

    def cpp_header(self):
        self.include_header_string = "#pragma once\n"
        self.include_header_string += cpp_header_include("string")
        self.include_header_string += cpp_header_include("sstream")
        self.include_header_string += cpp_header_include("vector")
        self.include_header_string += cpp_header_include("sql_handler.h", "\"", "\"")
        self.include_header_string += cpp_using_namespace("std")
        # string to_string
        self.include_header_string += "inline std::string to_string(const std::string &s) {  return s; }\n"

    def get_sql_func_name(self, name):
        return name + "Sql"

    def make_get_sql_func(self, item, param_name):
        sql_func_content = "static std::string " + self.get_sql_func_name(item.getAttribute("id"))
        if (param_name):
            sql_func_content += " (const " + param_name + " &param)"
        else:
            sql_func_content +="()"
        sql_func_content += " {\n"

        sql_func_content += "\t"
        sql_func_content += "ostringstream ss;\n"
        for item_sql in item.childNodes:
            sql_func_content += parse_condation(item_sql)
        sql_func_content += "\treturn ss.str();\n"
        sql_func_content += "}\n\n"
        self.make_sql_func += sql_func_content

    '''
    the_way: by label, by index
    '''
    def make_get_result_func(self, item, result_name, param_name, get_ret_way):
        self.make_result_func += "\n"
        # 返回值
        return_type = ""
        if result_name == "":
            return_type = "int32_t"
        else:
            return_type = "std::vector<%s>"%(result_name)
        self.make_result_func +="static " + return_type

        # 函数名
        sql_func_name = item.getAttribute("id")
        self.make_result_func += " " + sql_func_name

        sql_handler_type = ""
        if get_ret_way == "by_label":
            sql_handler_type = "ColumnLabelSqlHandler"
        elif get_ret_way == "by_index":
            sql_handler_type = "ColumnIndexSqlHandler"

        # 参数值
        if param_name == "":
            self.make_result_func += " (%s *sql_handler) {\n"%(sql_handler_type)
        else:
            self.make_result_func += " (%s *sql_handler, const %s &param) {\n"%(sql_handler_type, param_name)

        head = "\t"
        #定义返回值
        if result_name != "":
            self.make_result_func += head + return_type + " ret;\n"
        #获取sql
        self.make_result_func += head + "auto sql = " + self.get_sql_func_name(item.getAttribute("id")) + "(param);\n"
        #执行sql
        if result_name == "":
            pass
        else:
            self.make_result_func += head + "sql_handler->ExecuteQuery(sql);\n"
        #不是sql这里就已经返回了
        if result_name == "":
            self.make_result_func += head + "return sql_handler->ExecuteUpdate(sql);\n}\n"
            return

        self.make_result_func += head + "while(sql_handler->Next()) {\n"
        self.make_result_func += head + "\tret.emplace_back();\n"
        tag = self.root.getElementsByTagName(result_name)[0]
        fields = tag.getElementsByTagName("field")
        for i in range(0, len(fields)):
            if get_ret_way == "by_label":
                self.make_result_func += "\t\tret.back().%s = sql_handler->Get%s(\"%s\");\n"%(
                    fields[i].getAttribute("name"), type_convert_sql_handler[fields[i].getAttribute("type")],
                    fields[i].getAttribute("name"))
            elif get_ret_way == "by_index":
                self.make_result_func += "\t\tret.back().%s = sql_handler->Get%s(%d);\n"%(
                    fields[i].getAttribute("name"), type_convert_sql_handler[fields[i].getAttribute("type")], i + 1)
                pass
        self.make_result_func += head +"}\n"
        self.make_result_func += head + "return ret;\n}\n"

    def parse_xml(self):
        for name in self.need_deal_action:
            action = self.root.getElementsByTagName(name)
            for item in action:
                # 生成方法的请求参数
                param_name = item.getAttribute("param_type")
                param_type = self.root.getElementsByTagName(param_name)
                if (len(param_type) == 1):
                    self.param_class_sql += make_struct(param_type[0])
                elif (len(param_type) == 0):
                    param_name = ""

                # 生成方法的返回结果
                result_name = item.getAttribute("result_type")
                result_type = self.root.getElementsByTagName(result_name)
                if (len(result_type) == 1):
                    self.result_class_sql += make_struct(result_type[0], True)
                elif (len(result_type) == 0):
                    result_name = ""

                # 生成产生sql的函数
                self.make_get_sql_func(item, param_name)

                # 生成执行函数（输入结构体，返回结果结构体）
                self.make_get_result_func(item, result_name, param_name, "by_label")
                self.make_get_result_func(item, result_name, param_name, "by_index")
def main():
    for arg in sys.argv[1:]:
        print "process begin : " + arg
        parse = ParseXml(arg)
        parse.draw(os.path.splitext(arg)[0] + "_sql.h")
        print "finish : " + arg

if __name__ == "__main__":
    sys.exit(main())
