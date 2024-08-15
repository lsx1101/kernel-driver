#include <ntifs.h>
#include<minwindef.h>
#include"Memory.h"

UINT64 WP_ON(KIRQL irql) {
    UINT64 cr0 = __readcr0();
    UINT64 old_cr0 = cr0;
    cr0 |= 0x10000;
    _enable();
    __writecr0(cr0);
    KeLowerIrql(irql);
    return old_cr0;
}

KIRQL WP_OFF() {
    KIRQL irql = KeRaiseIrqlToDpcLevel();
    UINT64 cr0 = __readcr0();
    cr0 &= 0xfffffffffffeffff;
    __writecr0(cr0);
    _disable();
    return irql;
}

NTSTATUS ZwProtectVirtualMemory(
    IN HANDLE ProcessHandle,
    IN OUT PVOID * BaseAddress,
    IN OUT PSIZE_T RegionSize,
    IN ULONG NewProtect,
    OUT PULONG OldProtect
);

NTSTATUS ReadMemory(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:���ڴ� pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T bytesTransferred = 0;

    __try {
        status = MmCopyVirtualMemory(
            targetProcess,
            (PVOID)memOp->Address,
            PsGetCurrentProcess(),
            memOp->Buffer,
            memOp->Size,
            KernelMode,
            &bytesTransferred
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:MmCopyVirtualMemoryִ��ʧ�� GetExceptionCode=\n");
        }
        DbgPrint("lsx:���ڴ� status=%u Buffer=%u\n", status, memOp->Buffer);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:���ڴ�ʧ�� GetExceptionCode=%u\n", status, GetExceptionCode());
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS WriteMemory(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:д�ڴ� pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T bytesTransferred = 0;

    __try {
        status = MmCopyVirtualMemory(
            PsGetCurrentProcess(),
            memOp->Buffer,
            targetProcess,
            (PVOID)memOp->Address,
            memOp->Size,
            KernelMode,
            &bytesTransferred
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:MmCopyVirtualMemoryִ��ʧ�� GetExceptionCode=\n");
        }


    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:д�ڴ�ʧ�� GetExceptionCode=%u\n", GetExceptionCode());
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS WriteMemoryByMdl(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:MDLд�ڴ� pid=%u address=%p buffer=%d size=%u\n", 
        memOp->ProcessId,
        memOp->Address,
        *(BYTE*)memOp->Buffer,
        memOp->Size);
    NTSTATUS status = STATUS_SUCCESS;
    // ��ȡд������
    BYTE* GetData;
    __try
    {
        GetData = ExAllocatePool(PagedPool, memOp->Size);
        RtlCopyMemory(GetData, memOp->Buffer, memOp->Size);
    }
    __except (1)
    {
        return FALSE;
    }
    // ���ӵ�Ŀ�����
    KAPC_STATE stack = { 0 };
    KeStackAttachProcess(targetProcess, &stack);

    PMDL mdl = IoAllocateMdl((PVOID)memOp->Address, memOp->Size, 0, 0, NULL);
    if (mdl == NULL)
    {
        return FALSE;
    }

    MmBuildMdlForNonPagedPool(mdl);

    PVOID ChangeData = NULL;

    __try
    {
        ChangeData = MmMapLockedPages(mdl, KernelMode);
        RtlCopyMemory(ChangeData, GetData, memOp->Size);
    }
    __except (1)
    {
        status = FALSE;
        goto END;
    }

END:
    IoFreeMdl(mdl);
    KeUnstackDetachProcess(&stack);
    ObDereferenceObject(targetProcess);

    return status;
}

NTSTATUS WriteMemoryOnlyRead(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:д�ڴ� pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T bytesTransferred = 0;
    HANDLE hTargetProcess = NULL;
    ULONG oldProtect;
    SIZE_T returnSize;

    // ����һ���������������ڸ�����Դ�����Ƿ�ɹ�
    BOOLEAN resourceAllocated = FALSE;

    __try {
        // ��PEPROCESS��ȡ�û�ģʽ���̾��
        status = ObOpenObjectByPointer(
            targetProcess,
            OBJ_KERNEL_HANDLE,
            NULL,
            PROCESS_ALL_ACCESS,
            *PsProcessType,
            KernelMode,
            &hTargetProcess
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwQueryVirtualMemoryִ��ʧ�� status=%u\n", status);
            // ���ش�����벢�����ѷ������Դ
            return status;
        }
        // �����Դ����ɹ�
        resourceAllocated = TRUE;

        // ��ȡĿ���ڴ�ҳ��ĵ�ǰ��������
        MEMORY_BASIC_INFORMATION mbi;
        status = ZwQueryVirtualMemory(
            hTargetProcess,
            (PVOID)memOp->Address,
            MemoryBasicInformation,
            &mbi,
            sizeof(mbi),
            &returnSize
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwQueryVirtualMemoryִ��ʧ�� status=%u\n", status);
            // ���ش�����벢�����ѷ������Դ
            return status;
        }

        // �޸��ڴ�ҳ�汣������Ϊ��д
        status = ZwProtectVirtualMemory(
            hTargetProcess,
            (PVOID)memOp->Address,
            &memOp->Size,
            PAGE_READWRITE,
            &oldProtect
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwProtectVirtualMemoryִ��ʧ�� status=%u\n", status);
            // ���ش�����벢�����ѷ������Դ
            return status;
        }

        // ʹ��MmCopyVirtualMemory���п�����ڴ濽��
        status = MmCopyVirtualMemory(
            PsGetCurrentProcess(),
            memOp->Buffer,
            targetProcess,
            (PVOID)memOp->Address,
            memOp->Size,
            KernelMode,
            &bytesTransferred
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:MmCopyVirtualMemoryִ��ʧ�� status=%u\n", status);
            // ���ش�����벢�����ѷ������Դ
            return status;
        }

        // �ָ��ڴ�ҳ���ԭʼ��������
        status = ZwProtectVirtualMemory(
            hTargetProcess,
            (PVOID)memOp->Address,
            &memOp->Size,
            oldProtect,
            &oldProtect
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwProtectVirtualMemoryִ��ʧ�� status=%u\n", status);
            // ���ش�����벢�����ѷ������Դ
            return status;
        }

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:д�ڴ�ʧ�� GetExceptionCode=%u\n", GetExceptionCode());
        // �����쳣���벢�����ѷ������Դ
        status = GetExceptionCode();
    }

    // �����еķ���·���϶�������Դ����
    if (resourceAllocated) {
        ZwClose(hTargetProcess);
    }

    return status;
}


NTSTATUS WriteMemoryOld(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    PMDL mdl = IoAllocateMdl((PVOID)memOp->Address, (ULONG)memOp->Size, FALSE, FALSE, NULL);
    if (!mdl) {
        DbgPrint("lsx:IoAllocateMdlִ��ʧ��\n");
        ObDereferenceObject(targetProcess);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    __try {
        MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:MmProbeAndLockPagesִ��ʧ�� GetExceptionCode=%u\n", GetExceptionCode());
        IoFreeMdl(mdl);
        ObDereferenceObject(targetProcess);
        return GetExceptionCode();
    }

    PVOID mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
    if (!mappedAddress) {
        DbgPrint("lsx:MmMapLockedPagesSpecifyCacheִ��ʧ��\n");
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        ObDereferenceObject(targetProcess);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);
    if (!NT_SUCCESS(status)) {
        DbgPrint("lsx:MmProtectMdlSystemAddressִ��ʧ��\n");
        MmUnmapLockedPages(mappedAddress, mdl);
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        ObDereferenceObject(targetProcess);
        return status;
    }

    RtlCopyMemory(mappedAddress, memOp->Buffer, memOp->Size);

    MmProtectMdlSystemAddress(mdl, PAGE_READONLY);

    MmUnmapLockedPages(mappedAddress, mdl);
    MmUnlockPages(mdl);
    IoFreeMdl(mdl);
    ObDereferenceObject(targetProcess);

    return STATUS_SUCCESS;
}

NTSTATUS KReadProcessMemory(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE kapcState;
    RtlZeroMemory(&kapcState, sizeof(KAPC_STATE));

    PVOID tempBufKernel = ExAllocatePool2(NonPagedPool, memOp->Size, 'mtag');

    KeStackAttachProcess((PVOID)targetProcess, &kapcState);
    BOOLEAN ret = MmIsAddressValid((PVOID)memOp->Address);
    if (ret)
    {
        RtlCopyMemory(tempBufKernel, (PVOID)memOp->Address, (UINT32)memOp->Size);

    }
    KeUnstackDetachProcess(&kapcState);
    RtlCompareMemory(memOp->Buffer, tempBufKernel, (UINT32)memOp->Size);
    ExFreePool(tempBufKernel);
    return status;
}

NTSTATUS KWriteProcessMemory(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE kapcState;
    RtlZeroMemory(&kapcState, sizeof(KAPC_STATE));
    DbgPrint("lsx:KWriteProcessMemoryִ�� pid=%u address=%p buffer=%u\n", memOp->ProcessId, memOp->Address, memOp->Buffer);
    PMDL g_pdml = IoAllocateMdl(memOp->Buffer, (UINT32)memOp->Size, 0, 0, NULL);
    if (!g_pdml)
    {
        return status;
    }
    MmBuildMdlForNonPagedPool(g_pdml);
    //unsigned char* Mapped = (unsigned char*)MmMapLockedPages(g_pdml, KernelMode);
    PVOID Mapped = MmMapLockedPagesSpecifyCache(g_pdml, KernelMode, MmCached, NULL, FALSE, NormalPagePriority);

    if (!Mapped)
    {
        IoFreeMdl(g_pdml);
        return status;
    }
    KeStackAttachProcess((PVOID)targetProcess, &kapcState);
    // ���Ŀ������Ƿ���Ч
    BOOLEAN ret = MmIsAddressValid((PVOID)memOp->Address);
    if (ret)
    {
        DbgPrint("lsx:MmIsAddressValidִ��\n");
        // д�����ݵ�Ŀ������ڴ�
        RtlCopyMemory((PVOID)memOp->Address, Mapped, (UINT32)memOp->Size);
    }
    else {
        DbgPrint("lsx:MmIsAddressValidִ�д���\n");
        status = STATUS_INVALID_PARAMETER;
    }
    // ����Ŀ����̵�������
    KeUnstackDetachProcess(&kapcState);
    MmUnmapLockedPages((PVOID)Mapped, g_pdml);
    IoFreeMdl(g_pdml);

    return status;
}

NTSTATUS KWriteProcessMemory2(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE KapcState;
    PMDL mdl = NULL;

    DbgPrint("lsx:KWriteProcessMemoryOptimizedִ�� pid=%u address=%p buffer=%u\n",
        memOp->ProcessId, memOp->Address, *(unsigned char*)memOp->Buffer);

    // ���ӵ�Ŀ�����������
    KeStackAttachProcess((PVOID)targetProcess, &KapcState);

    // ���Ŀ���ַ��Ч��
    if (!MmIsAddressValid((PVOID)memOp->Address))
    {
        DbgPrint("lsx:MmIsAddressValidִ�д���\n");
        status = STATUS_INVALID_ADDRESS;
        goto Cleanup;
    }

    // ���䲢��ʼ��MDL
    mdl = IoAllocateMdl((PVOID)memOp->Address, (UINT32)memOp->Size, FALSE, FALSE, NULL);
    if (!mdl)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    MmBuildMdlForNonPagedPool(mdl);
    // ӳ���ڴ棬ָ��д�뻺��������ȷ����ȷд��
    PVOID mappedAddress = NULL;
    mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
    if (!mappedAddress)
    {
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto FreeMdl;
    }

    // ִ��д�����
    RtlCopyMemory(mappedAddress, memOp->Buffer, (UINT32)memOp->Size);

    // ���ӳ��
    MmUnmapLockedPages(mappedAddress, mdl);
Cleanup:
    // �������������
    KeUnstackDetachProcess(&KapcState);

FreeMdl:
    if (mdl)
    {
        // �ͷ�MDL
        IoFreeMdl(mdl);
    }

    return status;
}
NTSTATUS KWriteProcessMemory1(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE kapcState;
    RtlZeroMemory(&kapcState, sizeof(KAPC_STATE));
    DbgPrint("lsx:KWriteProcessMemory1ִ�� pid=%u address=%p buffer=%u\n", memOp->ProcessId, memOp->Address, *(unsigned char*)memOp->Buffer);
    KeStackAttachProcess((PVOID)targetProcess, &kapcState);
    // ���Ŀ������Ƿ���Ч
    BOOLEAN ret = MmIsAddressValid((PVOID)memOp->Address);
    if (!ret)
    {
        DbgPrint("lsx:MmIsAddressValidִ�д���\n");
        status = STATUS_INVALID_PARAMETER;
        return status;
    }
    PMDL g_pdml = IoAllocateMdl((PVOID)memOp->Address, (UINT32)memOp->Size, 0, 0, NULL);
    if (!g_pdml)
    {
        return status;
    }
    MmBuildMdlForNonPagedPool(g_pdml);
    //g_pdml->MdlFlags = MDL_WRITE_OPERATION | MDL_ALLOCATED_FIXED_SIZE | MDL_PAGES_LOCKED ;
    //MmProbeAndLockPages(g_pdml, KernelMode, IoWriteAccess);
   // unsigned char* Mapped = (unsigned char*)MmMapLockedPages(g_pdml, KernelMode);
    PVOID Mapped = MmMapLockedPagesSpecifyCache(g_pdml, KernelMode, MmCached, NULL, FALSE, NormalPagePriority);
    if (!Mapped)
    {
        IoFreeMdl(g_pdml);
        return status;
    }

    DbgPrint("lsx:MmIsAddressValidִ��\n");
    // д�����ݵ�Ŀ������ڴ�
     // �ر�д����
    KIRQL irql =  WP_OFF();
    RtlCopyMemory(Mapped, memOp->Buffer, memOp->Size);
    KeLowerIrql(irql);
    // ����Ŀ����̵�������
    KeUnstackDetachProcess(&kapcState);
    MmUnmapLockedPages((PVOID)Mapped, g_pdml);
    IoFreeMdl(g_pdml);

    return status;
}

NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    DbgPrint("lsx:����DispatchDeviceControl����\n");
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    ULONG controlCode = irpStack->Parameters.DeviceIoControl.IoControlCode;

    PMEMORY_OPERATION memOp = (PMEMORY_OPERATION)Irp->AssociatedIrp.SystemBuffer;
    NTSTATUS status = STATUS_SUCCESS;
    PEPROCESS targetProcess;

    __try {
        status = PsLookupProcessByProcessId((HANDLE)memOp->ProcessId, &targetProcess);
        if (!NT_SUCCESS(status)) {
            __leave;
        }

        switch (memOp->Type) {
        case DATA_TYPE_BINARY:
        case DATA_TYPE_BYTE:
        case DATA_TYPE_WORD:
        case DATA_TYPE_DWORD:
        case DATA_TYPE_FLOAT:
        case DATA_TYPE_DOUBLE:
        case DATA_TYPE_STRING:
        case DATA_TYPE_BYTE_ARRAY:
            if (controlCode == IOCTL_MEMORY_READ) {
                status = ReadMemory(targetProcess, memOp);
                //status = KReadProcessMemory(targetProcess, memOp);
            }
            else if (controlCode == IOCTL_MEMORY_WRITE) {
                //status = WriteMemory(targetProcess, memOp);
                //status = KWriteProcessMemory(targetProcess, memOp);
                //status = KWriteProcessMemory2(targetProcess, memOp);
                //status = WriteMemoryOnlyRead(targetProcess, memOp);
                //status = KWriteProcessMemory1(targetProcess, memOp);
                status = WriteMemoryByMdl(targetProcess, memOp);
            }
            else {
                status = STATUS_INVALID_DEVICE_REQUEST;
            }
            break;
        default:
            status = STATUS_INVALID_PARAMETER;
            break;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        status = GetExceptionCode();
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = NT_SUCCESS(status) ? sizeof(MEMORY_OPERATION) : 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return status;
}