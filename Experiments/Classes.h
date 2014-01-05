#pragma once
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <vector>

class Command {
	public:
		virtual void Execute(const std::vector<int> *units) = 0;
		virtual ~Command() { }
};