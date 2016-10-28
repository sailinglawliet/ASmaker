/****************************************************************/
// Provide a simple implement for RC4 encryption & decryption
// FileName: src4.cpp
// Authors: Sails@OSchina
// Copyright:
// Last Modified Time: 16:40 6/6/2014
/****************************************************************/
//#include "stdafx.h"
#include "src4.h"

const char SimpleRC4::m[8]={0x89,0xab,0xcd,0xef,0x01,0x23,0x45,0x67};

bool SimpleRC4::Encry(char * from , ULONG len_from , char * to)
{
	if(!from||len_from==0||!to) return false;

	CoreRC4(from , len_from , to);
	return true;
}

bool SimpleRC4::Decry(char * from , ULONG len_from , char * to)
{
	if(!from||len_from==0||!to) return false;

	CoreRC4(from , len_from , to);
	return true;
}

void SimpleRC4::PrepareRC4()
{
	UINT j = 0;
	char index[key_size]={0};
	for (UINT i = 0;i < 256 ;++i)
	{
		innerkey[i]=i;
		index[i]=outerkey[i%outerkeylen];
	}
	
	for (UINT i = 0;i < key_size ;++i)
	{
		j = (j+innerkey[i]+index[i])%key_size;
		swap_byte(innerkey[i],innerkey[j]);
	}
}

void SimpleRC4::CoreRC4(char * from , ULONG len_from , char * to)
{
	if(!from||len_from==0||!to) return;

	char key[key_size]={0};
	for (UINT i = 0 ; i < key_size ; ++i)
	{
		key[i]=innerkey[i];
	}

	UINT j = 0, k=0, l=0;
	for (ULONG i = 0 ; i < len_from ; ++i)
	{
		j = (j+1)%key_size;
		k = (k+key[j])%key_size;
		swap_byte(key[j],key[k]);
		l = (key[j]+key[k])%key_size;
		to[i] = from[i] ^ key[l];
	}
}

void Cryptor::set_flag(int f)
{
	_flag = f;
}

bool Cryptor::Enc(char * from , ULONG len_from , char * to)
{
	if(!from||len_from==0||!to) return false;
	bool res;
	if (_flag)
	{
		res=rc.Encry(from ,len_from ,to);
	}
	else
	{
		Cry_Core(from , len_from , to);
		res = true;
	}
	return res;
}

bool Cryptor::Dec(char * from , ULONG len_from , char * to)
{
	if(!from||len_from==0||!to) return false;
	bool res;
	if (_flag)
	{
		res=rc.Decry(from , len_from , to);
	}
	else
	{
		res = Cry_Core(from , len_from , to);		
	}
	return res;
}

bool Cryptor::Cry_Core(char * from , ULONG len_from , char * to)
{

	for(UINT i=0;i<len_from;++i)
	{
		to[i] = from[i]^base_key;
	}
	return true;
}