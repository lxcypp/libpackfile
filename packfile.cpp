#include "packfile.h"
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
	m_mmap = mmap(NULL, m_filesize, PROT_READ, MAP_SHARED, m_fd, 0);
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


