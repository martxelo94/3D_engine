/**
* @file rtti.cpp
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Implement RTTI class (Real Time Type Information)
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#include <pch.h>

RTTI::RTTI(const std::type_info &info, const RTTI* parent)
	: info(info), parent(parent)
{}

bool RTTI::is_derived(const RTTI* cur, const RTTI* rtti) const {
	if (!cur) return false;
	if (cur == rtti) return true;
	return is_derived(cur->parent, rtti);
}