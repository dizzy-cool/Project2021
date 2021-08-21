#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <mysql/mysql.h>


int main(){


	//1 初始化句柄
	//等同与 MYSQL mysql;
	//mysql_init(&mysql);
	MYSQL *mysql = mysql_init(NULL);//动态分布
	if(mysql == NULL){
		printf("init mysql error\n");
		return -1;//初始化失败 返回-1
	}
	//2 链接mysql服务器
	//127.0.0.1为本地的回环网卡地址
	if(	mysql_real_connect(mysql,"127.0.0.1","root","kb181818","db_blog",0,NULL,0) == NULL){
		printf("connect mysql sercer failed:%s\n", mysql_error(mysql));
		return -1;
	}
	//3 设置客户端字符集
	if (mysql_set_character_set(mysql,"utf8") != 0){
		printf("set character failed:%s\n", mysql_error(mysql));
		return -1;
	}		
	//4 选择要使用的数据库
	if(mysql_select_db(mysql,"db_blog") != 0){
		printf("select db failed:%s\n", mysql_error(mysql));
		return -1;
	}
	/*//5 创建表
	char *sql_str = "create table if not exists tb_stu(id int, name varchar(32), info text, score decimal(4,2),birth datetime);";*/

	//char* sql = "insert tb_stu values(1,'张三','真的好帅',99.456,'2002-02-28 12:00:00');";
	//char* sql = "update tb_stu set name='党康博',info='比谁都牛'where name='张三';";
	//char* sql = "delete from tb_stu where name='党康博';";
	
	char *sql = "select * from tb_stu;";
	int ret = mysql_query(mysql,sql);
	if (ret != 0) {
		printf("qurey mysql failed:%s\n", mysql_error(mysql));
		return -1;
	}
	
	MYSQL_RES *res = mysql_store_result(mysql);
	if(res == NULL){
		printf("store result failed:%s\n", mysql_error(mysql));
		return -1;
	}
	int num_row = mysql_num_rows(res);//行数
	int num_col = mysql_num_fields(res);//列数
	int i = 0;
	int j = 0;
	for(i = 0;i < num_row; i++){
		MYSQL_ROW row = mysql_fetch_row(res);
		for(j = 0; j < num_col; j++){
			printf("%s\t",row[j]);
		}
		printf("\n");
	}	
  	//释放结果集 防止内存泄漏
  	mysql_free_result(res);
	//关闭数据库
	mysql_close(mysql);//
	return 0;
}
  
