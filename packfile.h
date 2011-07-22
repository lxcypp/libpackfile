#ifndef _FILEPACK_H_
#define _FILEPACK_H_
/*
文件格式：
索引区+内容区
索引区=256KB的块（可以放256*2=512个文件索引）
索引区=索引+索引+索引+索引...
索引=512B的块=文件目录名（508B）+偏移（4B）
*/
class LPackFile{
public:
	LPackFile(const char *filename);
	void AppendFile(const char*srcfilename, const char* destfilename);
	void DeleteFile(const char*destfilename);
	void PackFile();
};

#endif

