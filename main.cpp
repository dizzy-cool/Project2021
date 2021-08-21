#include "db.hpp"
#include "httplib.h"

using namespace httplib;
blog_system::TableBlog *table_blog;//全局变量实现
blog_system::TableTag *table_tag;
//具体对应的业务处理
void InsertBlog(const Request &req,Response &rsp){
	//从提交中取出正文,正文就是提交的博客信息,一Json格式的字符串形式组织的
	//req.body;
	//将Json格式的字符串进行反序列化,得到各个博客信息
	Json::Reader reader;
	Json::Value blog;
	Json::FastWriter writer;//其中没有Json的格式,以逗号分隔
	Json::Value errmsg;//error message
	bool ret = reader.parse(req.body,blog);//将请求正文进行Json反序列化-因为正文就是json格式的博客信息
	if (ret == false){
		printf("InsertBlog parse blog json failed!\n");
		rsp.status = 400;
		//设置错误信息的正文
		errmsg["ok"] = false;
		errmsg["reason"] = "parse blog json failed!";
		rsp.set_content(writer.write(errmsg),"application/json");//设置正文,正文的类型添加正文到rsp.body中
	//	rsp.body = writer.write(errmsg);
		return ;
	}
	//将得到的博客信息插入到数据库中
	ret = table_blog->Insert(blog);
	if(ret == false){
		printf("InsertBlog insert into database failed!\n");\
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	
}
void DeleteBlog(const Request &req,Response &rsp){
	/// /blog/123 /blog/(\d+) req.matches[0](存放的是) = /blog/123	 req.matches[1] = 123 
	int blog_id = std::stoi(req.matches[1]); //matches得到正则表达式匹配到的数字
	bool ret = table_blog->Delete(blog_id);
	if (ret == false){
		printf("DeleteBlog delete from database failed!\n");
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	
}
void UpdateBlog(const Request &req,Response &rsp){
	int blog_id = std::stoi(req.matches[1]);
	Json::Value blog;
	Json::Reader reader;
	bool ret = reader.parse(req.body,blog);
	if (ret == false){
		printf("UpdateBlog parse json failed!\n");
		rsp.status = 400;
		return ;
	}
	blog["id"] = blog_id;
	ret =table_blog->Update(blog);	
	
	if (ret == false){
		printf("DeleteBlog delete from database failed!\n");
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	
}
void GetAllBlog(const Request &req,Response &rsp){
	//获取到所有博客信息
	//从数据中取出博客数据列表
	Json::Value blogs;
	bool ret = table_blog->GetAll(&blogs);
	if (ret = false){
		printf("GetAllBlog select from database failed!\n");
		rsp.status = 500;
		return ;
	}
	//将数据进行json序列化,添加到rsp正文当中
	Json::FastWriter writer;
	rsp.set_content(writer.write(blogs),"application/json");
	rsp.status = 200;
	return ;	
}
void GetOneBlog(const Request &req,Response &rsp){
	int blog_id = std::stoi(req.matches[1]);
	Json::Value blog;
	blog["id"] = blog_id;
	bool ret = table_blog->GetOne(&blog);
	if (ret = false){
		printf("GetOneBlog select from database failed!\n");
		rsp.status = 500;
		return ;
	}
	//将数据进行json序列化,添加到rsp正文当中
	Json::FastWriter writer;
	rsp.set_content(writer.write(blog),"application/json");
	rsp.status = 200;
	return ;	
}
//Tag标签对应的接口
void InsertTag(const Request &req,Response &rsp){
	//从提交中取出正文,正文就是提交的博客信息,一Json格式的字符串形式组织的
	//req.body;
	//将Json格式的字符串进行反序列化,得到各个博客信息
	Json::Reader reader;
	Json::Value tag;
	Json::FastWriter writer;//其中没有Json的格式,以逗号分隔
	Json::Value errmsg;//error message
	bool ret = reader.parse(req.body,tag);//将请求正文进行Json反序列化-因为正文就是json格式的博客信息
	if (ret == false){
		printf("InsertTag parse tag json failed!\n");
		rsp.status = 400;
		//设置错误信息的正文
		errmsg["ok"] = false;
		errmsg["reason"] = "parse tag json failed!";
		rsp.set_content(writer.write(errmsg),"application/json");//设置正文,正文的类型添加正文到rsp.body中
	//	rsp.body = writer.write(errmsg);
		return ;
	}
	//将得到的博客信息插入到数据库中
	ret = table_tag->Insert(tag);
	if(ret == false){
		printf("InsertTag insert into database failed!\n");\
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	
}
void DeleteTag(const Request &req,Response &rsp){
	int tag_id = std::stoi(req.matches[1]);
	bool ret = table_tag->Delete(tag_id);
	if (ret == false){
		printf("delete from database failed!\n");
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	

}
void UpdateTag(const Request &req,Response &rsp){
	int tag_id = std::stoi(req.matches[1]);
	Json::Value tag;
	Json::Reader reader;
	bool ret = reader.parse(req.body,tag);
	if (ret == false){
		printf("UpdateTag parse json failed!\n");
		rsp.status = 400;
		return ;
	}
	tag["id"] = tag_id;
	ret =table_tag->Update(tag);	
	
	if (ret == false){
		printf("DeleteTag delete from database failed!\n");
		rsp.status = 500;
		return ;
	}
	rsp.status = 200;
	return ;	
}
void GetAllTag(const Request &req,Response &rsp){
	Json::Value tags;
	bool ret = table_tag->GetAll(&tags);
	if (ret = false){
		printf("GetAllTag select from database failed!\n");
		rsp.status = 500;
		return ;
	}
	//将数据进行json序列化,添加到rsp正文当中
	Json::FastWriter writer;
	rsp.set_content(writer.write(tags),"application/json");
	rsp.status = 200;
	return ;	
}
void GetOneTag(const Request &req,Response &rsp){
	int tag_id = std::stoi(req.matches[1]);
	Json::Value tag;
	tag["id"] = tag_id;
	bool ret = table_tag->GetOne(&tag);
	if (ret = false){
		printf("GetOneTag select from database failed!\n");
		rsp.status = 500;
		return ;
	}
	//将数据进行json序列化,添加到rsp正文当中
	Json::FastWriter writer;
	rsp.set_content(writer.write(tag),"application/json");
	rsp.status = 200;
	return ;	
}


//主函数
int main (){

	MYSQL *mysql = blog_system::MysqlInit();//初始化句柄
	table_blog = new blog_system::TableBlog(mysql);	
	table_tag = new blog_system::TableTag(mysql);
	//业务处理模块
	Server server;
	// /index.html --> ./www/index.html  设置url相对根目录的目的,限制客户端访问该根目录底下的文件
	//设置相对更目录的目的:当客户端请求静态文件资源时候,httplib会直接根据路径读取文件数据进行响应
	std::string str1 = "./www";

	//std::cout << ".............................................." << std::endl;
//	server.set_base_dir("./www");
	server.set_mount_point("/","./www");
//	std::cout << "::::::::::::::::::::::::::::::::::::::::::" << std::endl;
//	server.set_base_dir("./www");//设置url中资源相对根目录,并且在请求时候,自动添加index.html
	//博客信息的增删改查
	server.Post("/blog",InsertBlog);
	//正则表达式: \d-匹配数字字符 +表示匹配前面的字符一次或多次  ()为了临时保存匹配的数据
	server.Delete(R"(/blog/(\d+))",DeleteBlog);//R"()"取出括号中的所有字符的特殊含义
	//标签信息的增删改查
	server.Put(R"(/blog/(\d+))", UpdateBlog);
	server.Get("/blog",GetAllBlog);
	server.Get(R"(/blog/(\d+))",GetOneBlog);


	server.Post("/tag",InsertTag);
	server.Delete(R"(/tag/(\d+))",DeleteTag);//R"()"取出括号中的所有字符的特殊含义
	server.Put(R"(/tag/(\d+))", UpdateTag);
	server.Get("/tag",GetAllTag);
	server.Get(R"(/tag/(\d+))",GetOneTag);
	//网络通信-- 搭建http服务器
	server.listen("0.0.0.0",80);

	return 0;
}
