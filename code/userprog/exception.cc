// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------

void
ExceptionHandler(ExceptionType which)
{
    int type = kernel->machine->ReadRegister(2);
	int val,fid,size;
    int status, exit, threadID, programID;
	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");
    switch (which) {
    case SyscallException:
      	switch(type) {
      	case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
			SysHalt();
                        cout<<"in exception\n";
			ASSERTNOTREACHED();
			break;
		case SC_MSG:
			DEBUG(dbgSys, "Message received.\n");
			val = kernel->machine->ReadRegister(4);
			{
			char *msg = &(kernel->machine->mainMemory[val]);
			cout << msg << endl;
			}
			SysHalt();
			ASSERTNOTREACHED();
			break;
		case SC_Create:
			//read input argument from register #4
			val = kernel->machine->ReadRegister(4);
			{
			// get file name to be opened
			char *filename = &(kernel->machine->mainMemory[val]);
			
			//System call
			status = SysCreate(filename);
			
			//write result(status flag) to register #2
			kernel->machine->WriteRegister(2, status);
			}

			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
            break;
		case SC_PrintInt:
		{
			//read input argument
			val = kernel->machine->ReadRegister(4);
			
			// System call
			SysPrintInt(val);

			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
            break;
		}
		case SC_Open:
		{
			val = kernel->machine->ReadRegister(4);
			
			char *file_name = &(kernel->machine->mainMemory[val]);
			fid = SysOpen(file_name);
			
			kernel->machine->WriteRegister(2, fid);

			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Close:
		{
			val = kernel->machine->ReadRegister(4);
			
			int ret = SysClose(val);
			
			kernel->machine->WriteRegister(2, ret);
			
			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Read:
		{
			val = kernel->machine->ReadRegister(4);   // first argument
			size = kernel->machine->ReadRegister(5);  // second argument
			fid = kernel->machine->ReadRegister(6);

			char *data_read = &(kernel->machine->mainMemory[val]);

			int num_read = SysRead(data_read, size, fid);

			kernel->machine->WriteRegister(2, num_read);
			
			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Write:
		{
			val = kernel->machine->ReadRegister(4);   // first argument
			size = kernel->machine->ReadRegister(5);  // second argument
			fid = kernel->machine->ReadRegister(6);

			char *data_write = &(kernel->machine->mainMemory[val]);

			int num_write = SysWrite(data_write, size, fid); //return num of write

			kernel->machine->WriteRegister(2, num_write);
			
			// Write PC
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
			/* int op2 */(int)kernel->machine->ReadRegister(5));
			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);	
			/* Modify return point */
			{
	  		/* set previous programm counter (debugging only)*/
	  		kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
	  			
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  		kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	 		/* set next programm counter for brach execution */
	 		kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg)+4);
			}
			cout << "result is " << result << "\n";	
			return;	
			ASSERTNOTREACHED();
            break;
		case SC_Exit:
			DEBUG(dbgAddr, "Program exit\n");
            val=kernel->machine->ReadRegister(4);
            cout << "return value:" << val << endl;
			kernel->currentThread->Finish();
            break;
      	default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception " << (int)which << "\n";
		break;
    }
    ASSERTNOTREACHED();
}

