# ZabbixSender
c++ ����zabbix��Ŀ
## ������
asio "only header ��������ͨ��"
nlohman:json  "json�Ľ��������л�"
spdlog  "��־������"
## Zabbix����淶
����Ϊcpp��Ŀ�������Ķ���һ��host����Ϊcpp ��zabbixר��
key ����zpp��Ŀ������(Ӣ����)�����һ����Ŀ����һ�����еĳ�������һ�������㶨λ�������

�������в�ȡ����ƥ��alert,���������Ĺ�������(���������ϣ����з����������ݾ�������Ϊ����)

��Ϊ�й����ô��������ɣ��ⲿ���ɳ������������֪ͨ��ά���߿����ķ�ʽ�й�
## ʹ�ù淶
>> ע�⣺��Ҫ���������ַ�����zabbix������
### �����ļ�
�����ļ���Ŀ¼�´���zabbixĿ¼��Ŀ¼�´��zabbix.json�����ļ���
```
{
	"ZabbixHost": "127.0.0.1",
	"ZabbixPort": 10051,
	"MonitoringHost": "zabis",
	"MonitoringKey": "alert"
}
```
���������
|������|����|
|:--|:--|
|ZabbixHost|zabbix�����ip|
|ZabbixPort|zabbix����˼����Ķ˿ڣ�һ������Ϊ10051|
|MonitoringHost|zabbix web �д�����host|
|MonitoringKey|zabbix �д�����item������Ϊ��Ŀ���ƣ������Ļ������ʼ���������֪ͨ��ʽ����ֱ�Ӷ�λ������ĳ���|  
## c++ʹ�� 
1. ʹ��ʱһ��Ҫasio,spdlog,nlohman(���û�����Ƶ�includeĿ¼)
2. ��ҪzabbixĿ¼���Լ��������ļ�

demo
```
#include "ZbxSender.h"
ZabbixSend("");
```