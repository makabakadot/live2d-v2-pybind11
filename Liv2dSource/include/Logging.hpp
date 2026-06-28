#pragma once
#include <iostream>
#include <string>

namespace live2d
{
	void Info(const std::string& msg)
	{
		std::cout << msg << std::endl;
	}
	void Debug(const std::string& msg)
	{
		std::cout << msg << std::endl;
	}
}