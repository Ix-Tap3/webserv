#include <Utils.hpp>

std::vector<std::string> Utils::strSplit(const std::string &s, char delim)
{
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = s.find(delim);

	while (end != std::string::npos)
	{
		tokens.push_back(s.substr(start, end - start));
		start = end + 1;
		end = s.find(delim, start);
	}
	tokens.push_back(s.substr(start));
	return tokens;
}

int	Utils::stringToInt(std::string str)
{
	int					res;
	std::stringstream	ss;

	ss << str;
	ss >> res;

	return (res);
}

std::string	Utils::intToString(int nb)
{
	std::stringstream	ss;

	ss << nb;
	return (ss.str());
}

std::string	Utils::strToMin(std::string& str)
{
	std::string out(str);
	for (size_t i = 0; i < out.size(); ++i)
		out[i] = std::tolower(static_cast<unsigned char>(out[i]));
	return out;
}

bool	Utils::isTchar(char c)
{
	if (std::isalnum(static_cast<unsigned char>(c)))
		return true;

	static const std::string special = "!#$%&'*+-.^_`|~";
	return (special.find(c) != std::string::npos);
}

void	Utils::DeleteUselessSpace(std::string& str)
{
	size_t start = 0;
	while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start])))
		start++;

	if (start == str.length())
	{
		str.clear();
		return ;
	}

	size_t end = str.length() - 1;
	while (end > start && std::isspace(static_cast<unsigned char>(str[end])))
		end--;

	str = str.substr(start, end - start + 1);
}
