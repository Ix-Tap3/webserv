#include <HttpParser.hpp>
#include <HttpException.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

/*
** Le fichier de tests est decoupe en blocs separes par la ligne "======".
** Chaque bloc commence par UNE ligne de description (terminee par un
** simple '\n'), suivie de la requete HTTP brute (avec de vrais \r\n).
*/
static std::vector<std::string> splitBlocks(const std::string& content, const std::string& delim)
{
	std::vector<std::string>	blocks;
	size_t						start = 0;
	size_t						pos = content.find(delim);

	while (pos != std::string::npos)
	{
		blocks.push_back(content.substr(start, pos - start));
		start = pos + delim.length();
		pos = content.find(delim, start);
	}
	blocks.push_back(content.substr(start));
	return (blocks);
}

// Affiche les \r et \n de maniere visible pour bien voir la requete testee
static void	printVisible(const std::string& raw)
{
	if (raw.empty())
	{
		std::cout << "  (vide)" << std::endl;
		return ;
	}
	std::cout << "  ";
	for (size_t i = 0; i < raw.size(); ++i)
	{
		if (raw[i] == '\r')
			std::cout << "\\r";
		else if (raw[i] == '\n')
			std::cout << "\\n\n  ";
		else
			std::cout << raw[i];
	}
	std::cout << std::endl;
}

int	main(int argc, char **argv)
{
	std::string	filename = "tests.http";

	if (argc > 1)
		filename = argv[1];

	std::ifstream	file(filename.c_str(), std::ios::binary);
	if (!file.is_open())
	{
		std::cerr << "Impossible d'ouvrir le fichier: " << filename << std::endl;
		return (1);
	}

	std::stringstream	buffer;
	buffer << file.rdbuf();
	std::string			content = buffer.str();

	std::vector<std::string>	blocks = splitBlocks(content, "\n======\n");
	int							total = 0;
	int							noException = 0;
	int							exceptionThrown = 0;

	for (size_t i = 0; i < blocks.size(); ++i)
	{
		std::string	block = blocks[i];
		if (block.empty())
			continue;

		size_t	nl = block.find('\n');
		std::string	desc = (nl == std::string::npos) ? block : block.substr(0, nl);
		std::string	raw = (nl == std::string::npos) ? "" : block.substr(nl + 1);

		total++;
		std::cout << "================ TEST " << total << " ================" << std::endl;
		std::cout << "Description   : " << desc << std::endl;
		std::cout << "Requete brute :" << std::endl;
		printVisible(raw);
		std::cout << "Resultat      :" << std::endl;

		HttpParser	parser;
		try
		{
			Header	h = parser.ParseHeader(raw);
			std::cout << "  -> OK, " << h._headersFields.size()
				<< " header(s) parse(s)" << std::endl;
			for (size_t j = 0; j < h._headersFields.size(); ++j)
			{
				std::cout << "     [" << h._headersFields[j].first << "] = ["
					<< h._headersFields[j].second << "]" << std::endl;
			}
			noException++;
		}
		catch (HttpException& e)
		{
			std::cout << "  -> HttpException (code " << e.getStatusCode()
				<< "): " << e.what() << std::endl;
			exceptionThrown++;
		}
		catch (std::exception& e)
		{
			std::cout << "  -> std::exception INATTENDUE: " << e.what() << std::endl;
			exceptionThrown++;
		}
		std::cout << std::endl;
	}

	std::cout << "=====================================================" << std::endl;
	std::cout << "Total: " << total << " | Sans exception: " << noException
		<< " | Avec exception: " << exceptionThrown << std::endl;

	return (0);
}
