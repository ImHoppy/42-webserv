/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request_tests.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/03 12:43:43 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/04 14:36:30 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request.hpp"
#include "response.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>


std::string	rqst_file_to_string(const std::string & filename)
{
	std::ifstream	file(filename.c_str());
	std::string		readout;
	std::string		raw_rqst;
	
	raw_rqst += "\r\n";
	raw_rqst += "\r\n";
	raw_rqst += "\r\n";
	if (!file)
	{
		perror("error opening rqst file");
		exit(1);
	}
	while (getline(file, readout))
	{
		if (readout.size() != 0)
			raw_rqst += readout;
		raw_rqst += "\r\n";
	}
	file.close();
	return raw_rqst;
}

int	main(int argc, char **argv)
{
	std::string	raw_rqst;
	if (argc == 2)
		raw_rqst = rqst_file_to_string(argv[1]);
	else
		raw_rqst = rqst_file_to_string("request_template.txt");

//	request		rqst(raw_rqst);
//	std::cout << rqst << std::endl;
//	std::cout << "\nRaw request:\n" << rqst.getRawRequest() << std::endl;

	response	rpse(raw_rqst);
	return (0);
}
