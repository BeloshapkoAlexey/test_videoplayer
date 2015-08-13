#include "VideoException.h"

VideoException::VideoException(const char* what)
{
	m_what = what;
}

VideoException::~VideoException()
{

}

std::string& VideoException::what()
{
	return m_what;
}