#include <iostream>
#include <sstream>//std::istringstream
#include <string>//std::string
#include <list>//std::list
#include <map>//std::map


#ifndef _PROGRAM_OPTIONS_LITE_H_
#define _PROGRAM_OPTIONS_LITE_H_


namespace df
{
	namespace program_options_lite// Nested namespace.
	{
		struct Options;// Like a class, declare a struct structure, declared forward, declared only, undefined.


		struct ParseFailure : public std::exception  //struct is mixed with class inheritance
		{
			ParseFailure(std::string arg0, std::string val0) throw() //throw(): indicate that no exception of 
																	 // any type is thrown.
			: m_szArg(arg0), m_szVal(val0)
			{}

			~ParseFailure() throw() {};

			std::string m_szArg;
			std::string m_szVal;

		};


		struct ErrorReporter
		{
			ErrorReporter() : m_bIsErrored(0) {};// When explicitly declaring a default constructor with
											   // no parameters, the definition of the default constructor or 
											   // the initialization table should be given.Otherwise, only
											   // declaring the default constructor without definition will 
											   // easily cause constructor overload and amiguity when calling


			virtual ~ErrorReporter() {}
			virtual std::ostream& error(const std::string& where);
			virtual std::ostream& warn(const std::string& where);

			bool m_bIsErrored;

		};


		extern ErrorReporter defaultErrorReporter;
		

		void Do_help(std::ostream& out, Options& opts, unsigned columns = 80);
		std::list<const char*> Scan_argv(Options& opts, unsigned argc, const char* argv[], ErrorReporter& errorReporter = defaultErrorReporter);
		void Set_defaults(Options& opts);
		void Parse_config_file(Options& opts, const std::string& filename, ErrorReporter& errorReporter = defaultErrorReporter);


		// OptionBase: Virtual base class for storing information 
		// relating to a specific option. This base class describes common elements.  Type specific
		// information should be stored in a derived class.
		struct OptionBase
		{

			OptionBase(const std::string& name, const std::string& desc)
			: m_OptString(name), m_OptDesc(desc)
			{}

			virtual ~OptionBase() {}
		
			// Parse argument arg, to obtain a value for the option.
			virtual void Parse(const std::string& arg, ErrorReporter&) = 0;

			// Set the argument to the default value.
			virtual void Set_default() = 0;// Declare a pure virtual function in a base class 
										   // so that derived classes can define it as needed.
			

			std::string m_OptString;
			std::string m_OptDesc;

		};


		// Type specific option storage. 
		template<typename T>
		struct Option : public OptionBase
		{
			Option(const std::string& name, T& storage, T defaultVal, const std::string& desc)
			: OptionBase(name, desc), m_OptStorage(storage), m_OptDefaultVal(defaultVal)
			{}

			void Parse(const std::string& arg, ErrorReporter&);

			void Set_default()
			{
				m_OptStorage = m_OptDefaultVal;// "=" operator overloading.
			}
		
	
			T &m_OptStorage;
			T m_OptDefaultVal;
		};

		
		// Generic parsing
		// CdkNote: learning "inline void" the way to write.
		// Inline void Parse() can call static function in AppEncCfg.cpp, this is a
		// point of knowledge.
		template<typename T>
		inline void
		Option<T>::Parse(const std::string& arg, ErrorReporter&)
		{
			std::istringstream argSS(arg, std::istringstream::in);
			argSS.exceptions(std::ios::failbit);
			try
			{
				argSS >> m_OptStorage;// ">>" operator overloading.
			}
			catch (...)
			{
				throw ParseFailure(m_OptString, arg);
			}
		}


		// String parsing is specialized -- copy the whole string, not just the
		// first word.  
		// This is actually a normal function, overridden with the generic Parse().
		// The template <> does not represent a template function, and can be removed.
		inline void
		Option<std::string>::Parse(const std::string& arg, ErrorReporter&)
		{
			m_OptStorage = arg;
		}
		

		class OptionSpecific;
		struct Options
		{
			struct Names
			{
				Names() : m_Opt(0) {};// Constructor of a class declared by struct.
				~Names()
				{
					if (m_Opt)
					{
						delete m_Opt;
					}
				}
				std::list<std::string> m_OptLong;
				std::list<std::string> m_OptShort;
				OptionBase* m_Opt;// The OptionBase class was defined earlier.
			};

			void Add_option(OptionBase* opt);
			OptionSpecific Add_options();


			typedef std::list<Names*> NamesPtrList;
			NamesPtrList m_OptList;
			typedef std::map<std::string, NamesPtrList> NamesMap;//#include <map>,STL header files do not have .h extension
			NamesMap m_OptLongMap;
			NamesMap m_OptShortMap;


		};


		// Option class for argument handling using a user provided function.
		struct OptionFunc : public OptionBase
		{
			typedef void (Func)(Options&, const std::string&, ErrorReporter&);
			OptionFunc(const std::string& name, Options& tmpParent, Func* tmpFunc, const std::string& desc)
			: OptionBase(name, desc), m_Parent(tmpParent), m_pFunc(tmpFunc)
			{}

			void Parse(const std::string& arg, ErrorReporter& errorReporter)
			{
				m_pFunc(m_Parent, arg, errorReporter);
			}


			// Pure virtual function should be defined in derived classes.
			void Set_default()
			{
				return;
			}

		private:
			Options& m_Parent;
			Func* m_pFunc;

		};


		class OptionSpecific
		{
		public:
			OptionSpecific(Options& tmpParent) : parent(tmpParent) {}


			// function call operator() is overloaded 
			// Add option described by name to the parent Options list,
			//   with storage for the option's value
			//   with defaultVal as the default value
			//   with desc as an optional help description
			template<typename T>
			OptionSpecific&
			operator()(const std::string& name, T& storage, T defaultVal, const std::string& desc = "")
			{
				parent.Add_option(new Option<T>(name, storage, defaultVal, desc));//The address of a derived object is assigned to a pointer variable to
																				  // a base object that can access only the base members of the derived class.
				return *this;
			}

			// Add option described by name to the parent Options list,
			// with desc as an optional help description.
			// Instead of storing the value somewhere, a function of type
			// OptionFunc::Func is called.
			// It is upto this function to correctly handle evaluating the option's value.

			OptionSpecific&
			operator()(const std::string& name, OptionFunc::Func* func, const std::string& desc = "")
			{
				parent.Add_option(new OptionFunc(name, parent, func, desc));
				return *this;
			}

		private:
			Options& parent;

		};

		
	}// namespace: program_options_lite

}// namespace: df



#endif// _PROGRAM_OPTIONS_LITE_H_


