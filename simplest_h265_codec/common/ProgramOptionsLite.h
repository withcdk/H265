
#include <string>//std::string
#include <list>//std::list
#include <map>//std::map
#include <sstream>//std::istringstream
#include <iostream>


#ifndef __PROGRAM_OPTIONS_LITE__
#define __PROGRAM_OPTIONS_LITE__

namespace df
{
	namespace program_options_lite// nested namespace
	{
		struct Options;//like a class, declare a struct structure, declared forward, declared only, undefined

		struct ParseFailure : public std::exception  //struct is mixed with class inheritance
		{
			ParseFailure(std::string arg0, std::string val0) throw() //throw(): indicate that no exception of 
				                                                      // any type is thrown
			: arg(arg0), val(val0)
			{}

			~ParseFailure() throw() {};

			std::string arg;
			std::string val;

			const char* what() const throw() { return "Option Parse Failure"; }// useless temporarily
		};


		struct ErrorReporter
		{
			ErrorReporter() : is_errored(0) {};// When explicitly declaring a default constructor with
			                                    // no parameters, the definition of the default constructor or 
			                                    // the initialization table should be given.Otherwise, only
			                                    // declaring the default constructor without definition will 
			                                    // easily cause constructor overload and amiguity when calling
			
			
			virtual ~ErrorReporter() {}
			virtual std::ostream& error(const std::string& where);

			bool is_errored;

		};

		extern ErrorReporter defaultErrorReporter;

		void Do_help(std::ostream& out, Options& opts, unsigned columns = 80);
		std::list<const char*> Scan_argv(Options& opts, unsigned argc, const char* argv[], ErrorReporter& m_ErrorReporter = defaultErrorReporter);
		void Set_defaults(Options& opts);




		// OptionBase: Virtual base class for storing information 
		// relating to a specific option. This base class describes common elements.  Type specific
		// information should be stored in a derived class.
		struct OptionBase
		{
			
			OptionBase(const std::string& name, const std::string& desc)
			: m_OptString(name), m_OptDesc(desc)
			{}

			virtual ~OptionBase() {}

			// parse argument arg, to obtain a value for the option 
			virtual void Parse(const std::string& arg, ErrorReporter&) = 0;
			// set the argument to the default value
			virtual void Set_default() = 0;// declare a pure virtual function in a base class 
			                              // so that derived classes can define it as needed

			std::string m_OptString;
			std::string m_OptDesc;


		};

		


		// Type specific option storage 
		template<typename T>
		struct Option : public OptionBase
		{
			Option(const std::string& name, T& storage, T defaultVal, const std::string& desc)
			: OptionBase(name, desc), m_OptStorage(storage), m_OptDefaultVal(defaultVal)
			{}
			

			void Parse(const std::string& arg, ErrorReporter&);


			void Set_default()
			{
				m_OptStorage = m_OptDefaultVal;
			}

			T& m_OptStorage;
			T m_OptDefaultVal;
		};


		// Generic parsing 
		template<typename T>
		inline void
		Option<T>::Parse(const std::string& arg, ErrorReporter&)
		{
			std::istringstream arg_ss(arg, std::istringstream::in);
			arg_ss.exceptions(std::ios::failbit);
			try
			{
				arg_ss >> m_OptStorage;
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
		template<>
		inline void
		Option<std::string>::Parse(const std::string& arg, ErrorReporter&)
		{
			m_OptStorage = arg;
		}
		
		

		class OptionSpecific;
		struct Options
		{
			~Options();



			// the return value of Add_options() is to call the OptionSpecific class function call operator() 
			// overloaded function
			OptionSpecific Add_options();


			struct Names
			{
				Names() : m_Opt(0) {};// constructor of a class declared by struct
				~Names()
				{
					if (m_Opt)
					{
						delete m_Opt;
					}
				}
				std::list<std::string> m_OptLong;
				std::list<std::string> m_OptShort;
				OptionBase* m_Opt;// the OptionBase class was defined earlier
			};


			void Add_option(OptionBase *m_Opt);
			typedef std::list<Names*> NamesPtrList;
			NamesPtrList m_OptList;
			typedef std::map<std::string, NamesPtrList> NamesMap;//#include <map>,STL header files do not have .h extension
			NamesMap m_OptLongMap;
			NamesMap m_OptShortMap;

		};



		
		// Class with templated overloaded operator(), for use by Options::Add_options() 
		class OptionSpecific
		{
		public:
			OptionSpecific(Options& parent_) : parent(parent_) {}


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

		private:
			Options& parent;

		};




	}// namespace: program_options_lite 
}// namespace: df 


#endif//__PROGRAM_OPTIONS_LITE__
