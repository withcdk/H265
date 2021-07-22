
#include <string>//std::string，chendekai
#include <list>//std::list
#include <map>//std::map



#ifndef __PROGRAM_OPTIONS_LITE__
#define __PROGRAM_OPTIONS_LITE__

namespace df
{
	namespace program_options_lite//嵌套的命名空间，chendekai
	{
		struct Options;//像类一样，声明一个struct结构，前向声明，只声明，未定义，chendekai


		void setDefaults(Options& opts);


		struct ErrorReporter
		{
			ErrorReporter() : is_errored(0) {};//显式声明一个无参的默认构造函数时，要给出该默认构造函数的定义或给出初始化表，
			                                     //否则，只声明而不定义，容易造成构造函数重载，调用时的歧义性，chendekai
			virtual ~ErrorReporter() {}
			virtual std::ostream& error(const std::string& where);

			bool is_errored;

		};

		extern ErrorReporter default_error_reporter;
		std::list<const char*> scanArgv(Options& opts, unsigned argc, const char* argv[], ErrorReporter& error_reporter = default_error_reporter);





		/** OptionBase: Virtual base class（Virtual base class：虚基类） for storing information 
		* relating to a specific option. This base class describes common elements.  Type specific
		* information should be stored in a derived class（derived class：派生类）.            */
		struct OptionBase
		{
			OptionBase(const std::string& name, const std::string& desc)
				: opt_string(name), opt_desc(desc)
			{};

			virtual ~OptionBase() {}

			/* parse argument arg, to obtain a value for the option */
			//virtual void parse(const std::string& arg, ErrorReporter&) = 0;
			/* set the argument to the default value */
			virtual void setDefault() = 0;//在基类中声明纯虚函数，以便派生类根据需要对它进行定义，chendekai

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

			//void parse(const std::string& arg, ErrorReporter&);


			void setDefault()
			{
				opt_storage = opt_default_val;
			}

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
				Names() : opt(0) {};//struct声明的类，的构造函数，chendekai
				~Names()
				{
					if (opt)
					{
						delete opt;
					}
				}
				std::list<std::string> opt_long;
				std::list<std::string> opt_short;
				OptionBase* opt;//OptionBase类已在前面定义，chendekai
			};


			void addOption(OptionBase *opt);
			typedef std::list<Names*> NamesPtrList;
			NamesPtrList opt_list;
			typedef std::map<std::string, NamesPtrList> NamesMap;//#include <map>,STL头文件没有扩展名.h
			NamesMap opt_long_map;
			NamesMap opt_short_map;

		};



		
		/* Class with templated overloaded operator(), for use by Options::addOptions() */
		class OptionSpecific
		{
		public:
			OptionSpecific(Options& parent_) : parent(parent_) {}


			//函数调用运算符()重载，chendekai
			/**
			* Add option described by name to the parent Options list,
			*   with storage for the option's value
			*   with default_val as the default value
			*   with desc as an optional help description
			*/
			template<typename T>
			OptionSpecific&
			operator()(const std::string& name, T& storage, T default_val, const std::string& desc = "")
			{
				parent.addOption(new Option<T>(name, storage, default_val, desc));//派生类对象的地址赋给指向基类对象的指针变量,
				                                                                    //该指针只能访问派生类中的基类成员，chendekai
				return *this;
			}

		private:
			Options& parent;

		};




	}/* namespace: program_options_lite */
}/* namespace: df */


#endif//__PROGRAM_OPTIONS_LITE__
