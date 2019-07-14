# -*- coding: UTF-8 -*-
#!/usr/local/bin/python

from xml.dom import *
import xml.dom.minidom
import re

type_convert = {"int32":"int32_t",  "array[int32]":"std::vector<int32_t>",
                "uint32":"uint32_t", "array[uint32]":"std::vector<uint32_t>",
                "string": "std::string", "array[string]":"std::vector<std::string>",
                "uint64":"uint64_t",  "array[uint64]":"std::vector<uint64_t>",
                "int64":"int64_t",  "array[int64]":"std::vector<int64_t>",
                }

def makeStruct(tag):
    str = "struct " + tag.nodeName + " {\n"
    for item in tag.getElementsByTagName("field"):
        str += "\t" + type_convert[item.getAttribute("type")] + " " + item.getAttribute("name") + ";\n"
    str += "};\n"
    return str

def printSS(str, head = "\t"):
    return head + "ss << \" " + str + "\";\n"

def printOneValue(str):
    return "\'\" + to_string(" + str + ") + \"\'"

def printValue(str):
    return "\'\" + to_string(" + str + ") + \"\'"

class PreFix:
    def prefixStr(self, matched):
        value = matched.group('value')
        return printValue(self.prefix + value[2:len(value)-1])

    def replaceField(self, data, prefix):
        self.prefix = prefix
        return (re.sub('(?P<value>#{[0-9a-zA-Z_]{1,}})', self.prefixStr, data))
def RePrefix(data, prefix):
    return PreFix().replaceField(data, prefix)

class PreFixCppField:
    def prefixStr(self, matched):
        value = matched.group('value')
        return self.prefix + value[2:len(value)-1]

    def replaceField(self, data, prefix):
        self.prefix = prefix
        return (re.sub('(?P<value>#{[0-9a-zA-Z_]{1,}})', self.prefixStr, data))
def RePreFixCppField(data, prefix):
    return PreFixCppField().replaceField(data, prefix)

def dealTextToSql(data, field_prefix, head = ""):
    str = ""
    t = data.split("\n") # 每一行处理成一行
    for item in t:
        item = item.replace('\n', ' ')
        item = re.sub(' +', ' ', item)
        item = RePrefix(item, field_prefix)
        if (item != "" and item.isspace() == False):
            str += printSS(item, head)
    return str

def parseCondation(tag, level = 1):
    head=""
    for count in range(0, level):
        head += "\t"
    str = ""
    if tag.nodeName == "if":
        str += head + "if ("
        str += RePreFixCppField(tag.getAttribute("test"), "param.")
        str += ") {\n"
        for if_tag in tag.childNodes:
            str += parseCondation(if_tag, level + 1)
        str += head + "}\n"
    elif tag.nodeName == "foreach":
        collection = tag.getAttribute("collection")
        collection = RePreFixCppField(collection, "param.")
        open = tag.getAttribute("open")
        close = tag.getAttribute("close")
        separator = tag.getAttribute("separator")

        str += printSS(open, head)
        str += head + "for (size_t i = 0; i < " + collection + ".size(); i++) {\n"
        str += head + "\tif (i != 0) {" + printSS(separator, "\n\t\t" + head) + head + "\t}\n"
        str += printSS(printOneValue(collection + "[i]"), head + "\t")
        str += head + "}\n"
        str += printSS(close, head)

    elif tag.nodeType == 3:
        if (tag.data != "" and tag.data.isspace() == False):
            str += dealTextToSql(tag.data, "param.", head)
    return str

need_deal_action = ("select", "update", "insert", "delete")

dom = xml.dom.minidom.parse(r'./test.xml')
root = dom.documentElement

# string to_string
print "#include<string>\n"
print "#include<sstream>\n"
print "#include<vector>\n"
print "using namespace std;\n\n"

print "std::string to_string(const std::string &s) {  return s; }\n"

for name in need_deal_action:
    action = root.getElementsByTagName(name)
    for item in action:
        # 生成方法的请求参数
        param_name = item.getAttribute("param_type")
        param_type = root.getElementsByTagName(param_name)
        if (len(param_type) == 1):
            print makeStruct(param_type[0])
        elif (len(param_type) == 0):
            param_name = ""

        # 生成方法的返回结果
        result_name = item.getAttribute("result_type")
        result_type = root.getElementsByTagName(result_name)
        if (len(result_type) == 1):
            print makeStruct(result_type[0])
        elif (len(result_type) == 0):
            result_name = ""

        # 生成产生sql的函数
        sql_func_name = item.getAttribute("id")
        sql_func_content = "std::string " + sql_func_name + "Sql"
        if (param_name):
            sql_func_content += " (const " + param_name + " &param)"
        else:
            sql_func_content +="()"
        sql_func_content += " {\n"

        sql_func_content += "\t"
        sql_func_content += "ostringstream ss;\n"
        for item_sql in item.childNodes:
            sql_func_content += parseCondation(item_sql)
        sql_func_content += "\treturn ss.str();\n"
        sql_func_content += "}\n"
        print sql_func_content

