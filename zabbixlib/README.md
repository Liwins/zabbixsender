# ZabbixSender
c++ 发送zabbix项目
## 依赖库
asio "only header 用于网络通信"
nlohman:json  "json的解析和序列化"
spdlog  "日志三方库"
## Zabbix定义规范
可以为cpp项目但单独的定义一个host，作为cpp 的zabbix专用
key 定义zpp项目的名称(英文名)，最好一个项目或者一个运行的程序设置一个，方便定位错误程序。

触发器中采取正则匹配alert,或者其他的过滤条件(该条件符合，所有发过来的数据均触发行为即可)

行为中关联该触发器即可，这部分由出现问题后怎样通知运维或者开发的方式有关
## 使用规范
>> 注意：不要发送中文字符串给zabbix服务器
### 配置文件
运行文件的目录下创建zabbix目录，目录下存放zabbix.json配置文件。
```
{
	"ZabbixHost": "127.0.0.1",
	"ZabbixPort": 10051,
	"MonitoringHost": "zabis",
	"MonitoringKey": "alert"
}
```
配置项介绍
|配置项|介绍|
|:--|:--|
|ZabbixHost|zabbix服务端ip|
|ZabbixPort|zabbix服务端监听的端口，一般设置为10051|
|MonitoringHost|zabbix web 中创建的host|
|MonitoringKey|zabbix 中创建的item，建议为项目名称，这样的话报警邮件或者其他通知方式可以直接定位出问题的程序|  
## c++使用 
1. 使用时一定要asio,spdlog,nlohman(如果没有则复制到include目录)
2. 需要zabbix目录，以及其配置文件

demo
```
#include "ZbxSender.h"
ZabbixSend("");
```