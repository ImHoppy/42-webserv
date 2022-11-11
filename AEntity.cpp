/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AEntity.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/11 21:39:30 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/11 22:02:13 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AEntity.hpp"

AEntity::AEntity(void) : _type("entity") {}

AEntity::~AEntity(void) {}

AEntity::AEntity(const AEntity& src) : _type(src._type) {}

AEntity::AEntity(const std::string& type) : _type(type) {}

AEntity&		AEntity::operator=(AEntity const & rhs)
{
	_type = rhs._type;
	return *this;
}
