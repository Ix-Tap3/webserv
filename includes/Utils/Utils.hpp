#ifndef UTILS_HPP
# define UTILS_HPP

# include <WebservInclude.h>

namespace Utils
{
	std::vector<std::string>	strSplit(const std::string &s, char delim);
	int			stringToInt(std::string str);
	std::string	strToMin(std::string& str);
	std::string	intToString(int nb);
	bool		isTchar(char c);
	void		DeleteUselessSpace(std::string& str);
}

#endif