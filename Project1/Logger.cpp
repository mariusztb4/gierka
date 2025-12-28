#include "Logger.h"
#include "Engine.h"
#include <iostream>

void Logger::Info(const std::string& msg)
{
    std::cout << "[INFO] " << msg << std::endl;
    Engine::GetInstance(1920, 1080).GetConsole().AddLog(msg, MessageType::Info);
}

void Logger::Warning(const std::string& msg)
{
    std::cout << "[WARNING] " << msg << std::endl;
    Engine::GetInstance(1920, 1080).GetConsole().AddLog(msg, MessageType::Warning);
}

void Logger::Error(const std::string& msg)
{
    std::cerr << "[ERROR] " << msg << std::endl;
    Engine::GetInstance(1920, 1080).GetConsole().AddLog(msg, MessageType::Error);
}
