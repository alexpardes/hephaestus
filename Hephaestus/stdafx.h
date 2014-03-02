#pragma once

#include "SDKDDKVer.h"

#include <cassert>
#include <cmath>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#pragma warning( push )
#pragma warning( disable : 4250 )
#include <boost/asio.hpp>
#pragma warning( pop )

#include <boost/serialization/export.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/thread.hpp>

#include <json/json.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>