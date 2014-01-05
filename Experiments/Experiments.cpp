// Experiments.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include "Classes.h"


int _tmain(int argc, _TCHAR* argv[])
{
	A *b = new B();
	A *c = new B();
	std::ofstream stream;
	boost::archive::text_oarchive serializer(stream);
	serializer << b;

	serializer << c;

	std::ifstream stream2;
	boost::archive::text_iarchive chive(stream2);
	chive >> b;



	return 0;
}