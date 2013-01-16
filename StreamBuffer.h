#ifndef    _ZJY_STREAMBUFFER_H_
#define    _ZJY_STREAMBUFFER_H_
#include <string>

class CReadStream
{
public:
	CReadStream(const char *buffer,int len)
		:m_bytes(buffer),m_limit(len),m_pos(0)
	{
	}

	size_t LeftBytes()
	{
		return m_limit- m_pos; 
	}

	void Skip(int step)
	{
		m_pos+=step;
	}

	const char *data()
	{
		return m_bytes;
	}


	CReadStream &operator >>(int &v)
	{
		Get(&v,sizeof(v));
		v= ntohl(v);
		return *this;;
	}

	CReadStream &operator >>(unsigned int &v)
	{
		Get(&v,sizeof(v));
		v= ntohl(v);
		return *this;
	}
	CReadStream &operator >>(unsigned short &v)
	{
		Get(&v,sizeof(v));
		v= ntohs(v);
		return *this;
	}

	CReadStream &operator >>( std::string &str)
	{
		int len=0;char c;
		(*this)>>len;
		while(len--)
		{
			(*this)>>c;
			str+=c;
		}
		return *this;
	}

	CReadStream &operator >>( char &c)
	{		
		Get(&c,sizeof(char));
		return *this;
	}

private:
	void Get(void *data,int n)
	{
		if(m_pos+n>m_limit)
			throw "out of range";
		memcpy(data,m_bytes+m_pos,n);
		m_pos+=n;  
	}
protected:
	const char *m_bytes;
	int m_limit;
	int m_pos;
};

//使用string,可以利用stl的memory pool
class CWriteStream
{
private:
	std::string m_bufer;
public:	
	CWriteStream()
	{
	}

	int size()const
	{
		return (int)m_bufer.size();
	}

	const char *data()const
	{
		return m_bufer.data();
	}

	CWriteStream &operator <<(const int &v)
	{
		int t=htonl(v);
		Put(&t,sizeof(t));
		return *this;
	}

	CWriteStream &operator <<(const unsigned int &v)
	{
		int t=htonl(v);
		Put(&t,sizeof(t));
		return *this;
	}

	CWriteStream &operator <<(const unsigned short &v)
	{
		unsigned short t=htons(v);
		Put(&t,sizeof(t));
		return *this;
	}

	//notice!! data not  alias of string
	CWriteStream &operator <<( const char* data)
	{
		size_t len = strlen(data);
		Put(data,(int)len);
		return *this;
	}

	CWriteStream &operator <<( const std::string &str)
	{
		(*this)<<(int)str.size();
		Put(str.c_str(),int(str.size()));
		return *this;
	}

	CWriteStream &operator <<(const char &c)
	{		
		m_bufer+=c;
		return *this;
	}

	CWriteStream &operator <<(const CWriteStream &c)
	{		

		Put( c.data(), c.size() );
		return *this;
	}
	void Put(const void *data,int n)
	{		
		m_bufer.append((const char*)data,n);
	}
	void Clear()
	{
		m_bufer.clear();
	}
};
#endif
