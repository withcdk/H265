

#include <string>
#include <iostream>
#include <algorithm>//max()��chendekai

#include "program_options_lite.h"

using namespace std;//Ϊ��ʹ��std�е�string��chendekai


namespace df
{
	namespace program_options_lite
	{

		ErrorReporter default_error_reporter;

		ostream& ErrorReporter::error(const string& where)
		{
			is_errored = 1;
			cerr << where << " error: ";
			return cerr;
		}

		Options::~Options()
		{
			for (Options::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); it++)
			{
				delete *it;//������Ϊit�ĵ�������������iterator�൱��ָ�룬ɾ����ʼ��Options����ʱ����Name *����ռ�ռ䣬chendekai
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
					opt_long_map[opt_name].push_back(names);//�±귽ʽ����map����Ԫ�أ�chendekai
				}
				opt_start += opt_end + 1;
			}
			opt_list.push_back(names);
		}

		/* Helper method to initiate adding options to Options */
		OptionSpecific Options::addOptions()
		{
			return OptionSpecific(*this);//thisָ����������thisָ��Options�����
			                                //OptionSpecific(*this)�ǵ��ù��캯������һ������OptionSpecific���󣩣�chendekai
		}

		static void setOptions(Options::NamesPtrList& opt_list, const string& value, ErrorReporter& error_reporter)
		{
			/* multiple options may be registered for the same name:
			*   allow each to parse value */
			for (Options::NamesPtrList::iterator it = opt_list.begin(); it != opt_list.end(); ++it)
			{
				(*it)->opt->parse(value, error_reporter);
			}
		}

		static const char spaces[41] = "                                        ";

		/* format help text for a single option:
		* using the formatting: "-x, --long",
		* if a short/long option isn't specified, it is not printed
		*/
		static void doHelpOpt(ostream& out, const Options::Names& entry, unsigned pad_short = 0)
		{
			pad_short = min(pad_short, 8u);//u��ʾunsigned�ͣ�chendekai

			if (!entry.opt_short.empty())
			{
				unsigned pad = max((int)pad_short - (int)entry.opt_short.front().size(), 0);
				out << "-" << entry.opt_short.front();
				if (!entry.opt_long.empty())
				{
					out << ", ";
				}
				out << &(spaces[40 - pad]);//40-pad�����ÿһ���ո����pad���ո��ַ���40�Ǳ�ʾspaces[]������40���ո��ַ���
				                             //40������ֿ����������ã�chendekai
			}
			else
			{
				out << "   ";
				out << &(spaces[40 - pad_short]);
			}

			if (!entry.opt_long.empty())
			{
				out << "--" << entry.opt_long.front();
			}
		}

		/* format the help text */
		void doHelp(ostream& out, Options& opts, unsigned columns)
		{
			const unsigned pad_short = 3;
			/* first pass: work out the longest option name */
			unsigned max_width = 0;
			for (Options::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
			{
				ostringstream line(ios_base::out);
				doHelpOpt(line, **it, pad_short);
				max_width = max(max_width, (unsigned)line.tellp());//�����help��Ϣ�У���ġ���option�ͳ�option����֮�͡��ĳ��ȣ�
				                                                     //����ǰ��2���ո�chendekai
			}

			unsigned opt_width = min(max_width + 2, 28u + pad_short) + 2;//28��ʾoption text���飨��ƫ�ã�ֵ��chendekai
			unsigned desc_width = columns - opt_width;

			/* second pass: write out formatted option and help text.
			*  - align start of help text to start at opt_width
			*  - if the option text is longer than opt_width, place the help
			*    text at opt_width on the next line.
			*/
			for (Options::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
			{
				ostringstream line(ios_base::out);
				line << "  ";
				doHelpOpt(line, **it, pad_short);

				const string& opt_desc = (*it)->opt->opt_desc;
				if (opt_desc.empty())
				{
					/* no help text: output option, skip further processing */
					cout << line.str() << endl;
					continue;
				}
				size_t currlength = size_t(line.tellp());
				if (currlength > opt_width)
				{
					/* if option text is too long (and would collide with the
					* help text, split onto next line */
					line << endl;
					currlength = 0;
				}
				/* split up the help text, taking into account new lines,
				*   (add opt_width of padding to each new line) */
				for (size_t newline_pos = 0, cur_pos = 0; cur_pos != string::npos; currlength = 0)
				{
					/* print any required padding space for vertical alignment */
					line << &(spaces[40 - opt_width + currlength]);
					newline_pos = opt_desc.find_first_of('\n', newline_pos);
					if (newline_pos != string::npos)
					{
						/* newline found, print substring (newline needn't be stripped) */
						newline_pos++;
						line << opt_desc.substr(cur_pos, newline_pos - cur_pos);//line��������ʽ����д�����У�chendekai
						cur_pos = newline_pos;
						continue;
					}
					if (cur_pos + desc_width > opt_desc.size())
					{
						/* no need to wrap text������Ҫ�ı����У�chendekai��, remainder is less than avaliable width */
						line << opt_desc.substr(cur_pos);
						break;
					}
					/* find a suitable point to split text (avoid spliting in middle of word) */
					size_t split_pos = opt_desc.find_last_of(' ', cur_pos + desc_width);
					if (split_pos != string::npos)
					{
						/* eat up multiple space characters���Ե�����Ŀո��ַ���chendekai�� */
						split_pos = opt_desc.find_last_not_of(' ', split_pos) + 1;
					}

					/* bad split if no suitable space to split at.  fall back to width */
					bool bad_split = split_pos == string::npos || split_pos <= cur_pos;
					if (bad_split)
					{
						split_pos = cur_pos + desc_width;
					}
					line << opt_desc.substr(cur_pos, split_pos - cur_pos);

					/* eat up any space for the start of the next line */
					if (!bad_split)
					{
						split_pos = opt_desc.find_first_not_of(' ', split_pos);
					}
					cur_pos = newline_pos = split_pos;

					if (cur_pos >= opt_desc.size())
					{
						break;
					}
					line << endl;//endlҲ��ʽ����д�����У�chendekai
				}

				cout << line.str() << endl;
			}
		}


		struct OptionWriter
		{
			OptionWriter(Options& rOpts, ErrorReporter& err)
			: opts(rOpts), error_reporter(err)
			{}
			virtual ~OptionWriter() {}

			virtual const string where() = 0;//���麯����chendekai

			bool storePair(bool allow_long, bool allow_short, const string& name, const string& value);
			bool storePair(const string& name, const string& value)
			{
				return storePair(true, true, name, value);
			}

			Options& opts;
			ErrorReporter& error_reporter;
		};

		bool OptionWriter::storePair(bool allow_long, bool allow_short, const string& name, const string& value)
		{
			bool found = false;
			Options::NamesMap::iterator opt_it;
			if (allow_long)
			{
				opt_it = opts.opt_long_map.find(name);
				if (opt_it != opts.opt_long_map.end())
				{
					found = true;
				}
			}

			/* check for the short list */
			if (allow_short && !(found && allow_long))
			{
				opt_it = opts.opt_short_map.find(name);
				if (opt_it != opts.opt_short_map.end())
				{
					found = true;
				}
			}

			if (!found)
			{
				error_reporter.error(where())
					<< "Unknown option `" << name << "' (value:`" << value << "')\n";
				return false;
			}

			setOptions((*opt_it).second, value, error_reporter);
			return true;
		}
		
		//��.cpp�ļ��������ṹ�����ͣ�chendekai
		struct ArgvParser : public OptionWriter
		{
			ArgvParser(Options& rOpts, ErrorReporter& rError_reporter)
			: OptionWriter(rOpts, rError_reporter)
			{}

			const string where() { return "command line"; }//where()���ص�ֵΪconst��

			void parseGNU(unsigned argc, const char* argv[]);
			unsigned parseSHORT(unsigned argc, const char* argv[]);
		};



		/**
		* returns number of extra arguments consumed
		*/
		void ArgvParser::parseGNU(unsigned argc, const char* argv[])
		{
			/* gnu style long options can take the forms:
			*  --option=arg����ȷ��ʽ��--BitstreamFile=str.bin��chendekai��
			*  --option arg��--b=str.bin����ʽ���ԣ�������--b str.bin��--BitstreamFile str.bin������Ҫ�Ĳ���������Ϊ����ȷ��ʽ��chendekai��
			*  --option����ȷ��ʽ������--help,chendekai��
			*/
			string arg(argv[0]);
			size_t arg_opt_start = arg.find_first_not_of('-');
			size_t arg_opt_sep = arg.find_first_of('=');
			string option = arg.substr(arg_opt_start, arg_opt_sep - arg_opt_start);

			if (arg_opt_sep == string::npos)
			{
				/* no argument found => argument in argv[1] (maybe) */
				/* xxx, need to handle case where option isn't required��������Ҫ�Ĳ�����chendekai�� */

				if (!storePair(true, false, option, "1"))
				{
					return ;
				}
			}
			else
			{
				/* argument occurs after option_sep */
				string val = arg.substr(arg_opt_sep + 1);
				storePair(true, false, option, val);
			}

		}





		unsigned ArgvParser::parseSHORT(unsigned argc, const char* argv[])
		{
			/* short options can take the forms:
			*  --option arg��û�ã����ܴ�������ѡ�chendekai��
			*  -option arg��-b   -o�ȣ�chendekai��
			*/
			string arg(argv[0]);
			size_t arg_opt_start = arg.find_first_not_of('-');
			string option = arg.substr(arg_opt_start);
			/* lookup option */

			/* argument in argv[1] */
			/* xxx, need to handle case where option isn't required */
			if (argc == 1)
			{
				error_reporter.error(where())//where()����public��Ա�����������ڽṹ����ʹ�ã�chendekai
					<< "Not processing option `" << option << "' without argument\n";
				return 0; /* run out of argv for argument */
			}
			storePair(false, true, option, string(argv[1]));

			return 1;
		}





		list<const char*>
		scanArgv(Options& opts, unsigned argc, const char* argv[], ErrorReporter& error_reporter)
		{
			ArgvParser avp(opts, error_reporter);

			/* a list for anything that didn't get handled as an option�����ᱻ�����ѡ��list������chendekai�� */
			list<const char*> non_option_arguments;

			for (unsigned i = 1; i < argc; i++)
			{
				if (argv[i][0] != '-')
				{
					non_option_arguments.push_back(argv[i]);
					continue;
				}

				if (argv[i][1] == 0)
				{
					/* a lone single dash is an argument (usually signifying stdin) */
					non_option_arguments.push_back(argv[i]);
					continue;
				}

				if (argv[i][1] != '-')
				{
					/* handle short (single dash�������ۺţ�chendekai) options */
					/*   ���� �����ۺ�  �������в�������-b   -o   �ȣ�chendekai   */
					i += avp.parseSHORT(argc - i, &argv[i]);
					continue;
				}

				if (argv[i][2] == 0)
				{
					/* a lone double dash�����ۺţ�chendekai�� ends option processing */
					/****     -- b str.bin ���������в�����ȫ��ʶ��Ϊnon option ,chendekai */
					while (++i < argc)
					{
						non_option_arguments.push_back(argv[i]);
					}
					break;
				}

				/* handle long (double dash) options */
				avp.parseGNU(argc - i, &argv[i]);//����linuxϵͳ��ʽ�����������chendekai
			}

			return non_option_arguments;
		}



		/* for all options in opts, set their storage to their specified
		* default value��ע��д�ĺ�����ˣ�chendekai�� */
		void setDefaults(Options& opts)
		{
			for (Options::NamesPtrList::iterator it = opts.opt_list.begin(); it != opts.opt_list.end(); it++)
			{//�������һ����chendekai
				(*it)->opt->setDefault();
			}
		}

	}
}

