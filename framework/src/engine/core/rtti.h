/**
* @file rtti.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare RTTI class (Real Time Type Information)
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/

#ifndef RTTI_H
#define RTTI_H

class RTTI
{
private:
	const std::type_info info;
	const RTTI* parent;
public:
	RTTI(const std::type_info &info, const RTTI* parent);
	RTTI(const RTTI&) = delete;
	RTTI(RTTI&&) = delete;
	RTTI& operator=(const RTTI&) = delete;

	std::string name() const;

	inline bool is_exactly(const RTTI& rtti) const { return &rtti == this; }
private:
	bool is_derived(const RTTI* cur, const RTTI* rtti) const;	// recursion
public:
	inline bool is_derived(const RTTI& rtti) const { return is_derived(this, &rtti); }

	template<typename T>
	inline bool is_exactly() const {
		return &T::type == this;
	}

	template<typename T>
	inline bool is_derived() const {
		return is_derived(T::type);
	}
};

// RTTI declaration inside class
#define RTTI_DCL	\
static const RTTI type;

#define RTTI_IMPL(class_name, class_parent)	\
const RTTI class_name::type(typeid(class_name), &class_parent::type);

#define RTTI_IMPL(class_name)	\
const RTTI class_name::type(typeid(class_name), nullptr);

#endif