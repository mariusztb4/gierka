#pragma once
#include <string>
#include "MessageType.h"

class Logger
{
public:
    static void Info(const std::string& msg);
    static void Warning(const std::string& msg);
    static void Error(const std::string& msg);
};