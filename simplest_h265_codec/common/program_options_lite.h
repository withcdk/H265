
#include <string>//std::string��chendekai
#include <list>//std::list
#include <map>//std::map



#ifndef __PROGRAM_OPTIONS_LITE__
#define __PROGRAM_OPTIONS_LITE__

namespace df
{
	namespace program_options_lite//Ƕ�׵������ռ䣬chendekai
	{
		struct Options;//����һ��������һ��struct�ṹ��ǰ��������ֻ������δ���壬chendekai

		/** OptionBase: Virtual base class��Virtual base class������ࣩ for storing information 
		* relating to a specific option. This base class describes common elements.  Type specific
		* information should be stored in a derived class��derived class�������ࣩ.            */
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
			* addOptions()�ķ���ֵΪ������OptionSpecific���()���캯��,
			* opts.addOptions()("help",do_help,false,"this help text")
		    *                  ("BitstreamFile,b", m_bitstreamFileName, string(""), "bitstream input file name");
			* ������һ����������û�ж����¶���������
			* author:chendekai
			*/
			OptionSpecific addOptions();


			struct Names
			{
				Names() : opt(0) {};//struct�������࣬�Ĺ��캯����chendekai
				~Names()
				{
					if (opt)
					{
						delete opt;
					}
				}
				std::list<std::string> opt_long;
				std::list<std::string> opt_short;
				OptionBase* opt;//OptionBase������ǰ�涨�壬chendekai
			};


			void addOption(OptionBase *opt);
			typedef std::list<Names*> NamesPtrList;
			NamesPtrList opt_list;
			typedef std::map<std::string, NamesPtrList> NamesMap;//#include <map>,STLͷ�ļ�û����չ��.h
			NamesMap opt_long_map;
			NamesMap opt_short_map;

		};



		
		/* Class with templated overloaded operator(), for use by Options::addOptions() */
		class OptionSpecific
		{
		public:
			OptionSpecific(Options& parent_) : parent(parent_) {}


			//�������������()���أ�chendekai
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
				parent.addOption(new Option<T>(name, storage, default_val, desc));//���������ĵ�ַ����ָ���������ָ�����,chendekai
				return *this;
			}

		private:
			Options& parent;

		};




	}
}


#endif//__PROGRAM_OPTIONS_LITE__
