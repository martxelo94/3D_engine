/**
* @file pool.h
* @author Markel Pisano , 540002615 , markel.p@digipen.edu
* @date 2021/01/02	(eus)
* @brief Declare Pool class
*
* @copyright Copyright (C) 2021 The Miss Cojones Morenos .
*/
#ifndef POOL_H
#define POOL_H

#define DEFAULT_POOL_SIZE 64u / sizeof(T)
#define POOL_SPEED_FOR_MEMORY_TRADEOFF 1

/*
	Storage for Blocks of the same type
*/

template<typename T, size_t N = DEFAULT_POOL_SIZE>
class Pool
{
public:
	typename T type;
	typename T* type_ptr;
private:
	//DATA
	static const size_t element_count = N;
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
	typedef std::stack<size_t, std::deque<size_t> > FREE_SLOTS_STACK;
	FREE_SLOTS_STACK free_stack;
#else
	size_t free_slot_count = element_count;
#endif
	typedef std::bitset<element_count> FREE_SLOTS_BITSET;
	FREE_SLOTS_BITSET free_bitset;	//always need this becouse the Block doesn't know if it is allocated
	Block<T> data[element_count];

public:
	Pool()
		: free_bitset(FREE_SLOTS_BITSET().set())
#if !POOL_SPEED_FOR_MEMORY_TRADEOFF
		, free_slot_count(element_count) {}
#else
	{
		std::vector<size_t> tmp(element_count);
		std::iota(std::begin(tmp), std::end(tmp), 0);
		free_stack = FREE_SLOTS_STACK(std::deque<size_t>(tmp.begin(), tmp.end()));
	}
#endif
	Pool(Pool const& other)
		: free_bitset(other.free_bitset)
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		, free_stack(other.free_stack)
#else
		, free_slot_count(other.free_slot_count)
#endif
	{
		std::memcpy(data, other.data, element_count * sizeof(T));
	}
	Pool& operator=(const Pool& other) {
		std::memcpy(data, other.data, element_count * sizeof(T));
		free_bitset = other.free_bitset;
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		free_stack = other.free_stack;
#else
		free_slot_count = other.free_slot_count;
#endif
		return *this;
	}
	inline static const size_t & size()
	{
		return element_count;
	}
	template<typename ...Args>
	T* allocate(Args&&...args)
	{
		//Find first free slot
		size_t index = first_available_slot();
		if (index == element_count)
			return nullptr;
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		//as first_available_slots returns the top of the stack, we pop it
		free_stack.pop();
#else
		--free_slot_count;
#endif
		free_bitset.set(index, 0);
		//allocate memory and construct with args
		return data[index].Block<T>::allocate(std::forward(args)...);

	}
	bool free(void* mem_address)
	{
		//Get index
		size_t index = (size_t)mem_address - (size_t)&data[0];
		index /= sizeof(T);
		//Check if is out of bounds
		if (index >= element_count)
			return false;
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		//push to the stack
		free_stack.push(index);
#else
		// else, set index to true
		++free_slot_count;
#endif
		free_bitset.set(index, 1);
		//call destructor
		data[index].Block<T>::free();
		return true;
	}
	/*
		free every allocated item
	*/
	void free() {
		//call destructor foreach allocated element
		for (size_t i = 0; i < element_count; ++i)
			if (free_bitset[i] == false)
				data[i].Block<T>::free();
		//reset all
		*this = Pool();
	}
	inline bool is_full() const
	{
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		return free_stack.empty();
#else
		return free_slot_count == 0;
#endif
	}
	inline bool empty() const {
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		return free_stack.size() == element_count;
#else
		return free_slot_count == element_count;
#endif
	}
	bool belongs(void* mem_address) const
	{
		return (size_t)mem_address
			>= (size_t)data
			&& (size_t)mem_address
			< (size_t)&data[element_count];
	}
	size_t first_available_slot() const
	{
#if POOL_SPEED_FOR_MEMORY_TRADEOFF
		// get stack's top
		if (free_stack.empty())
			return element_count;	// no slots available
		return free_stack.top();
#else
		//linear iteration
		for (size_t i = 0; i < element_count; ++i) {
			if (free_bitset[i]) return i;
		}
		return element_count;
#endif
	}

	//call function foreach allocated item
	template<typename F>
	void for_each(F&& f) {
		for (size_t i = 0; i < element_count; ++i)
			if (free_bitset[i] == false)
				f(data[i]);
	}


};


#endif
