/****************************************************************/
// Provide a simple implement for RC4 encryption & decryption
// FileName: src4.h
// Authors: Sails@OSchina
// Copyright:
// Last Modified Time: 16:40 6/6/2014
// RC4加密算法，由一个外部key(最好64bit以上)决定内部的一个sbox亦即
// InnnerKey来作为xor运算的源，对明文进行加密
// MultiThreading & mmap?
/****************************************************************/
#ifndef _simple_rc4_
#define _simple_rc4_

typedef unsigned int UINT;
typedef unsigned long ULONG;

const UINT key_size=256;

class SimpleRC4 
{
public:
	//Cons & Decons
	SimpleRC4(UINT len=8):outerkeylen(len)
	{
		outerkeylen = outerkeylen > 8?8:outerkeylen;

		outerkey = new char[outerkeylen];
		for (UINT i = 0; i < outerkeylen; ++i)
		{
			outerkey[i]=m[i];
		}

		PrepareRC4();
	}
	~SimpleRC4()
	{
		delete[] outerkey;
	}
	bool Encry(char * from , ULONG len_from , char * to);
	bool Decry(char * from , ULONG len_from , char * to);
private:
	//Bans for Copy cons
	SimpleRC4(const SimpleRC4 &);
	SimpleRC4& operator=(const SimpleRC4 &);

	//UTIL
	void PrepareRC4();
	void CoreRC4(char * from , ULONG len_from , char * to);
private:
	//key len
	UINT outerkeylen;
	//key for user
	char * outerkey;
	//key for RC4
	char innerkey[key_size];
	//Magic Number
	const static char m[8];
};

inline void swap_byte(char& a , char& b)
{
	//not even a skill, if we turn on o2 
	//if (a!=b)
		//b^=a^=b^=a;
	char temp=a;
	a = b;
	b = temp;
}

class Cryptor
{
public:
	Cryptor(UINT flag = 1):_flag(flag),base_key(0x12){}
	~Cryptor(){}
	void set_flag(int);
	bool Enc(char * from , ULONG len_from , char * to);
	bool Dec(char * from , ULONG len_from , char * to);
private:
	//UTIL
	bool Cry_Core(char * from , ULONG len_from , char * to);
	//bans
	Cryptor(const Cryptor&);
	Cryptor & operator= (const Cryptor&);
private:
	UINT _flag;
	SimpleRC4 rc;
	char base_key;
};

#endif