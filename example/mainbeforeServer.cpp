#include "db.hpp"

int main (){
	MYSQL *mysql = blog_system::MysqlInit();
	blog_system::TableBlog table_blog(mysql);
	
	Json::Value blog;


	/*
//	blog["id"] = 3;
	blog["tag_id"] = 6;
	blog["title"] = "This id a C# 的 blog";
	blog["content"] = "C# 是 牛人is a elite environment";
	table_blog.Insert(blog);
//	table_blog.Update(blog);
	*/
//	table_blog.GetAll(&blog);

	blog["id"] = 3;
	table_blog.GetOne(&blog);
	Json::StyledWriter writer;
	std::cout << writer.write(blog) <<std::endl;
	/*
	table_blog.GetAll(&blog);
	Json::StyledWriter writer;
	std::cout << writer.write(blog) << std::endl;
	*/


	blog_system::TableTag table_tag(mysql);
//	Json::Value tag;
	//tag["name"] = "C#是真的";
	//tag["id"] = 1;
	//table_tag.Insert(tag);
	//table_tag.Delete(5);
	
	/*
	table_tag.GetOne(&tag);
	Json::StyledWriter writer;
	std::cout << writer.write(tag) <<std::endl;
	*/
	
	return 0;


}
