#include "packfile.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

LPackFile::LPackFile()
{
	m_fd = 0;
	m_mmap = 0;
	m_filesize = 0;
}

LPackFile::LPackFile(const char *filename)
{
	m_fd = 0;
	m_mmap = 0;
	m_filesize = 0;
	OpenFile(filename);
}

LPackFile::~LPackFile()
{
	CloseFile();
}

void LPackFile::CloseFile()
{
	if(m_fd>0)
	{
		close(m_fd);
	}
	if(m_mmap!=0)
	{
		munmap(m_mmap, m_filesize);
	}
	m_fd = 0;
	m_mmap = 0;
	m_FileOffset.clear();
	m_FileSize.clear();
}

void LPackFile::OpenFile(const char *filename)
{
	if(access(filename,0)!=0)
	{
		m_fd = open(filename, O_CREAT);
	}else
	{
		m_fd = open(filename, O_RDWR);
	}
	m_filesize = lseek64(m_fd, 0, SEEK_END);
	lseek64(m_fd, 0, SEEK_SET);
	m_mmap = mmap(NULL, m_filesize, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0);
	
	uint64_t blockoff = 0;
	while(blockoff < m_filesize)
	{
		FILEINDEX *pIndex = (FILEINDEX*)((char*)m_mmap + blockoff);
		for(int i=0;i<INDEXCOUNT;++i)
		{
			if(pIndex[i].m_size==0 && pIndex[i].m_filename[0]==0)
			{//空记录，跳过
			}else
			if(pIndex[i].m_filename[0]==0)
			{//空文件名（被删除的），跳过
			}else
			{
				m_FileOffset[pIndex[i].m_filename]=pIndex[i].m_offset;
				m_FileSize[pIndex[i].m_filename]=pIndex[i].m_size;
			}
		}
	}
}

void LPackFile::AppendSubFile(const char*srcfilename, const char* destfilename)
{
}
void LPackFile::DeleteSubFile(const char*destfilename)
{
}
void LPackFile::ReadSubFile(const char*destfilename)
{
}
void LPackFile::PackFile()
{
}


