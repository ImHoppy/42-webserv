#include <string>
#include <sstream>
#include "Utils.hpp"

int32_t		StrToInt(std::string const & str)
{
	std::istringstream s(str);
	int32_t i;
	s >> i;
	return i;
}
