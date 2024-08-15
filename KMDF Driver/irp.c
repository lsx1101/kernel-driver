#include"irp.h"

/*IRP请求处理*/
NTSTATUS IRPHandle(
    PDEVICE_OBJECT driver,
    PIRP irp
) {
    DbgPrint("lsx:进入IRP处理函数");
    PIO_STACK_LOCATION irpStack;
    irpStack = IoGetCurrentIrpStackLocation(irp);
    switch (irpStack->MajorFunction)
    {
    case IRP_MJ_DEVICE_CONTROL:
    {
        DeviceControlHandle(irp);
        break;
    }
    case IRP_MJ_CREATE:
    {
        break;
    }
    case IRP_MJ_CLOSE:
    {
        break;
    }
    default:
        break;
    }
    
    DbgPrint("lsx:离开IRP处理函数");
    irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}


/*IRP_MJ_DEVICE_CONTROL处理函数*/
void DeviceControlHandle(PIRP irp) {
    DbgPrint("lsx:进入IRP_MJ_DEVICE_CONTROL处理函数");
    PIO_STACK_LOCATION irpStack;
    irpStack = IoGetCurrentIrpStackLocation(irp);
    ULONG contolCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    switch (contolCode)
    {
    case READ_CODE:
    {
        //读数据
        Read_Data(irp);
        break;
    }
    case WRITE_CODE:
    {
        //写数据
        Write_Data(irp);
        break;
    }
    case READ_WRITE_CODE:
    {
        //读写数据
        Read_Write_Data(irp);
        break;
    }
    case ADD_PROTECT_PID:
    {
        //读写数据
        Add_Pid(irp);
        break;
    }
    case DEL_PROTECT_PID:
    {
        //读写数据
        Del_Pid(irp);
        break;
    }
    case READ_MEMORY:
    {
        //读写数据
        Read_Memory_Pid(irp);
        break;
    }
    default:
        break;
    }

    DbgPrint("lsx:离开IRP_MJ_DEVICE_CONTROL处理函数");
}

void Add_Pid(PIRP irp) {
    int* buffer = (int*)irp->AssociatedIrp.SystemBuffer;
    if (buffer)
    {
        UINT32* pPid = (UINT32*)buffer;
        UINT32 pid = pPid[0];
        addPid(pid);
        irp->IoStatus.Information = sizeof(int);
    }
}

void Del_Pid(PIRP irp) {
    int* buffer = (int*)irp->AssociatedIrp.SystemBuffer;
    if (buffer)
    {
        UINT32* pPid = (UINT32*)buffer;
        UINT32 pid = pPid[0];
        delPid(pid);
        irp->IoStatus.Information = sizeof(int);
    }
}

void Read_Memory_Pid(PIRP irp) {
    UINT64* buffer = (UINT64*)irp->AssociatedIrp.SystemBuffer;
    if (buffer)
    {
        DbgPrint("lsx:进入Read_Memory_Pid处理函数");
        UINT32 PID = (UINT32)(UINT64)buffer[0];
        PVOID pBase = (PVOID)(UINT64)buffer[1];
        //PVOID lpBuffer = (UINT32)(UINT64)buffer[2];
        UINT32 size = (UINT32)(UINT64)buffer[3];
        DbgPrint("lsx:pid=%d pBase=%p size=%d", PID, pBase, size);
        UINT32 readSize = ReadProcessMemoryForPid(PID, pBase, buffer, size);
        irp->IoStatus.Information = size;
    }

}

void Read_Data(PIRP irp) {
    char* outBuf = (char*)irp->AssociatedIrp.SystemBuffer;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(irp);
    char outMsg[] = "lsx:内核层返回数据->1234567890lsx\n";
    memcpy_s(outBuf, 512, outMsg, sizeof(outMsg));
    irp->IoStatus.Information = sizeof(outMsg);
}

void Write_Data(PIRP irp) {
    char* inputBuffer = (char*)irp->AssociatedIrp.SystemBuffer;
    KdPrint(("lsx:读取到用户层数据->%s", inputBuffer));
    irp->IoStatus.Information = 4;

}

void Read_Write_Data(PIRP irp) {
    char* buf = (char*)irp->AssociatedIrp.SystemBuffer;
    KdPrint(("lsx:读取到用户层数据->%s", buf));

    char outMsg[] = "lsx:内核层返回数据->asdfghqwertylsx\n";
    memcpy_s(buf, 512, outMsg, sizeof(outMsg));
    irp->IoStatus.Information = sizeof(outMsg);
}
