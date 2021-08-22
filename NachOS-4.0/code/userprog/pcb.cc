#include "pcb.h"
#include "utility.h"
#include "thread.h"
#include "addrspace.h"

extern void StartProcess(int arg);
PCB::PCB(int id)
{
    if (id == 0)
        this->parentID = -1;
    else
        this->parentID = kernel->currentThread->pid;

	this->exitcode = 0;
	this->thread = NULL;

	this->joinsem = new Semaphore("joinsem",0);
}
PCB::~PCB()
{
	
	if(joinsem != NULL)
		delete this->joinsem;
	if(thread != NULL)
	{		
		thread->FreeSpace();
		thread->Finish();
		
	}
}
int PCB::GetID(){ return this->thread->pid; }
int PCB::GetExitCode() { return this->exitcode; }
void PCB::SetExitCode(int ec){ this->exitcode = ec; }

// Process tranlation to block
// Wait for JoinRelease to continue exec
void PCB::JoinWait()
{
	//Gọi joinsem->P() để tiến trình chuyển sang trạng thái block và ngừng lại, chờ JoinRelease để thực hiện tiếp.
    joinsem->P();
}

// JoinRelease process calling JoinWait
void PCB::JoinRelease()
{ 
	// Gọi joinsem->V() để giải phóng tiến trình gọi JoinWait().
    joinsem->V();
}

void PCB::SetFileName(char* fn){ strcpy(FileName,fn);}
char* PCB::GetFileName() { return this->FileName; }

int PCB::Exec(char* filename, int id)
{  
    // Kiểm tra thread đã khởi tạo thành công chưa, nếu chưa thì báo lỗi là không đủ bộ nhớ, gọi mutex->V() và return.             
	this->thread = new Thread(filename);

	if(this->thread == NULL){
		printf("\nPCB::Exec:: Not enough memory..!\n");
		return -1;
	}

	//  Đặt processID của thread này là id.
	this->thread->pid = id;
	// Đặt parrentID của thread này là processID của thread gọi thực thi Exec
	this->parentID = kernel->currentThread->pid;
 	this->thread->Fork((VoidFunctionPtr) &StartProcess,(void*) id);

	// Trả về id.
	return id;

}