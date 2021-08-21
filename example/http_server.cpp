#include "httplib.h"

int main(void)
{
	 using namespace httplib;
	 Server svr;//实例化一个server对象
	 //svr.Get() --成员函数--注册请求-业务处理路由关系--告诉服务器哪一个请求对应使用哪个函数进行业务处理
	 //第一个参数:"/hi"--url中path资源路径
	 svr.Get("/hi", [](const Request& req, Response& res) {
		      res.set_content("Hello World!", "text/plain");//匿名函数:兰姆达表达式
			  });
	 svr.Get(R"(/numbers/(\d+))", [&](const Request& req, Response& res) {
		    auto numbers = req.matches[1];
		    res.set_content(numbers, "text/plain");
			  });
	 svr.Get("/body-header-param", [](const Request& req, Response& res) {
	      if (req.has_header("Content-Length")) {
			  auto val = req.get_header_value("Content-Length");
			}
		  if (req.has_param("key")) {
		      auto val = req.get_param_value("key");
		    }
		  res.set_content(req.body, "text/plain");
		  });
	 svr.Get("/stop", [&](const Request& req, Response& res) {
						    svr.stop();
			 });
	 svr.listen("0.0.0.0", 80);//开始服务器监听
}
