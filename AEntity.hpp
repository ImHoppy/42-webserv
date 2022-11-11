/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AEntity.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 21:37:40 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/11 21:59:32 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AENTITY_HPP
# define AENTITY_HPP

# include <iostream>

class	AEntity {
	public:
		AEntity(void);
		virtual ~AEntity(void);
		AEntity(AEntity const & src);
		AEntity(const std::string& type);
		AEntity &	operator=(AEntity const & rhs);

		virtual const std::string&	getType(void) const = 0;

	protected:
		std::string		_type;
};

#endif
