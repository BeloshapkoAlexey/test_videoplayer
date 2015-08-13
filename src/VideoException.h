#pragma once

#include <string>

class VideoException
{
public:
	VideoException(const char* what);
	~VideoException();

	std::string& what();

private:
	std::string m_what;
};