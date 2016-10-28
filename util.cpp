//#include "stdafx.h"
#include "util.h"
#include <assert.h>

//get specific sub path from a absolute path

string Get_Sub_Path_s(bf::path p , string pat)
{
	string p_s = p.string();
	size_t len = strlen(p_s.c_str());
	size_t pat_len = strlen(pat.c_str());
	assert( len>0 && pat_len>0 );
	//back finder
	size_t off = p_s.rfind(pat);
	//not find
	if(off == string::npos)
		return "";
	off += pat_len;
	//out range
	if(off > len)
		return "";
	//substr
	return p_s.substr(off);
}