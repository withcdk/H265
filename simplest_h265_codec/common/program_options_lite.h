
#include <string>//std::string，chendekai
#include <list>//std::list
#include <map>//std:map



#ifndef __PROGRAM_OPTIONS_LITE__
#define __PROGRAM_OPTIONS_LITE__

namespace df
{
	namespace program_options_lite//嵌套的命名空间，chendekai
	{
		struct Options;//像类一样，声明一个struct结构，前向声明，只声明，未定义，chendekai

		/** OptionBase: Virtual base class for storing information relating to a
		* specific option This base class describes common elements.  Type specific
		* information should be stored in a derived class. */
		struct OptionBase
		{
			OptionBase(const std::string& name, const std::string& desc)
				: opt_string(name), opt_desc(desc)
			{};

			virtual ~OptionBase() {}


			std::string opt_string;
			std::string opt_desc;


		};


		/** Type specific option storage */
		template<typename T>
		struct Option : public OptionBase
		{
			Option(const std::string& name, T& storage, T default_val, const std::string& desc)
				: OptionBase(name, desc), opt_storage(storage), opt_default_val(default_val)
			{}


			T& opt_storage;
			T opt_default_val;
		};


		class OptionSpecific;
		struct Options
		{
			~Options();



			/**
			* addOptions()的返回值为：调用OptionSpecific类的()构造函数,
			* opts.addOptions()("help",do_help,false,"this help text")
		    *                  ("BitstreamFile,b", m_bitstreamFileName, string(""), "bitstream input file name");
			* 即建立一个匿名对象（没有定义新对象）来调用
			* author:chendekai
			*/
			OptionSpecific addOptions();

			struct Names
			{
				Names() : opt(0) {};
				~Names()
				{
					if (opt)
					{
						delete opt;
					}
				}
				std::list<std::string> opt_long;
				std::list<std::string> opt_short;
				OptionBase* opt;
			};


			void addOption(OptionBase *opt);
			typedef std::list<Names*> NamesPtrList;
			NamesPtrList opt_list;
			typedef std::map<std::string, NamesPtrList> NamesMap;
			NamesMap opt_long_map;
			NamesMap opt_short_map;

		};



		
		/* Class with templated overloaded operator(), for use by Options::addOptions() */
		class OptionSpecific
		{
		public:
			OptionSpecific(Options& parent_) : parent(parent_) {}


			//函数调用运算符()重载，chendekai
			template<typename T>
			OptionSpecific&
			operator()(const std::string& name, T& storage, T default_val, const std::string& desc = "")
			{
				parent.addOption(new Option<T>(name, storage, default_val, desc));
				return *this;
			}

		private:
			Options& parent;

		};




	}
}


#endif//__PROGRAM_OPTIONS_LITE__
