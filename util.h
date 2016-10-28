#ifndef _MY_UTIL_
#define _MY_UTIL_

#include <boost/filesystem.hpp>
#include <string>

namespace bf = boost::filesystem;
using std::string;

//@Params:1 path 2 pattern
string Get_Sub_Path_s(bf::path ,string);

#endif