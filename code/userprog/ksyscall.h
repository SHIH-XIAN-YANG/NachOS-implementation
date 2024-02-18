/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls 
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__ 
#define __USERPROG_KSYSCALL_H__ 

#include "kernel.h"

#include "synchconsole.h"

void SysHalt()
{
  kernel->interrupt->Halt();
}

int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

// void SysPrintInt(int num){
// 	if(!num){
// 		putchar('0');
// 		putchar('\n');
// 	}

// 	int temp = num;
// 	int i = 1;

// 	while(temp){
// 		i*=10;
// 		temp/=10;
// 	}
// 	i/=10;

// 	while(num){
// 		putchar(num/i + '0');
// 		num%=i;
// 		i/=10;
// 	}
// 	while(i--){
// 		putchar('0');
// 	}
// 	putchar('\n');
// }


void SysPrintInt(int num){
	kernel->synchConsoleOut->PutInt(num);
}

OpenFileId SysOpen(char *file_name){
	return kernel->fileSystem->Open_File(file_name);
}

int SysWrite(char *buffer, int size, OpenFileId id){
	return kernel->fileSystem->Write_File(buffer, size, id);
}

int SysRead(char *buffer, int size, OpenFileId id){
	return kernel->fileSystem->Read_File(buffer, size, id);
}

int SysClose(OpenFileId id){
	return kernel->fileSystem->Close_File(id);
}


int SysCreate(char *filename)
{
	// return value
	// 1: success
	// 0: failed
	return kernel->interrupt->CreateFile(filename);
}


#endif /* ! __USERPROG_KSYSCALL_H__ */
