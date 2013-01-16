#include "FileClient.h"
#include "SysUtil.h"
#include "StreamBuffer.h"

#define	 READ_BUFFER_SIZE (32*1024)

#pragma comment(lib,"Ws2_32.lib")
//-----------------------------------------------------------
const char * FileInfo::read(int &nsize)
{
	static char buffer[READ_BUFFER_SIZE];
	if(!feof(fp))
	{
		nsize = (int)fread(buffer,1,READ_BUFFER_SIZE,fp);
		return buffer;
	}
	else
	{
		return 0;
	}
}
//-----------------------------------------------------------
unsigned int CFileClient::ChoseFile(const char *name,int type)
{
	if(IsFileExist(name))
	{
		m_file.fp = fopen(name,"rb");
		if(m_file.fp!=NULL)
		{
			m_file.file_size = GetSizeOfFile(name);
			m_file.short_name = ExtractFileName(string(name));
			m_file.type = type;
			return m_file.file_size;
		}
		else
		{
			m_error = "打开文件失败";
			return 0;
		}

	}
	else
	{
		m_error = "文件不存在";
		return 0;
	}
}
//-----------------------------------------------------------
bool CFileClient::Connect(const char *host,unsigned short port)
{
	if(!m_sock.Init())
	{
		m_error ="初始化网络失败"; 
		return false;
	}
	for(int i=0;i<3;i++)
	{
		if(m_sock.Connect(host,port))
			return true;
		Sleep(10);
	}
	return false;
}
//-----------------------------------------------------------
int CFileClient::SendFile(PTransInfo info)
{
	if(!info) 
	{
		m_error="info不能为NULL";
		return -1;
	}
	if(!m_file.Valued())
	{
		m_error="待传送文件未定义";
		return -1;
	}
	//修改完全
	info->finished=false;
	info->uploaded=0;
	m_stop=false;
	try
	{
		//发送文件信息
		//filename+filesize
		CWriteStream ws;
		string data;
		ws<<m_file.short_name<<(int)m_file.file_size<<m_file.type;
		m_sock.Send(ws.data(),ws.size());
		//wait....
		m_sock.Read(data);
		if(data.substr(0,2)!="ok")
		{	
			m_error="服务器拒绝了连接";
			m_sock.Close();
			return -1;
		}

		//一气发完
		while(!m_stop && !m_file.Finish())
		{
			int len=0;
			const char *data= m_file.read(len);
			if(data)
			{
				m_sock.Send(data,len);
				m_file.writed+=len;
				info->uploaded=m_file.writed;
			}
		}
		info->finished=true;
	}
	catch(sockerr &e)
	{
		m_error = e.errstr();
		return -1;
	}
	return 0;
}
