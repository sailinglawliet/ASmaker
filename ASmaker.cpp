// ASmaker.cpp : 定义控制台应用程序的入口点。
//
// copyright: sails鸢@oschina
// bfs mkr
// boost filesystem + boost program_options
//#include "stdafx.h"
#include "src4.h"
#include "util.h"

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <fstream>
#include <iterator>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

namespace bf = boost::filesystem;
namespace bpo = boost::program_options;

using std::vector;
using std::set;
using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::ostream_iterator;
using std::ios;

typedef std::queue<bf::path> dirqueue;

//@params: inpath, outpath , delima , mode , include , exclude
bool procedure_dir(bf::path _in , bf::path _out , string & _spliter , int _mode , set<string> & ft , set<string> & eft);
bool procedure_file(const bf::path & _in , const bf::path & _out, int _mode);

int main(int argc, char * argv[])
{
	//dec mode：1 src4,0 xor,by default 1
	int dec_mode = 1;
	string input_path_s = "";
	string output_path_s = "";
	set<string> f_type;				//include file type
	set<string> exf_type;			//exclude file type
	bpo::options_description op_desc("ASmaker，param rules like below，--i= or -i ");
	op_desc.add_options()
		("help,h", "Help")
		("inpath,i", bpo::value<string>(), "set input file path")
		("outpath,o",bpo::value<string>(),"set output file path, please include assets ")
		("mode,m",bpo::value<int>(&dec_mode)->default_value(1),"set decrypt mode，1 for rc4，0 for xor, by default is 1")
		("filetype,f",bpo::value< vector<string> >()->multitoken(),"set target type")
		("exfiletype,e",bpo::value< vector<string> >()->multitoken(),"set exclude target type")
	;
	
	bpo::variables_map vm;
	bpo::store(bpo::parse_command_line(argc, argv, op_desc), vm);
	bpo::notify(vm); 

	if(vm.count("help"))
	{
		cout << op_desc << "\n";
		return 1;
	}
	if(vm.count("inpath"))
	{
		input_path_s =  vm["inpath"].as<string>();
	}
	else
	{
		cout<<"plz input thy file or directories"<<endl;
		return 1;
	}
	if(vm.count("outpath"))
	{
		output_path_s = vm["outpath"].as<string>();
	}
	else
	{
		cout<<"plz set output path"<<endl;
		return 1;
	}
	if(vm.count("mode"))
	{
		dec_mode = vm["mode"].as<int>();
	}
	
	if (vm.count("filetype"))
	{
		for(vector<string>::const_iterator itor = vm["filetype"].as< vector<string> >().begin();itor != vm["filetype"].as< vector<string> >().end();++itor)
		{
			f_type.insert(*itor);
		}
		cout<<"type to be done is ";
		copy(f_type.begin(),f_type.end(),ostream_iterator<string>(cout," "));
		cout<<endl;
	}

	if (vm.count("exfiletype"))
	{
		for(vector<string>::const_iterator itor = vm["exfiletype"].as< vector<string> >().begin();itor != vm["exfiletype"].as< vector<string> >().end();++itor)
		{
			exf_type.insert(*itor);
		}
		cout<<"type to be ex is ";
		copy(exf_type.begin(),exf_type.end(),ostream_iterator<string>(cout," "));
		cout<<endl;
	}
	
	//check the intersection of include file type & exclude file type
	if((!f_type.empty())&&(!exf_type.empty()))
	{
		set<string> inter_sec;
		std::set_intersection(f_type.begin(), f_type.end(), exf_type.begin(), exf_type.end(),std::inserter(inter_sec, inter_sec.begin()));

		if (!inter_sec.empty())
		{
			cout<<"include type and exclude type can't set same one, plz check your arguments!"<<endl;
			return 1;
		}
	}

	//root dir
	bf::path scn_rt_pth(input_path_s.c_str());
	string scn_rt_s = scn_rt_pth.string();
	cout<<"thy mkr is under "<<scn_rt_s<<endl;

	//filesystem exam
	if(!bf::exists(scn_rt_pth))
	{
		cout<<"this is no name of"<<scn_rt_s<<"directory ，plz check your arguments!"<<endl;
		return 1;
	}

	//是否是文件夹
	bool is_dir_input = bf::is_directory(scn_rt_pth);

	bf::path real_root_path = scn_rt_pth;
	string delima = real_root_path.filename().string();

	//output root dir
	bf::path out_root_path(output_path_s.c_str());

	if (is_dir_input) 
	{
		if(!bf::exists(out_root_path))
		{
			bf::create_directories(out_root_path);
			cout<<"create target directory "<<out_root_path.string()<<endl;
		}
	}

	bool res_maker = false;
	if (is_dir_input)
	{
		res_maker = procedure_dir(real_root_path , out_root_path , delima , dec_mode , f_type , exf_type);
	}
	else
	{
		res_maker = procedure_file(real_root_path , out_root_path , dec_mode);
	}

	if(res_maker)
	{
		cout<<"ASmaker build success!"<<endl;
	}
	else
	{
		cout<<"ASmaker build failed"<<endl;
	}

	return 0;
}

//处理输入文件夹
bool procedure_dir(bf::path real_root_path , bf::path out_root_path , string & delima , int dec_mode , set<string> & f_type , set<string> & exf_type)
{
	ifstream i_file;
	ofstream o_file;
	Cryptor crptr;

	if(!dec_mode)
	{
		crptr.set_flag(dec_mode);
	}

	dirqueue my_dir_queue;
	my_dir_queue.push(real_root_path);

	bf::directory_iterator end_iter;
	while(!my_dir_queue.empty())
	{
		//itr & create dir
		bf::path op = out_root_path;
		bf::path p=my_dir_queue.front();
		my_dir_queue.pop();
		string sub_path_s = Get_Sub_Path_s(p,delima);
		if(!sub_path_s.empty())
		{
			op /= sub_path_s.c_str();
			if(!bf::exists(op))
			{
				bf::create_directory(op);
				cout<<"create target directory "<<op.string()<<endl;
			}
		}
		for (bf::directory_iterator it( p ) ; it != end_iter; ++it)
		{
			if(bf::is_directory(*it))
			{
				if(".svn"==it->path().filename())
					continue;
				//push directory
				my_dir_queue.push(*it);
			}
			else//none directory, files
			{
				//got extension
				if(it->path().has_extension())
				{
					string ext_key = it->path().extension().string();
					//include , enc
					set<string>::const_iterator inc_itor = f_type.find(ext_key);
					set<string>::const_iterator exc_itor = exf_type.find(ext_key);
					if(inc_itor != f_type.end())
					{
						i_file.open(it->path().string().c_str(),ios::binary);
						bf::path o_real_p = op;
						o_real_p /= it->path().filename().string().c_str();
						o_file.open(o_real_p.string(),ios::binary);

						unsigned long buf_size = 0;
						i_file.seekg(0,std::ios_base::end);
						buf_size = i_file.tellg();
						i_file.seekg(0,std::ios_base::beg);
						char * buf = new char[buf_size];
						i_file.read(buf,buf_size);
						crptr.Enc(buf,buf_size,buf);
						o_file.write(buf,buf_size);

						i_file.close();
						o_file.close();
						delete [] buf;
					}
					//exclude , do nothing
					else if (exc_itor != exf_type.end())
					{
						continue;
					}
					//common files, just copy
					else
					{
						bf::path o_real_p = op;
						o_real_p /= it->path().filename().string().c_str();
						bf::copy_file(it->path(), o_real_p, bf::copy_option::overwrite_if_exists);
					}
				}
				//if no extension, just copy
				else
				{
					bf::path o_real_p = op;
					o_real_p /= it->path().filename().string().c_str();
					bf::copy_file(it->path(), o_real_p, bf::copy_option::overwrite_if_exists);
				}
			}
		}
	}	
	return true;
}


//处理单个输入的文件
bool procedure_file(const bf::path & real_root_path , const bf::path & out_root_path ,  int dec_mode)
{
	ifstream i_file;
	ofstream o_file;
	Cryptor crptr;

	if(!dec_mode)
	{
		crptr.set_flag(dec_mode);
	}

	i_file.open(real_root_path.string(),ios::binary);
	o_file.open(out_root_path.string(),ios::binary);
	if(!o_file)
	{
		cout<<"open directory "<<out_root_path.string()<<" failed, plz check target output path"<<endl;
		return false;
	}
	
	unsigned long buf_size = 0;
	i_file.seekg(0,std::ios_base::end);
	buf_size = i_file.tellg();
	i_file.seekg(0,std::ios_base::beg);
	char * buf = new char[buf_size];
	i_file.read(buf,buf_size);
	crptr.Enc(buf,buf_size,buf);
	o_file.write(buf,buf_size);

	i_file.close();
	o_file.close();
	delete [] buf;

	return true;
}
