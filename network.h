#ifndef _NETWORK_H_
#define _NETWORK_H_

#include "common.h"

namespace space{
	class RawData{
		std::vector<unsigned char> buf;
		int writepos;
		int readpos;

	public:
		void clear(){buf.clear();readpos=0;writepos=0;}
		unsigned int size(){return buf.size();}

		void setWritePos(int pos){writepos = pos;}
		void setReadPos(int pos){readpos = pos;}

		void write(const void* data,int size,int pos=-1){
			if(pos!=-1) writepos=pos;
			while((unsigned int)(writepos + size) > buf.size()) buf.push_back(0);

			char *dst = ((char*)(&buf[0]) + writepos);
			memcpy(dst,data,size);
			writepos+=size;
		}

		const char * read(){return (const char *)(&buf[0]);}
		void read(void* dst,int size,int pos=-1){
			if(pos!=-1) readpos=pos;
			memcpy(dst,&buf[readpos],size);
			readpos+=size;
		}

		unsigned char & operator[](unsigned int pos){return buf[pos];}
		void assign(int num,unsigned char c=0){buf.assign(num,c);}

		RawData(){clear();}
		virtual ~RawData(){}

	};
}

#endif
