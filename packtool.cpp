#include "packfile.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
LPackFile file;//("a.pac");
const struct option long_options[] = {
	{ "packfile",      1, NULL, 'p' },
	{ "unpack",      1, NULL, 'u' },
	{ "compress",      0, NULL, 'c' },
	{ "version", 0, NULL, 'v' },
	{ NULL,          0, NULL, 0}
};
const char* const short_options = "p:vVu:c";
const  char *VERSION="Version 0.1 BUILD: "__DATE__;
std::string strPackFile="";
std::string strUnpackPath="";
bool bUnpack=false;
bool bCompress=false;

void ParseOption(int argc, char* argv[])
{
	int next_option;//下一个要处理的参数符号
	do
	{
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		switch (next_option)
		{
			case 'p':     /* pack file */    
				/* 此时optarg指向 -c后的filename */
				strPackFile=optarg;
				//SetConfigIniFile(optarg);
				break;
			case 'v':     /* version */
				printf("packtool  %s\r\n", VERSION);
				break;
			case 'V':     /* version */
				printf("packtool  %s\r\n", VERSION);
				exit(0);
				break;
			case 'c':
				bCompress=true;
				break;
			case 'u':
				strUnpackPath=optarg;
				bUnpack=true;
				break;
			case ':':     /* 缺乏长选项内容 */
				break;
			case '?':     /* 出现一个未指定的参数*/
				break;
			case -1:      /* 处理完毕后返回-1 */
			default:      /* 未指定的参数出现，出错处理 */
				break;
		}
	}while (next_option !=-1);
}

int main(int argc, char* argv[])
{
	ParseOption(argc, argv);
	if(strPackFile=="")
		return 0;
	file.OpenFile(strPackFile.c_str());
	if(bCompress)
	{
		file.CompactFile();
	}else
	if(bUnpack)
	{
	}else
	while (optind < argc)
	{
		//non-option ARGV arguments = file list
		printf("Appending file = %s\n",argv[optind]);
		file.AppendSubFile(argv[optind],argv[optind]);
		optind++;
	}
	//file.DeleteSubFile("Makefile");
	return 0;
}
