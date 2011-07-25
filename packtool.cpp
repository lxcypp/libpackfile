#include "packfile.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
LPackFile file;//("a.pac");
const struct option long_options[] = {
	{ "packfile",      1, NULL, 'p' },
	{ "version", 0, NULL, 'v' },
	{ NULL,          0, NULL, 0}
};
const char* const short_options = "p:vV";
const  char *VERSION="Version 0.1 BUILD: "__DATE__;
static std::string strPackFile="";

void ParseOption(int argc, char* argv[])
{
	int next_option;//��һ��Ҫ����Ĳ�������
	do
	{
		next_option = getopt_long (argc, argv, short_options, long_options, NULL);
		switch (next_option)
		{
			case 'p':     /* pack file */    
				/* ��ʱoptargָ�� -c���filename */
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
			case ':':     /* ȱ����ѡ������ */
				break;
			case '?':     /* ����һ��δָ���Ĳ���*/
				break;
			case -1:      /* ������Ϻ󷵻�-1 */
			default:      /* δָ���Ĳ������֣������� */
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
	while (optind < argc)
	{
		/*
		printf ("non-option ARGV-elements: ");
		while (optind < argc)
			printf ("%s ", argv[optind++]);
		putchar ('\n');
		*/
		printf("Appending file = %s\n",argv[optind]);
		file.AppendSubFile(argv[optind],argv[optind]);
		optind++;
	}
	//file.DeleteSubFile("Makefile");
	return 0;
}
