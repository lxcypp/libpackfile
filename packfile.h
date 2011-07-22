#ifndef _FILEPACK_H_
#define _FILEPACK_H_
#define __USE_LARGEFILE64
#include <stdint.h>
#include <map>
/*
文件格式：
索引区+内容区
索引区=256KB的块（可以放256*1024/512=512个文件索引）
索引区=索引+索引+索引+索引...
索引=512B的块=文件目录名（508B）+偏移（4B）
索引区的最后一个索引是指向下一个索引区的偏移值
索引的第一个4Byte=0，表示该条记录为空
*/

#define INDEXCOUNT	512
typedef struct _FILEINDEX_
{
	uint32_t m_size;		//文件大小(小于4G)，如果==0，且m_filename==“”则表示这个记录为空
	char	m_filename[500];	//文件名（含路径）
	uint64_t m_offset;		//文件偏移
}FILEINDEX,*PFILEINDEX;
typedef FILEINDEX FILEBLOCK[INDEXCOUNT];

class LPackFile{
	std::map<const char *, uint64_t> m_FileOffset;
	std::map<const char *, uint32_t> m_FileSize;
	int m_fd;
	void *m_mmap;
	uint64_t	m_filesize;
public:
	LPackFile();
	LPackFile(const char *filename);
	~LPackFile();
	void OpenFile(const char * filename);
	void CloseFile();
	void AppendSubFile(const char*srcfilename, const char* destfilename);
	void DeleteSubFile(const char*destfilename);
	void ReadSubFile(const char*destfilename);
	void PackFile();
};

#endif

