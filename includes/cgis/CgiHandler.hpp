/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/19 16:06:06 by pcaplat           #+#    #+#             */
/*   Updated: 2026/11/10 16:07:52 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGIHANDLER_HPP
# define CGIHANDLER_HPP

# include "../WebservInclude.h"
# include "../Utils/Utils.hpp"

class CgiHandler
{
	private:
		pid_t		_pid;		// pid du script, -1 si aucun
		int			_inFd;		// on ecrit le body ici -> stdin du script
		int			_outFd;		// on lit ici <- stdout du script
		time_t		_startTime;
		std::string	_body;		// body de la requete a envoyer au script
		size_t		_written;	// nombre d'octets du body deja envoyes
		std::string	_output;	// tout ce que le script a ecrit

		std::vector<std::string>	buildEnv(const HttpRequest& req, const CgiInfo& info) const;
		void	execChild(int inPipe[2], int outPipe[2], const HttpRequest& req, const CgiInfo& info) const;
		void	parseHeaderLine(const std::string& line, ResponseData& res) const;

	public:
		CgiHandler();
		~CgiHandler();

		bool			start(const HttpRequest& req, const CgiInfo& info);
		int				writeBody();
		int				readOutput();
		bool			isTimedOut() const;
		void			stop();
		void			forgetFd(int fd);
		ResponseData	buildResponse() const;

		int		getInFd() const;
		int		getOutFd() const;
};

#endif
