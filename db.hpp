#include <iostream>
#include <mysql/mysql.h>
#include <mutex>
#include <jsoncpp/json/json.h>


#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD "kb181818"
#define MYSQL_DB "db_blog"
namespace blog_system{
	static std::mutex g_mutex;//全局变量锁

		MYSQL *MysqlInit(){//
	//向外提供接口返回初始化完成的mysql句柄(链接服务器,选择数据库,设置字符集)
		MYSQL *mysql;
		//初始化
		mysql = mysql_init(NULL);//动态分配
		if (mysql == NULL){
			printf("init mysql error\n");
			return NULL;
		}
		//链接服务器
		if(mysql_real_connect(mysql,MYSQL_HOST,MYSQL_USER,MYSQL_PSWD,NULL,0,NULL, 0)==NULL){ //设置字符集
			printf("connect mysql server error:%s\n",mysql_error(mysql));
			mysql_close(mysql);//退出时关闭mysql,释放资源
			return NULL;
		} 
		//设置字符集
		if(mysql_set_character_set(mysql,"utf8") != 0){
			printf("set client character error:%s\n",mysql_error(mysql));
			mysql_close(mysql);//退出时关闭mysql,释放资源
			return NULL;
		}
	   	//选择数据库	
	   	if(mysql_select_db(mysql,MYSQL_DB) != 0){
			printf("select db error:%s\n",mysql_error(mysql));
			mysql_close(mysql);//退出时关闭mysql,释放资源
			return NULL;	
		}
		return mysql;
	}	
	void MysqkRelease(MYSQL *mysql){//销毁句柄
		if(mysql){
			mysql_close(mysql);
		}
		return ;
	}
 	bool MysqlQuery(MYSQL *mysql,const char* sql){//执行语句的共有接口 
		int ret = mysql_query(mysql,sql);		
		if (ret != 0){
			printf("query sql:[%s] failed:%s\n",sql,mysql_error(mysql));
			return false;
		}
		return true;
	}


	class TableBlog {
 		public: TableBlog(MYSQL *mysql):_mysql(mysql){} //从blog中取出博客信息,组织sql语句,将数据插入数据
		bool Insert(Json::Value &blog){ // id 	tag_id	 	title	 content	 ctime
			#define INSERT_BLOG "insert tb_blog values(null,'%d','%s','%s',now());"

			//因为博客的正文长度不定,有可能会很长,因此如果直接定义tmp空间的长度固定,有可能会导致越界访问
			//动态空间分布
			int len = blog["content"].asString().size() +4096;
			//std::string sql;
			//sql.resize(len);//设置长度
			char *tmp = (char*)malloc(len);	
			//	char tmp[4096] = {0};//4096可能放不下comment正文字符串

			//组织字符串:使用宏定义串联字符串
			sprintf(tmp,INSERT_BLOG,blog["tag_id"].asInt(),
					blog["title"].asCString(),
					blog["content"].asCString());
			bool ret = MysqlQuery(_mysql,tmp);
			free(tmp);
			return ret;//返回执行语句
		}

		bool Delete(int blog_id){//根据博客id删除博客
		#define DELETE_BLOG "delete from tb_blog where id=%d"
			char tmp[1024] = {0};
			sprintf(tmp, DELETE_BLOG,blog_id);
			bool ret = MysqlQuery(_mysql,tmp);
			return ret;
		}
		bool Update(Json::Value &blog){//从blog中取出博客信息,组织sql语句,更新数据库中的数据
		//id tag_id title content ctime {tag_id:1, title:linux博客, content=博客挺好看}

		#define UPDATE_BLOG "update tb_blog set tag_id=%d,title='%s',content='%s'where id=%d;"	
			int len = blog["content"].asString().size() + 4096;
			char *tmp = (char*)malloc(len);
			sprintf(tmp,UPDATE_BLOG,blog["tag_id"].asInt(),
					blog["title"].asCString(),
					blog["content"].asCString(),
					blog["id"].asInt());
			bool ret = MysqlQuery(_mysql,tmp);
			free(tmp);
			return ret;
		}
		//blogs表示参数返回所有博客
		bool GetAll(Json::Value* blogs){//通过blog返回所有的博客信息(因为通常是列表显示,因此不返回正文)
		#define GETALL_BLOG "select id, tag_id, title, ctime from tb_blog;"
		//执行查询语句
			g_mutex.lock();//执行加锁
			bool ret = MysqlQuery(_mysql,GETALL_BLOG);
			if (ret == false){
				 g_mutex.unlock();//退出时执行解锁操作
				return false;
			}

			//保存结果集
			MYSQL_RES *res = mysql_store_result(_mysql);
			g_mutex.unlock();
			if(res == NULL){//失败了
				printf("store all blog result failed:%s\n",mysql_error(_mysql));
				return false;
			}
			//遍历结果集
			int row_num = mysql_num_rows(res);
			for (int i = 0; i < row_num; i++){
				MYSQL_ROW row = mysql_fetch_row(res);
				Json::Value blog;
				blog["id"] = std::stoi(row[0]);
				blog["tag_id"] = std::stoi(row[1]);
				blog["title"] = row[2];
				blog["ctime"] = row[3];
				blogs->append(blog);//添加json数组元素,每一个博客都属于所有博客数组中的一个元素
			}
			mysql_free_result(res);
			return true;
		}

		bool GetOne(Json::Value *blog){//返回单个博客信息,包含正文
		#define GETONE_BLOG "select tag_id, title, content, ctime from tb_blog where id=%d;"
			g_mutex.lock();
			char tmp[1024] = {0};
			sprintf(tmp,GETONE_BLOG,(*blog)["id"].asInt());
			bool ret = MysqlQuery(_mysql,tmp);
			if(ret == false){
				g_mutex.unlock();//退出之前解锁
				return false;
			}
			MYSQL_RES *res = mysql_store_result(_mysql);
			g_mutex.unlock();//退出之前解锁
			if(res == NULL){//失败了
				printf("store all blog result failed:%s\n",mysql_error(_mysql));
				return false;
			}
			int row_num = mysql_num_rows(res);
			if(row_num != 1){
				printf("get one blog result error\n");
				mysql_free_result(res);
				return false;
			}
			MYSQL_ROW row = mysql_fetch_row(res);
			(*blog)["tag_id"]=std::stoi(row[0]);
			(*blog)["title"]=row[1];
			(*blog)["content"]=row[2];
			(*blog)["ctime"]=row[3];
			
			mysql_free_result(res);
			return true; } 
	private: 
		MYSQL *_mysql;//定义句柄
	
	};
	class TableTag{//标签信息类
		public:
			TableTag(MYSQL *mysql):_mysql(mysql){}
			bool Insert(Json::Value &tag){
#define INSERT_TAG "insert tb_tag values(null, '%s');"
				char tmp[1024] = {0};
				sprintf(tmp,INSERT_TAG,tag["name"].asCString());

				return MysqlQuery(_mysql,tmp);
			}
			bool Delete(int tag_id){
#define DELETE_TAG "delete from tb_tag where id=%d;"
				char tmp[1024] ={0};
				sprintf(tmp,DELETE_TAG,tag_id);

				return MysqlQuery(_mysql,tmp);
			}
			bool Update(Json::Value &tag){
#define UPDATE_TAG "update tb_tag set name='%s' where id=%d;"
				char tmp[1024] = {0};
				sprintf(tmp,UPDATE_TAG,tag["name"].asCString(),tag["id"].asInt());
				return MysqlQuery(_mysql,tmp);
			}

			bool GetAll(Json::Value *tags){
#define GETALL_TAG "select id, name from tb_tag;"
				g_mutex.lock();
				bool ret = MysqlQuery(_mysql,GETALL_TAG);
				if(ret == false){
					g_mutex.unlock();//错误时解锁退出,防止死锁
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				g_mutex.unlock();//成功时候解锁
				if (res == NULL){
					printf("store all tag result error:%s\n",mysql_error(_mysql));
					return false;
				}
				int row_num = mysql_num_rows(res);
				for(int i=0; i < row_num; i++){//遍历结果集
					MYSQL_ROW row = mysql_fetch_row(res);
					Json::Value tag;
					tag["id"] = std::stoi(row[0]);
					tag["name"] = row[1];
					tags->append(tag);
				}
				mysql_free_result(res);
				return true;
			}
			bool GetOne(Json::Value *tag){
#define GETONE_TAG "select id,name from tb_tag where id=%d;"
				g_mutex.lock();
				char tmp[1024] = {0};
				sprintf(tmp, GETONE_TAG,(*tag)["id"].asInt());

				bool ret = MysqlQuery(_mysql,tmp);
				if(ret == false){
					g_mutex.unlock();
					return false;
				}
				MYSQL_RES *res = mysql_store_result(_mysql);
				g_mutex.unlock();//成功时解锁
				if (res == NULL){
					printf("store one tag result error:%s\n",mysql_error(_mysql));
					return false;
				}
				int row_num = mysql_num_rows(res);
				if (row_num != 1){
					printf("get one tag result error\n");
					mysql_free_result(res);
					return false;
				}
				MYSQL_ROW row = mysql_fetch_row(res);
				(*tag)["name"] = row[1];
				mysql_free_result(res);
				return true;
			}


		private:
			MYSQL *_mysql;
	
	
	};
}
