#ifndef _FILEPACK_H_
#define _FILEPACK_H_
#define __USE_LARGEFILE64
#include <stdint.h>
#include <map>
#include <string>
/*
文件格式：
索引区+内容区
索引区=256KB的块（可以放256*1024/512=512个文件索引）
索引区=索引+索引+索引+索引...
索引=512B的块=文件目录名（508B）+偏移（4B）
索引区的最后一个索引是指向下一个索引区的偏移值
索引的第一个4Byte=0，表示该条记录为空
*/
typedef struct _FILEHEADER_
{
	_FILEHEADER_(uint32_t version)
	{
		m_magic='PACK';
		m_version = version;
		if(version==1)
		{
			m_indexcount = 512;
		}else
		{
			m_indexcount = 512;
		}
	}
	uint32_t m_magic;
	uint32_t m_version;
	uint32_t m_indexcount;	//每个索引区的索引数量
}FILEHEADER;

#define INDEXCOUNT	512
#define FILENAMESIZE 500
typedef struct _FILEINDEX_
{
	uint64_t m_offset;		//文件偏移
	uint32_t m_size;		//文件大小(小于4G)，如果==0，且m_filename==“”则表示这个记录为空
	char	m_filename[FILENAMESIZE];	//文件名（含路径）
}FILEINDEX,*PFILEINDEX;
//typedef FILEINDEX FILEBLOCK[INDEXCOUNT];

class LPackFile{
	FILEHEADER m_fileheader;
	std::map<std::string, uint64_t> m_FileOffset;	//subfile's index offset
	std::map<std::string, uint32_t> m_FileSize;
	int m_fd;
	void *m_mmap;
	uint64_t	m_filesize;
public:
	LPackFile();
	LPackFile(const char *filename);
	~LPackFile();
	
	void UnmapFile();
	void MapFile();
	void RecheckAllSubFile();
	
	void OpenFile(const char * filename);
	void CloseFile();
	//在文件末尾附加一个索引块
	void NewIndexBlockAtFileEnd();
	void AppendSubFile(const char*srcfilename, const char* destfilename);
	void DeleteSubFile(const char*destfilename);
	void ReadSubFile(const char*destfilename);
	void PackFile();
	
	uint64_t FindEmptyIndex();
};

#endif

