#ifndef ZBXSENDER_HPP
#define ZBXSENDER_HPP

#include <string>
#include <queue>
#include <thread>
#include <iostream>
#include "asio.hpp"
#include <atomic>
#include "nlohmann/json.hpp"
#include "spdlog/sinks/daily_file_sink.h"
#include <chrono>
void ZabbixSend(std::string msg);
struct ResultString {
	bool result = false;
	std::string resultmsg = "";
	ResultString(bool res, std::string msg) :result(res), resultmsg(msg) {}
};
struct ZabbixConfig {
	std::string ZabbixHost;
	int ZabbixPort;
	std::string MonitoringHost;
	std::string MonitoringKey;
};
void from_json(const nlohmann::json& j, ZabbixConfig& p);
class ZbxSender
{
public:
	ZbxSender();
	std::atomic<bool> ready;
	
	void send(std::string data);


	~ZbxSender();
private:
	std::shared_ptr<spdlog::logger> logger;
	std::queue<std::string> queue;
	std::string TextError;
	ZabbixConfig config;
	void run();
	bool tcp_send(std::string value);
};
#endif // ZBXSENDER_HPP