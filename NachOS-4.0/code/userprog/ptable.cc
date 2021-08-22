#include "ptable.h"
#include "main.h"
#include "openfile.h"

#define For(i,a,b) for (int i = (a); i < b; ++i)

PTable::PTable(int size)
{

    if (size < 0)
        return;

    psize = size;
    bm = new Bitmap(size);

    For(i,0,MAX_PROCESS){
		pcb[i] = 0;
    }

	bm->Mark(0);

	pcb[0] = new PCB(0);
	pcb[0]->SetFileName("./test/scheduler");
	pcb[0]->parentID = -1;
}

PTable::~PTable()
{
    if( bm != 0 )
	delete bm;
    
    For(i,0,psize){
		if(pcb[i] != 0)
			delete pcb[i];
    }
}

int PTable::ExecUpdate(char* name)
{
	// Tìm slot trống trong bảng Ptable.
	int index = this->GetFreeSlot();

    // Check if have free slot
	if(index < 0)
	{
		printf("\nPTable::Exec :There is no free slot.\n");
		return -1;
	}

	//Nếu có slot trống thì khởi tạo một PCB mới với processID chính là index của slot này
	pcb[index] = new PCB(index);
	pcb[index]->SetFileName(name);

	// parrentID là processID của currentThread
    pcb[index]->parentID = kernel->currentThread->threadId;

	// Gọi thực thi phương thức Exec của lớp PCB.
	int pid = pcb[index]->Exec(name,index);

	// Trả về kết quả thực thi của PCB->Exec.
	return pid;
}

int PTable::JoinUpdate(int id)
{
	pcb[id]->JoinWait();

	// Xử lý exitcode.	
	int ec = pcb[id]->GetExitCode();

    // Successfully
	return ec;
}
int PTable::ExitUpdate(int exitcode)
{              
    // Nếu tiến trình gọi là main process thì gọi Halt().
	int id = kernel->currentThread->threadId;
	if(id == 0)
	{
		
		kernel->currentThread->FreeSpace();		
		kernel->interrupt->Halt();
		return 0;
	}
    
        if(IsExist(id) == false)
	{
		printf("\nPTable::ExitUpdate: This %d is not exist. Try again?", id);
		return -1;
	}

	// Ngược lại gọi SetExitCode để đặt exitcode cho tiến trình gọi.
	pcb[id]->SetExitCode(exitcode);
    
    // Gọi JoinRelease để giải phóng tiến trình cha đang đợi nó(nếu có)
	pcb[id]->JoinRelease();
	
	Remove(id);
	return exitcode;
}

// Find free slot in order to save the new process infom
int PTable::GetFreeSlot()
{
	return bm->FindAndSet();
}

// Check if Process ID is Exist
bool PTable::IsExist(int pid)
{
	return bm->Test(pid);
}

// Remove proccess ID out of table
// When it ends
void PTable::Remove(int pid)
{
	bm->Clear(pid);
	if(pcb[pid] != 0)
		delete pcb[pid];
}

char* PTable::GetFileName(int id)
{
	return (pcb[id]->GetFileName());
}