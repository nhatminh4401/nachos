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
#include "synchconsole.h"
#include "syscall.h"
#include "ksyscall.h"
#include "ptable.h"
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

void StartProcess(int arg)
{
	kernel->currentThread->space->Execute(arg);
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);
	char c;
	int vaddr, memval; //value address, memory value
	int buffer;
	int length;
	char *buf;
	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		return;
	case PageFaultException:
	{
		DEBUG(dbgSys, "\n No valid trasnition found");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case ReadOnlyException:
	{
		DEBUG(dbgSys, "\n Write attempted to page marked read-only");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case BusErrorException:
	{
		DEBUG(dbgSys, "\n Translation resulted invalid physical address");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case AddressErrorException:
	{
		DEBUG(dbgSys, "\n Unaligned reference or one that was beyond the end of the address space");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case OverflowException:
	{
		DEBUG(dbgSys, "\n Integer overflow in add or sub.");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case IllegalInstrException:
	{
		DEBUG(dbgSys, "\n Unimplemented or reserved instr.");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}
	case NumExceptionTypes:
	{
		DEBUG(dbgSys, "\n Number exception types");
		SysHalt();
		ASSERTNOTREACHED();
		break;
	}

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			ASSERTNOTREACHED();
			break;
		case SC_Exec:
			char *filename;
			filename = new char[100];

			int buffadd;
			buffadd = kernel->machine->ReadRegister(4); /* only one argument, so that’s in R4 */
			int ch;
			int i;
			//find a proper place to free this allocation
			if (!kernel->machine->ReadMem(buffadd, 1, &ch))
				return;
			i = 0;
			while (ch != 0)
			{
				filename[i] = (char)ch;
				buffadd += 1;
				i++;
				if (!kernel->machine->ReadMem(buffadd, 1, &ch))
					return;
			}

			// AddrSpace *space;
			// space = new AddrSpace();
			// space->Load(filename);
			// Thread *t;
			// t = new Thread(filename);
			// t->space = space;
			// int processId;
			// processId = t->getId(); /* create this function by yourself */
			filename[i] = (char)0;
			/* now filename contains the file */
			//t->Fork(StartProcess, 0);
			//kernel->machine->WriteRegister(2, processId);
			extern PTable *pTab;
			// Return child process id
			int id;
			id = pTab->ExecUpdate(filename);
			kernel->machine->WriteRegister(2, id);
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;
		case SC_Join:
		{
			// int Join(SpaceId id)
			// Input: id dia chi cua thread
			// Output:
			int id;
			id = kernel->machine->ReadRegister(4);

			int res = pTab->JoinUpdate(id);

			kernel->machine->WriteRegister(2, res);
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Exit:
		{
			//void Exit(int status);
			// Input: status code
			int exitStatus = kernel->machine->ReadRegister(4);

			if (exitStatus != 0)
			{
				
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
				return;
				// ASSERTNOTREACHED();
				// break;
			}

			int res = pTab->ExitUpdate(exitStatus);

			kernel->currentThread->FreeSpace();
			kernel->currentThread->Finish();
			kernel->machine->WriteRegister(2, res);
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_Add:
			DEBUG(dbgSys, "Adding... " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

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
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;
			ASSERTNOTREACHED();
			break;

		case SC_ReadNum: //Read Interger
		{
			char num_string[11] = {0}; // max value and min value of C have 11 numbers
			long long l = 0;
			char c;
			int flag_overflow = 0;
			for (int i = 0; i < 12; i++)
			{
				c = kernel->synchConsoleIn->GetChar();
				if (i == 11 && c >= '0' && c <= '9') // Check if input are larger than 11 numbers
				{
					cerr << "Integer overflow";
					flag_overflow = 1;
					break;
				}
				if (c >= '0' && c <= '9') //Check if input are character or int
					num_string[i] = c;
				else if (i == 0 && c == '-') // Check to head of char
					num_string[i] = c;
				else
					break;
			}

			int i = (num_string[0] == '-') ? 1 : 0;
			while (i < 11 && num_string[i] >= '0' && num_string[i] <= '9')
				l = l * 10 + num_string[i++] - '0';
			l = (num_string[0] == '-') ? (-l) : l;
			if (flag_overflow == 1)
				l = 0;
			kernel->machine->WriteRegister(2, (int)l);
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}
			return;
			ASSERTNOTREACHED();
			break;
		}
		case SC_PrintNum: //Print Interger on screen
		{
			int n = kernel->machine->ReadRegister(4);

			char num_string[11] = {0}; // max value and min value of C have 11 numbers
			int tmp[11] = {0}, i = 0, j = 0;

			if (n < 0) //Check if N is negative
			{
				n = -n;
				num_string[i++] = '-';
			}

			do
			{
				tmp[j++] = n % 10; //Take rightmost number
				n /= 10;
			} while (n);
			while (j)
				num_string[i++] = '0' + (char)tmp[--j];
			for (int z = 0; z < i; z++)
				kernel->synchConsoleOut->PutChar(num_string[z]);

			kernel->machine->WriteRegister(2, 0);

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;
		}
		case SC_RandomNum: //Randomize an interger number
		{
			RandomInit(time(0));	// from sysdep.cc nachos
			int n = RandomNumber(); //from sysdep.cc nachos
			DEBUG(dbgSys, n);
			kernel->machine->WriteRegister(2, int(n));

			/* Modify return point */
			{
				/* set previous programm counter (debugging only)*/
				kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

				/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
				kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);

				/* set next programm counter for brach execution */
				kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
			}

			return;

			ASSERTNOTREACHED();

			break;
		}
		case SC_ReadChar: //Read a character

			c = kernel->synchConsoleIn->GetChar();	   //Read char (input)
			kernel->machine->WriteRegister(2, (int)c); //Write to register 2

			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			ASSERTNOTREACHED();
			break;

		case SC_PrintChar: //Print a character

			c = (char)kernel->machine->ReadRegister(4); //Read from memory r4 to get value
			kernel->synchConsoleOut->PutChar(c);		//Write char

			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			ASSERTNOTREACHED();
			break;

		case SC_ReadString: //Read char* with length
			// initialize variables
			buffer = kernel->machine->ReadRegister(4); //Get the value from register r4
			length = kernel->machine->ReadRegister(5); //Get the value from register r5
			buf = NULL;

			if (length > 0) //Input string
			{
				buf = new char[length];
				for (i = 0; i < length - 1; i++)
				{
					c = kernel->synchConsoleIn->GetChar();
					if (c == '\n') //If endline means the end
						break;
					else
						buf[i] = c; //read each character to the array of char
				}
			}

			if (buf != NULL) //If string != NULL
			{
				int n = strlen(buf) + 1;
				for (int i = 0; i < n; i++)
				{
					kernel->machine->WriteMem(buffer + i, 1, (int)buf[i]); //Write to memory
				}
				delete[] buf; //release buf
			}
			//kernel->machine->WriteRegister(2, 0); //Write result to register 2

			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			ASSERTNOTREACHED();
			break;

		case SC_PrintString: //Print char*
			vaddr = kernel->machine->ReadRegister(4);
			kernel->machine->ReadMem(vaddr, 1, &memval); //read memory to get value address
			while ((*(char *)&memval) != '\0')			 //While not end of string (\0)
			{
				kernel->synchConsoleOut->PutChar(*(char *)&memval); //Write each char
				vaddr++;
				kernel->machine->ReadMem(vaddr, 1, &memval); //Read each char from memory to write
			}
			/* set previous programm counter (debugging only)*/
			kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));
			/* set programm counter to next instruction (all Instructions are 4 byte wide)*/
			kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
			/* set next programm counter for brach execution */
			kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);

			return;
			ASSERTNOTREACHED();
			break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
