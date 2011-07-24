#include "packfile.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

LPackFile::LPackFile()
:m_fileheader(1)
{
	m_fd = 0;
	m_mmap = 0;
	m_filesize = 0;
}

LPackFile::LPackFile(const char *filename)
:m_fileheader(1)
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
	UnmapFile();
	if(m_fd>0)
	{
		close(m_fd);
	}
	m_fd = 0;
	m_FileOffset.clear();
	m_FileSize.clear();
}

void LPackFile::UnmapFile()
{
	if(m_mmap!=0)
	{
		munmap(m_mmap, m_filesize);
	}
	m_mmap = 0;
}
void LPackFile::MapFile()
{
	if(m_fd<=0)
		return;
	lseek64(m_fd, 0, SEEK_SET);
	m_filesize = lseek64(m_fd, 0, SEEK_END);
	lseek64(m_fd, 0, SEEK_SET);
	UnmapFile();
	m_mmap = mmap(NULL, m_filesize, PROT_READ|PROT_WRITE, MAP_SHARED, m_fd, 0);
}

void LPackFile::OpenFile(const char *filename)
{
	if(access(filename,0)!=0)
	{
		m_fd = open(filename, O_CREAT|O_RDWR, 0664);
		write(m_fd, &m_fileheader, sizeof(m_fileheader));
	}else
	{
		m_fd = open(filename, O_RDWR);
		read(m_fd,&m_fileheader,sizeof(m_fileheader));
	}
	lseek64(m_fd, 0, SEEK_SET);
	m_filesize = lseek64(m_fd, 0, SEEK_END);
	lseek64(m_fd, 0, SEEK_SET);
	MapFile();
	
}

void LPackFile::RecheckAllSubFile()
{
	m_FileOffset.clear();
	m_FileSize.clear();
	uint64_t blockoff = sizeof(FILEHEADER);
	while(blockoff < m_filesize)
	{
		FILEINDEX *pIndex = (FILEINDEX*)((char*)m_mmap + blockoff);
		int i=0;
		for(i=0;i<INDEXCOUNT-1;++i)
		{
			if(pIndex[i].m_size==0 && pIndex[i].m_filename[0]==0)
			{//空记录，跳过
			}else
			if(pIndex[i].m_filename[0]==0)
			{//空文件名（被删除的），跳过
			}else
			{
				m_FileOffset[pIndex[i].m_filename]=blockoff + i*sizeof(FILEINDEX);
				m_FileSize[pIndex[i].m_filename]=pIndex[i].m_size;
			}
		}
		//i==INDEXCOUNT-1，最后一条记录，其m_offset域是下一个索引区的偏移值
		if(pIndex[i].m_offset==0)
		{//无下一条记录，返回
			break;
		}else
		{//跳到下一条记录
			blockoff += pIndex[i].m_offset;
		}
	}
}

//在文件末尾附加一个索引块
void LPackFile::NewIndexBlockAtFileEnd()
{
	UnmapFile();
	char b[512*512];
	lseek64(m_fd, 0 ,SEEK_END);
	int size=sizeof(FILEINDEX)*m_fileheader.m_indexcount;
	memset(b,0,size);
	write(m_fd, b, size);
	lseek64(m_fd,0,SEEK_SET);
	m_filesize = lseek64(m_fd, 0, SEEK_END);
//	m_filesize += size;
	MapFile();
}

uint64_t LPackFile::FindEmptyIndex()
{
	uint64_t blockoff = sizeof(FILEHEADER);
	if(blockoff==m_filesize)
	{//只有文件头，没有文件内容，则新建一个索引块
		NewIndexBlockAtFileEnd();
	}
	while(blockoff < m_filesize)
	{
		FILEINDEX *pIndex = (FILEINDEX*)((char*)m_mmap + blockoff);
		int i=0;
		for(i=0;i<INDEXCOUNT-1;++i)
		{
			/*
			if(pIndex[i].m_size==0 && pIndex[i].m_filename[0]==0)
			{//空记录，跳过
				return blockoff + sizeof(FILEINDEX)*i;
			}else */
			if(pIndex[i].m_filename[0]==0)
			{//空文件名（空记录或被删除的）
				return blockoff + sizeof(FILEINDEX)*i;
			}
		}
		//i==INDEXCOUNT-1，最后一条记录，其m_offset域是下一个索引区的偏移值
		if(pIndex[i].m_offset==0)
		{//无下一条记录，需新建一条
			pIndex[i].m_offset = m_filesize;
			NewIndexBlockAtFileEnd();
			return pIndex[i].m_offset;
		}else
		{//跳到下一条记录
			blockoff += pIndex[i].m_offset;
		}
	}
	fprintf(stderr,"FindEmptyIndex Error!\n");
	return -1;
}

void LPackFile::AppendSubFile(const char*srcfilename, const char* destfilename)
{
	int fd = open(srcfilename,O_RDONLY);
	if(fd<=0)
		return;
	uint64_t filesize = lseek64(fd,0,SEEK_END);
	lseek64(fd,0,SEEK_SET);
	uint64_t emptyindexoffset = FindEmptyIndex();
	FILEINDEX *pIndex = (FILEINDEX*)((char*)m_mmap + emptyindexoffset);
	pIndex[0].m_offset = m_filesize;
	pIndex[0].m_size = filesize;
	strncpy(pIndex[0].m_filename, destfilename, FILENAMESIZE);
	
	UnmapFile();
	//复制文件内容
	lseek64(m_fd, 0 ,SEEK_END);
	char buffer[1024*64];
	uint64_t copyed = 0;
	while(copyed<filesize)
	{
		int s = read(fd,buffer,sizeof(buffer));
		if(s==-1)
		{//读取错误，重置索引区，并退出，已写入的数据留在那里放弃
			memset(pIndex, 0, sizeof(FILEINDEX));
			break;
		}
		copyed +=s;
		write(m_fd,buffer,s);
	}
	close(fd);
	
	MapFile();
	RecheckAllSubFile();
}
void LPackFile::DeleteSubFile(const char*destfilename)
{
	if(m_FileOffset.find(destfilename)==m_FileOffset.end())
		return;
	FILEINDEX *pIndex = (FILEINDEX*)((char*)m_mmap + m_FileOffset[destfilename]);
	if(pIndex[0].m_size>0)
		memset( (char*)m_mmap+pIndex[0].m_offset, 0, pIndex[0].m_size );
	memset(pIndex, 0, sizeof(FILEINDEX));
}
void LPackFile::ReadSubFile(const char*destfilename)
{
}
void LPackFile::PackFile()
{
}


