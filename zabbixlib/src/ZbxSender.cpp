#include "ZbxSender.h"

#include <nlohmann/json.hpp>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>

ZbxSender zbx;
void ZabbixSend(std::string msg)
{
	zbx.send(msg);
}

struct zabbixCoreData {
	std::string host;
	std::string key;
	std::string value;
};
struct zabbixData {
	std::string request = "sender data";
	std::vector<zabbixCoreData> data;
};
static std::string PathSeparatorRS()
{
#ifdef _WIN32
	return "\\";
#else
	return "/";
#endif
}
static bool CreateFolderRS(std::string strDir) {
#ifdef _WIN32
	return CreateDirectoryA(strDir.c_str(), NULL);
#else
	return mkdir(strDir.c_str(), 0700);
#endif
	return 0;
}
static std::string getAppPathRS() {
#ifdef _WIN32
	char szPath[MAX_PATH];
	HMODULE hModule = ::GetModuleHandleA(".");
	::GetModuleFileNameA(hModule, szPath, MAX_PATH);
	char* find = strrchr(szPath, '\\');
	if (find) {
		*(find + 1) = 0;
	}
	return szPath;
#else
	char szPath[MAX_PATH];
	char* s = getcwd(szPath, MAX_PATH);
	strcat(szPath, "/");
	return szPath;
#endif
}
template<class T>
static ResultString ClassToString(std::string& toString, const T& value) {
	try {
		T a{ value };
		nlohmann::json j(a);
		std::stringstream s;
		s << j.dump() << std::endl;
		toString = s.str();
		return ResultString(true, "");
	}
	catch (const  std::exception & e)
	{
		std::this_thread::sleep_for(std::chrono::seconds(2));
		return ResultString(false, e.what());
	}
}
template<class T>
static ResultString FileToClass(const std::string& filePath, T& value) {
	try {
		std::ifstream  input(filePath);
		nlohmann::json j;
		input >> j;
		value = j.get<T>();

		return ResultString(true, "");
	}
	catch (const  std::exception & e)
	{

		std::this_thread::sleep_for(std::chrono::seconds(2));
		return ResultString(false, e.what());
	}
}
ZbxSender::ZbxSender()
{
	// Create a daily logger - a new file is created every day on 2:30am
	std::string  logdir = getAppPathRS() + "zabbix"+ PathSeparatorRS() +"logs";
	CreateFolderRS(logdir);
	logger = spdlog::daily_logger_mt("daily_logger", logdir+"\\zabbix.log", 2, 30);
	spdlog::flush_every(std::chrono::seconds(1));
	auto resFileToclass = FileToClass(getAppPathRS() + "zabbix/zabbix.json", config);
	if (!resFileToclass.result) {
		logger->info("zabbix config not find or error:{}",resFileToclass.resultmsg);
	}
	ready = true;
	std::thread worker(&ZbxSender::run, this);
	worker.detach();
}



void ZbxSender::send(std::string data)
{
	queue.push(data);
}

void ZbxSender::run()
{
	while (ready)
	{
		if (!queue.empty())
		{
			std::string data = queue.front();
			queue.pop();
			tcp_send(data);
		}

		if (queue.size() > 100)
		{
			queue.empty();
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

ZbxSender::~ZbxSender()
{
	ready = false;
}




bool ZbxSender::tcp_send(std::string value) {
	value = "alert :" + value;
	asio::io_service io_service;
	asio::ip::tcp::socket socket(io_service);
	try
	{
		socket.connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(config.ZabbixHost), config.ZabbixPort));
	}
	catch (std::exception & e)
	{
		logger->info("{}: datadetail:{}", e.what(),value);
		return false;
	}
	
	if (value.length()>4000)
	{
		logger->error("msg is too long:{}", value);
		return false;
	}

	char msg[4096];
	memset(msg, 0x00, 4096);
	strcpy(msg, "ZBXD");
	msg[4] = 0x01;
	zabbixData data;
	zabbixCoreData coredata;
	coredata.host = config.MonitoringHost;
	coredata.key = config.MonitoringKey;
	coredata.value = value;
	data.data.push_back(coredata);
	std::string DATA ;
	ClassToString(DATA, data);
	uint8_t data_len = DATA.length();
	memcpy(msg + 5, &data_len, sizeof(uint8_t));
	memcpy(msg + 13, DATA.c_str(), DATA.length());

	asio::error_code error;
	asio::write(socket, asio::buffer(msg, data_len + 13), error);

	if (error) {
		TextError = "send failed: " + error.message();
		logger->info("{} data detail:{}", TextError,DATA);
	}
	else {
		logger->info("send:{}", value);
	}

	asio::streambuf receive_buffer;
	asio::read(socket, receive_buffer, asio::transfer_all(), error);

	if (error && error != asio::error::eof) {
		logger->info("receive failed: {}", error.message());
	}
	else {
		const char* data = asio::buffer_cast<const char*>(receive_buffer.data());
		logger->info("receive data:{}", data);
	}

	return 0;
}

void from_json(const nlohmann::json& j, ZabbixConfig& p)
{
	j.at("ZabbixHost").get_to(p.ZabbixHost);
	j.at("ZabbixPort").get_to(p.ZabbixPort);
	j.at("MonitoringHost").get_to(p.MonitoringHost);
	j.at("MonitoringKey").get_to(p.MonitoringKey);
}
void to_json( nlohmann::json& j, const  zabbixCoreData& p)
{
	j = nlohmann::json{ {"host", p.host}, {"key", p.key}, {"value", p.value} };
}
void to_json(nlohmann::json& j,const zabbixData& p)
{
	j = nlohmann::json{ {"request", p.request}, {"data", p.data}};
}
