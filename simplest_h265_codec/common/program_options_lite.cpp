

#include <string>

#include "program_options_lite.h"

using namespace std;//为了使用std中的string，chendekai


namespace df
{
	namespace program_options_lite
	{
		Options::~Options()
		{
			for (Options::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); it++)
			{
				delete *it;//变量名为it的迭代器，迭代器iterator相当于指针，删除初始化Options对象时，（Name *）所占空间，chendekai
			}
		}

		void Options::addOption(OptionBase *opt)
		{
			Names* names = new Names();
			names->opt = opt;
			string& opt_string = opt->opt_string;

			size_t opt_start = 0;
			for (size_t opt_end = 0; opt_end != string::npos;)
			{
				opt_end = opt_string.find_first_of(',', opt_start);
				bool force_short = 0;
				if (opt_string[opt_start] == '-')
				{
					opt_start++;
					force_short = 1;
				}
				string opt_name = opt_string.substr(opt_start, opt_end - opt_start);
				if (force_short || opt_name.size() == 1)
				{
					names->opt_short.push_back(opt_name);
					opt_short_map[opt_name].push_back(names);
				}
				else
				{
					names->opt_long.push_back(opt_name);
					opt_long_map[opt_name].push_back(names);//下标方式访问map容器元素，chendekai
				}
				opt_start += opt_end + 1;
			}
			opt_list.push_back(names);
		}


		/* Helper method to initiate adding options to Options */
		OptionSpecific Options::addOptions()
		{
			return OptionSpecific(*this);//this指向本类对象，这里，this指向Options类对象，
			                                //OptionSpecific(*this)是调用构造函数（建一个匿名OptionSpecific对象），chendekai
		}

	}
}

