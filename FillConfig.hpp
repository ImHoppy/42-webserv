#ifndef FILLCONFIG_HPP
# define FILLCONFIG_HPP

# include "GeneralConfig.hpp"
# include "LocationConfig.hpp"
# include <string>
# include <iostream>
# include <sstream>
# include "Parsing.hpp"
# include "Utils.hpp"
# include <algorithm>

typedef std::vector<std::pair<std::string, std::string> > key_value_t;
void	locationBlock(ServerConfig & server, key_value_t::iterator & it);
void	serverBlock(GeneralConfig & config, key_value_t::iterator & it);
bool	ServerNamesIsEqual(std::vector<std::string> const & a, std::vector<std::string> const & b);
void	CheckVirtualServer(GeneralConfig & config);
void	fillConfig(key_value_t key_value, GeneralConfig & config);

#endif
