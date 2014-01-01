#ifndef VECTOR_SERIALIZATION_
#define VECTOR_SERIALIZATION_

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "Util.h"

namespace boost {
namespace serialization {
	template<class Archive>
	void serialize(Archive &ar, Vector2f &vector, const unsigned int version) {
		ar & vector.x;
		ar & vector.y;
	}

	template<class Archive>
	void serialize(Archive &ar, Vector2i &vector, const unsigned int version) {
		ar & vector.x;
		ar & vector.y;
	}
}
}

#endif