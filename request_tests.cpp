/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_tests.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/03 12:43:43 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/03 13:10:46 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


std::string&	rqst_file_to_string(const std::string & filename)
{
	std::ifstream	file(filename);
	std::string		readout;
	std::string		raw_rqst;
	
	if (!file)
	{
		perror("error opening rqst file");
		exit(1);
	}
}

int	main(int argc, char **argv)
{
	std::string	raw_rqst;
	if (argc == 2)
		raw_rqst = rqst_file_to_string(argv[1]);
	else
		raw_rqst = rqst_file_to_string("request_template.txt");
	
		
	return (0);
}
