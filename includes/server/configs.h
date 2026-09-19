/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configs.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 09:59:58 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/19 11:09:50 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIGS_H
# define CONFIGS_H

# include <string>
# include <vector>
# include <map>

struct	LocationConfig
{
	int									max_body_size; // -1 = not set, inherited from server by default
	int									redirect_code; // -1 = not set, can be unset, have to be set if redirect_path is set
	bool								directory_listing; // inherited from server by default
	std::string							redirect_path; // empty = not set, can be unset, have to be set if redirect_code is set
	std::string							root; // empty = not set, inherited from server by default
	std::string							default_page; // inherited from server by default
	std::string							upload; // empty = not set, can be unset
	std::string							path; // empty = not set, must be set
	std::vector<std::string>			methods; // empty = not set, [ GET, POST, DELETE ] by default
	std::map<int, std::string>			error_pages; // empty = not set, inherited from server by default
};

struct	ServerConfig
{
	std::string							ip; // empty = not set, 0.0.0.0 by default or 127.0.0.1
	std::string							root; // empty = not set, must be set
	std::string							default_page; // empty = not set, "serv.html" by default
	std::vector<int>					port; // empty = not set, must be set
	std::vector<std::string>			hosts; // empty = not set, must be set or webserv by default
	std::vector<LocationConfig>			locations; // empty = not set, must be set
	std::map<std::string, std::string>	cgis; // empty = not set, can be unset
	std::map<int, std::string>			error_pages; // empty = not set, can be unset, "serv_error.html" by default
	int									max_body_size; // -1 = not set, 1000000 by default ?
	bool								directory_listing; // false by default
};


#endif
