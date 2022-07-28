
#include <fstream>// ifstream
#include <algorithm>//max()

#include "AppProgramOptionsLite.h"

using namespace std;//to use string in std




namespace df
{
	namespace program_options_lite
	{
		ErrorReporter defaultErrorReporter;


		ostream& ErrorReporter::error(const string& where)
		{
			m_bIsErrored = 1;
			cerr << where << " error: ";
			return cerr;
		}
		ostream& ErrorReporter::warn(const string& where)
		{
			cerr << where << " warning: ";
			return cerr;
		}


		static const char spaces[41] = "                                        ";

		// format help text for a single option:
		// using the formatting: "-x, --long",
		// if a short/long option isn't specified, it is not printed
		static void Do_help_opt(ostream& out, const Options::Names& entry, unsigned padShort = 0)
		{
			padShort = min(padShort, 8u);//"u" stands for unsigned type

			if (!entry.m_OptShort.empty())
			{
				unsigned pad = max((int)padShort - (int)entry.m_OptShort.front().size(), 0);
				out << "-" << entry.m_OptShort.front();
				if (!entry.m_OptLong.empty())
				{
					out << ", ";
				}
				out << &(spaces[40 - pad]);// Each space after 40-pad is printed with pad characters, 
										   // 40 stands for 40 Spaces in the Spaces [] array. The number 40 can be set arbitrarily.
			}
			else
			{
				out << "   ";
				out << &(spaces[40 - padShort]);
			}

			if (!entry.m_OptLong.empty())
			{
				out << "--" << entry.m_OptLong.front();
			}
		}

		// format the help text 
		void Do_help(ostream& out, Options& opts, unsigned columns)
		{
			const unsigned padShort = 3;
			// first pass: work out the longest option name 
			unsigned maxWidth = 0;
			for (Options::NamesPtrList::iterator it = opts.m_OptList.begin(); it != opts.m_OptList.end(); it++)
			{
				ostringstream line(ios_base::out);
				Do_help_opt(line, **it, padShort);
				maxWidth = max(maxWidth, (unsigned)line.tellp());// calculate the length of the longest "sum of short option and 
																 // long option lengths" in the help message, without the first two Spaces
			}

			unsigned optWidth = min(maxWidth + 2, 28u + padShort) + 2;// 28 represents the option text experience(or preference) value
			unsigned descWidth = columns - optWidth;

			// second pass: write out formatted option and help text.
			//  - align start of help text to start at optWidth
			//  - if the option text is longer than optWidth, place the help
			//    text at optWidth on the next line.
			//
			for (Options::NamesPtrList::iterator it = opts.m_OptList.begin(); it != opts.m_OptList.end(); it++)
			{
				ostringstream line(ios_base::out);
				line << "  ";
				Do_help_opt(line, **it, padShort);

				const string& m_OptDesc = (*it)->m_Opt->m_OptDesc;
				if (m_OptDesc.empty())
				{
					// no help text: output option, skip further processing 
					cout << line.str() << endl;
					continue;
				}
				size_t currLength = size_t(line.tellp());
				if (currLength > optWidth)
				{
					// if option text is too long (and would collide with the
					// help text, split onto next line 
					line << endl;
					currLength = 0;
				}
				// split up the help text, taking into account new lines,
				//   (add optWidth of padding to each new line) 
				for (size_t newlinePos = 0, curPos = 0; curPos != string::npos; currLength = 0)
				{
					// print any required padding space for vertical alignment 
					line << &(spaces[40 - optWidth + currLength]);
					newlinePos = m_OptDesc.find_first_of('\n', newlinePos);
					if (newlinePos != string::npos)
					{
						// newline found, print substring (newline needn't be stripped) 
						newlinePos++;
						line << m_OptDesc.substr(curPos, newlinePos - curPos);// line variable, formatted, written to the stream
						curPos = newlinePos;
						continue;
					}
					if (curPos + descWidth > m_OptDesc.size())
					{
						// no need to wrap text, remainder is less than avaliable width 
						line << m_OptDesc.substr(curPos);
						break;
					}
					// find a suitable point to split text (avoid spliting in middle of word) 
					size_t splitPos = m_OptDesc.find_last_of(' ', curPos + descWidth);
					if (splitPos != string::npos)
					{
						// eat up multiple space characters
						splitPos = m_OptDesc.find_last_not_of(' ', splitPos) + 1;
					}

					// bad split if no suitable space to split at.  fall back to width 
					bool badSplit = splitPos == string::npos || splitPos <= curPos;
					if (badSplit)
					{
						splitPos = curPos + descWidth;
					}
					line << m_OptDesc.substr(curPos, splitPos - curPos);

					// eat up any space for the start of the next line 
					if (!badSplit)
					{
						splitPos = m_OptDesc.find_first_not_of(' ', splitPos);
					}
					curPos = newlinePos = splitPos;

					if (curPos >= m_OptDesc.size())
					{
						break;
					}
					line << endl;// "endl" is also formatted and written to the stream
				}

				cout << line.str() << endl;
			}
		}


		struct OptionWriter
		{
			OptionWriter(Options& opts, ErrorReporter& err)
			: m_Opts(opts), m_ErrorReporter(err)
			{}

			virtual const string Where() = 0;

			bool Store_pair(bool allowLong, bool allowShort, const string& name, const string& value);
			bool Store_pair(const string& name, const string& value)
			{
				return Store_pair(true, true, name, value);
			}


			Options& m_Opts;
			ErrorReporter& m_ErrorReporter;
		};

		struct ArgvParser : public OptionWriter
		{
			ArgvParser(Options& opts, ErrorReporter& errorReporter)
			: OptionWriter(opts, errorReporter)
			{}

			const string Where() { return "command line"; }//where() returns const

			void Parse_GNU(unsigned argc, const char* argv[]);
			unsigned Parse_SHORT(unsigned argc, const char* argv[]);
		};

		static void Set_options(Options::NamesPtrList& m_OptList, const string& value, ErrorReporter& errorReporter)
		{
			// multiple options may be registered for the same name:
			//   allow each to parse value 
			for (Options::NamesPtrList::iterator it = m_OptList.begin(); it != m_OptList.end(); ++it)
			{
				(*it)->m_Opt->Parse(value, errorReporter);
			}
		}

		/**************************************************************************************************
		Description: Store the value in option. e.g encoder_intra_main.cfg assigned to m_szInputFileName.
		Input para:
		Return value:
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		*****************************************************************************************************/
		bool OptionWriter::Store_pair(bool allowLong, bool allowShort, const string& name, const string& value)
		{
			bool found = false;
			Options::NamesMap::iterator optIt;
			if (allowLong)
			{
				optIt = m_Opts.m_OptLongMap.find(name);
				if (optIt != m_Opts.m_OptLongMap.end())
				{
					found = true;
				}
			}

			// check for the short list 
			if (allowShort && !(found && allowLong))
			{
				optIt = m_Opts.m_OptShortMap.find(name);
				if (optIt != m_Opts.m_OptShortMap.end())
				{
					found = true;
				}
			}

			if (!found)
			{
				m_ErrorReporter.error(Where())
					<< "Unknown option '" << name << "' (value:'" << value << "')\n";
				return false;
			}

			Set_options((*optIt).second, value, m_ErrorReporter);
			return true;
		}

		// returns number of extra arguments consumed
		void ArgvParser::Parse_GNU(unsigned argc, const char* argv[])
		{
			// gnu style long options can take the forms:
			//  --option=arg£¨right format£º--BitstreamFile=str.bin£©
			//  --option arg£¨--b=str.bin£¨format wrong£©£¬or --b str.bin and --BitstreamFile str.bin£¬all are incorrect format£©
			//  --option£¨right format£¬e.g --help£©
			string arg(argv[0]);
			size_t argOptStart = arg.find_first_not_of('-');
			size_t argOptSep = arg.find_first_of('=');
			string option = arg.substr(argOptStart, argOptSep - argOptStart);

			if (argOptSep == string::npos)
			{
				// no argument found => argument in argv[1] (maybe) 
				// xxx, need to handle case where option isn't required

				if (!Store_pair(true, false, option, "1"))
				{
					return;
				}
			}
			else
			{
				// argument occurs after option_sep 
				string val = arg.substr(argOptSep + 1);
				Store_pair(true, false, option, val);
			}

		}

		unsigned ArgvParser::Parse_SHORT(unsigned argc, const char* argv[])
		{

			// short options can take the forms:
			//  -option arg£¨-b   -o£©

			string arg(argv[0]);
			size_t argOptStart = arg.find_first_not_of('-');
			string option = arg.substr(argOptStart);

			// lookup option
			// argument in argv[1] 
			// xxx, need to handle case where option isn't required 
			if (argc == 1)
			{
				m_ErrorReporter.error(Where())// where () is a public member function that can be used outside the structure
					<< "Not processing option '" << option << "' without argument\n";
				return 0; // run out of argv for argument 
			}
			Store_pair(false, true, option, string(argv[1]));

			return 1;
		
		}

		/**************************************************************************************
		Description: Scan argvs in command line parameter. e.g -c encoder_intra_main.cfg
		Input para:		
		Return value: 
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		***********************************************************************************/
		list<const char*>
		Scan_argv(Options& opts, unsigned argc, const char* argv[], ErrorReporter& errorReporter)
		{
			ArgvParser avp(opts, errorReporter);

			// A list for anything that didn't get handled as an option.
			list<const char*> nonOptionArguments;

			for (unsigned i = 1; i < argc; i++)
			{
				if (argv[i][0] != '-')
				{
					nonOptionArguments.push_back(argv[i]);
					continue;
				}

				if (argv[i][1] == 0)
				{
					// a lone single dash is an argument (usually signifying stdin) 
					nonOptionArguments.push_back(argv[i]);
					continue;
				}


				
				if (argv[i][1] != '-')
				{
					// Handle short (single dash) options. 
					// Handle command line arguments for single dashes, e.g -b, -o and so on.
					i += avp.Parse_SHORT(argc - i, &argv[i]);
					continue;
				}

				
				if (argv[i][2] == 0)
				{
					// a lone double dash ends option processing 
					//-- b str.bin, this command argument all identified as non option
					while (++i < argc)
					{
						nonOptionArguments.push_back(argv[i]);
					}
					break;
				}

				
				// handle long (double dash) options 
				avp.Parse_GNU(argc - i, &argv[i]);// handle command arguments in Linux format
				
			}

			return nonOptionArguments;
		}

		
		/******************************************************************************
		Description: For all options in opts, set their storage to their specified
		             default value.
		Input para:
		Return value:
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		******************************************************************************/
		void Set_defaults(Options& opts)
		{
			// multiple parameters are processed together
			for (Options::NamesPtrList::iterator it = opts.m_OptList.begin(); it != opts.m_OptList.end(); it++)
			{
				(*it)->m_Opt->Set_default();
			}
		}


		struct CfgStreamParser : public OptionWriter
		{
			CfgStreamParser(const string& name, Options& opts, ErrorReporter& errorReporter)
			: OptionWriter(opts, errorReporter)
			, m_szName(name)
			, m_iLineNum(0)
			{}

			const string Where()
			{
				ostringstream os;
				os << m_szName << ":" << m_iLineNum;
				return os.str();
			}

			void Scan_line(string& line);
			void Scan_stream(istream& in);

			const string m_szName;
			int m_iLineNum;

		};

		void CfgStreamParser::Scan_line(string& line)
		{
			// Strip any leading whitespace.
			size_t start = line.find_first_not_of(" \t\n\r");
			if (start == string::npos)
			{
				// Blank line.
				return;
			}
			if (line[start] == '#')
			{
				// Comment line.
				return;
			}

			// Look for first whitespace or ':' after the option end.
			size_t optionEnd = line.find_first_of(": \t\n\r", start);
			string option = line.substr(start, optionEnd - start);

			// Look for ':', eat up any whitespace first.
			start = line.find_first_not_of(" \t\n\r", optionEnd);
			if (start == string::npos)
			{
				// Error: badly formatted line.
				m_ErrorReporter.warn(Where()) << "line formatting error\n";
				return;
			}


			if (line[start] != ':')
			{
				// Error: badly formatted line.
				m_ErrorReporter.warn(Where()) << "line formatting error\n";
				return;
			}

			// Look for start of value string -- eat up any leading whitespace.
			start = line.find_first_not_of(" \t\n\r", ++start);
			if (start == string::npos)
			{
				// Error: badly formatted line.
				m_ErrorReporter.warn(Where()) << "line formatting error\n";
				return;
			}

			// Extract the value part, which may contain embedded spaces by
			// searching for a word at a time, until we hit a comment or end of line.
			size_t valueEnd = start;
			do
			{
				if (line[valueEnd] == '#')
				{
					// Rest of line is a comment. 
					valueEnd--;
					break;
				}

				valueEnd = line.find_first_of(" \t\n\r", valueEnd);

				// Consume any white space, incase there is another word.
				// Any trailing whitespace will be removed shortly.
				valueEnd = line.find_first_not_of(" \t\n\r", valueEnd);


			} while (valueEnd != string::npos);

			// Strip any trailing space from value.
			valueEnd = line.find_last_not_of(" \t\n\r", valueEnd);

			string value;
			if (valueEnd >= start)
			{
				value = line.substr(start, valueEnd + 1 - start);
			}
			else
			{
				// Error: no value.
				m_ErrorReporter.warn(Where()) << "no value found\n";
			}

			Store_pair(true, false, option, value);

		}

		/******************************************************************************
		Description: scan every line in the configuration file
		Input para:
		Return value:
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		******************************************************************************/
		void CfgStreamParser::Scan_stream(istream& in)
		{
			do
			{
				m_iLineNum++;
				string line;
				getline(in, line);
				Scan_line(line);
			} while (!!in);
		}

		/******************************************************************************
		Description: Parse configuration file.
		Input para:
		Return value:
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		******************************************************************************/
		void Parse_config_file(Options& opts, const std::string& filename, ErrorReporter& errorReporter)
		{
			ifstream cfgStream(filename.c_str(), ifstream::in);
			if (!cfgStream)
			{
				errorReporter.error(filename) << "Failed to open config file\n";
				return;
			}
			CfgStreamParser csp(filename, opts, errorReporter);
			csp.Scan_stream(cfgStream);

		}

	
		void Options::Add_option(OptionBase* opt)
		{
			Names* names = new Names();
			names->m_Opt = opt;
			string& m_OptString = opt->m_OptString;

			size_t optStart = 0;
			for (size_t optEnd = 0; optEnd != string::npos;)
			{
				optEnd = m_OptString.find_first_of(',', optStart);
				bool forceShort = 0;
				if (m_OptString[optStart] == '-')
				{
					optStart++;
					forceShort = 1;
				}
				string optName = m_OptString.substr(optStart, optEnd - optStart);
				if (forceShort || optName.size() == 1)
				{
					names->m_OptShort.push_back(optName);
					m_OptShortMap[optName].push_back(names);
				}
				else
				{
					names->m_OptLong.push_back(optName);
					m_OptLongMap[optName].push_back(names);// access map container elements by subscript
				}
				optStart += optEnd + 1;
			}
			m_OptList.push_back(names);
		}
	 
		/******************************************************************************
		Description: Helper method to initiate adding options to Options.
		Input para:
		Return value:
		Author£ºDe-Kai Chen    <cdk5@foxmail.com>
		******************************************************************************/
		OptionSpecific Options::Add_options()
		{
			return OptionSpecific(*this); // The constructor is called explicitly.
										  //"this" refers to the class object, in this case, "this" refers to the Options class object,
										  // OptionSpecific(*this) calls the constructor(creates an anonymous OptionSpecific object)
										  // e.g OptionSpecific(*this)("help",                         doHelp,                        false,              "this help text")
										  //                          ("BitstreamFile,b",              m_szBitstreamFileName,            string(""),         "bitstream input file name")
										  //                          ("ReconFile,o",                  m_szReconFileName,                string(""),         "reconstructed YUV output file name\n"
										  //   
		}


		
	}// namespace: program_options_lite



}// namespace: df



