#include "packfile.h"
LPackFile file;//("a.pac");
int main(int, char*[])
{
	file.OpenFile("a.pac");
	file.AppendSubFile("Makefile","Makefile");
	return 0;
}
