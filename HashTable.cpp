// HashTable.cpp
#include "HashTable.h"
#include <functional>
#include <stdexcept>

HashTable::HashTable(size_t size) noexcept : _capacity(size), _filled(0)
{
	table.resize(size);
}

HashTable::~HashTable() = default;

void HashTable::insert(const KeyType &key, const ValueType &value) 
{
	size_t index = hash_function(key);
	auto &node = table[index];
	for (auto &pair : node)
	{
		if (pair.first == key) 
		{
			pair.second = value;
			return;
		}
	}
	node.emplace_back(key, value);
	_filled++;

	if (getLoadFactor() > 0.75) 
	{
		size_t new_capacity = _capacity * 2;
		std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);

		auto old_table = std::move(table);
		_capacity = new_capacity;
		_filled = 0;

		for (auto &old_node : old_table) 
		{
			for (auto &pair : old_node) 
			{
				size_t new_index = hash_function(pair.first);
				new_table[new_index].emplace_back(pair);
				_filled++;
			}
		}

		table = std::move(new_table);
	}
}

bool HashTable::find(const KeyType &key, ValueType &value) const 
{
	size_t index = hash_function(key);
	const auto &node = table[index];
	for (const auto &pair : node) 
	{
		if (pair.first == key) 
		{
			value = pair.second;
			return true;
		}
	}
	return false;
}

void HashTable::remove(const KeyType &key) 
{
	size_t index = hash_function(key);
	auto &node = table[index];
	for (auto it = node.begin(); it != node.end(); ++it) 
	{
		if (it->first == key) 
		{
			node.erase(it);
			_filled--;
			return;
		}
	}
}

ValueType &HashTable::operator[](const KeyType &key) 
{
	size_t orig_index = hash_function(key);
	auto &orig_node = table[orig_index];
	for (auto &pair : orig_node) 
	{
		if (pair.first == key) 
		{
			return pair.second;
		}
	}

	orig_node.emplace_back(key, ValueType{});
	_filled++;

	if (getLoadFactor() > 0.75) 
	{
		size_t new_capacity = _capacity * 2;
		std::vector<std::list<std::pair<KeyType, ValueType>>> new_table(new_capacity);

		auto old_table = std::move(table);
		_capacity = new_capacity;
		_filled = 0;

		for (auto &old_node : old_table) 
		{
			for (auto &pair : old_node) 
			{
				size_t new_index = hash_function(pair.first);
				new_table[new_index].emplace_back(pair);
				_filled++;
			}
		}

		table = std::move(new_table);

		size_t new_index = hash_function(key);
		auto &new_node = table[new_index];
		for (auto &pair : new_node) 
		{
			if (pair.first == key) 
			{
				return pair.second;
			}
		}
		throw std::runtime_error("Элемент не найден (после ресайза)");
	}

	return orig_node.back().second;
}

double HashTable::getLoadFactor() 
{
	return static_cast<double>(_filled) / _capacity;
}

size_t HashTable::hash_function(const KeyType &key) const 
{
	const double A = 0.618033988749895;
	size_t hash = 0;
    
	for (char c : key) 
	{
		hash = hash * 31 + static_cast<size_t>(c);
	}
    
	double val = hash * A;
	val -= static_cast<size_t>(val); // дробная часть
	return static_cast<size_t>(_capacity * val);
}
