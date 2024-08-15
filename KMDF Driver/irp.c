#include"irp.h"

/*IRP������*/
NTSTATUS IRPHandle(
    PDEVICE_OBJECT driver,
    PIRP irp
) {
    DbgPrint("lsx:����IRP������");
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
    
    DbgPrint("lsx:�뿪IRP������");
    irp->IoStatus.Status = STATUS_SUCCESS;
    IoCompleteRequest(irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}


/*IRP_MJ_DEVICE_CONTROL������*/
void DeviceControlHandle(PIRP irp) {
    DbgPrint("lsx:����IRP_MJ_DEVICE_CONTROL������");
    PIO_STACK_LOCATION irpStack;
    irpStack = IoGetCurrentIrpStackLocation(irp);
    ULONG contolCode = irpStack->Parameters.DeviceIoControl.IoControlCode;
    switch (contolCode)
    {
    case READ_CODE:
    {
        //������
        Read_Data(irp);
        break;
    }
    case WRITE_CODE:
    {
        //д����
        Write_Data(irp);
        break;
    }
    case READ_WRITE_CODE:
    {
        //��д����
        Read_Write_Data(irp);
        break;
    }
    case ADD_PROTECT_PID:
    {
        //��д����
        Add_Pid(irp);
        break;
    }
    case DEL_PROTECT_PID:
    {
        //��д����
        Del_Pid(irp);
        break;
    }
    case READ_MEMORY:
    {
        //��д����
        Read_Memory_Pid(irp);
        break;
    }
    default:
        break;
    }

    DbgPrint("lsx:�뿪IRP_MJ_DEVICE_CONTROL������");
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
        DbgPrint("lsx:����Read_Memory_Pid������");
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
    char outMsg[] = "lsx:�ں˲㷵������->1234567890lsx\n";
    memcpy_s(outBuf, 512, outMsg, sizeof(outMsg));
    irp->IoStatus.Information = sizeof(outMsg);
}

void Write_Data(PIRP irp) {
    char* inputBuffer = (char*)irp->AssociatedIrp.SystemBuffer;
    KdPrint(("lsx:��ȡ���û�������->%s", inputBuffer));
    irp->IoStatus.Information = 4;

}

void Read_Write_Data(PIRP irp) {
    char* buf = (char*)irp->AssociatedIrp.SystemBuffer;
    KdPrint(("lsx:��ȡ���û�������->%s", buf));

    char outMsg[] = "lsx:�ں˲㷵������->asdfghqwertylsx\n";
    memcpy_s(buf, 512, outMsg, sizeof(outMsg));
    irp->IoStatus.Information = sizeof(outMsg);
}
