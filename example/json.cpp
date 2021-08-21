#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

int main(){


	Json::Value value;
	value["name"] = "张三";
	value["age"] = 18;
	value["score"] = 88.88;

	Json::StyledWriter writer;
	std::string str = writer.write(value);
	std::cout << str << std::endl;//序列化
	


	Json::Value value1;
	Json::Reader reader;
	
	reader.parse(str, value1);
	std::cout << value1["name"].asString() << std::endl;	
	std::cout << value1["age"].asInt() << std::endl;
	std::cout << value1["score"].asFloat() << std::endl;
	return 0;
