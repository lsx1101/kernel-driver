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
    DbgPrint("lsx:读内存 pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
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
            DbgPrint("lsx:MmCopyVirtualMemory执行失败 GetExceptionCode=\n");
        }
        DbgPrint("lsx:读内存 status=%u Buffer=%u\n", status, memOp->Buffer);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:读内存失败 GetExceptionCode=%u\n", status, GetExceptionCode());
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS WriteMemory(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:写内存 pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
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
            DbgPrint("lsx:MmCopyVirtualMemory执行失败 GetExceptionCode=\n");
        }


    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:写内存失败 GetExceptionCode=%u\n", GetExceptionCode());
        status = GetExceptionCode();
    }

    return status;
}

NTSTATUS WriteMemoryByMdl(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    DbgPrint("lsx:MDL写内存 pid=%u address=%p buffer=%d size=%u\n", 
        memOp->ProcessId,
        memOp->Address,
        *(BYTE*)memOp->Buffer,
        memOp->Size);
    NTSTATUS status = STATUS_SUCCESS;
    // 获取写入数据
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
    // 附加到目标进程
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
    DbgPrint("lsx:写内存 pid=%u address=%p\n", memOp->ProcessId, memOp->Address);
    NTSTATUS status = STATUS_SUCCESS;
    SIZE_T bytesTransferred = 0;
    HANDLE hTargetProcess = NULL;
    ULONG oldProtect;
    SIZE_T returnSize;

    // 声明一个布尔变量，用于跟踪资源分配是否成功
    BOOLEAN resourceAllocated = FALSE;

    __try {
        // 从PEPROCESS获取用户模式进程句柄
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
            DbgPrint("lsx:ZwQueryVirtualMemory执行失败 status=%u\n", status);
            // 返回错误代码并清理已分配的资源
            return status;
        }
        // 标记资源分配成功
        resourceAllocated = TRUE;

        // 获取目标内存页面的当前保护属性
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
            DbgPrint("lsx:ZwQueryVirtualMemory执行失败 status=%u\n", status);
            // 返回错误代码并清理已分配的资源
            return status;
        }

        // 修改内存页面保护属性为可写
        status = ZwProtectVirtualMemory(
            hTargetProcess,
            (PVOID)memOp->Address,
            &memOp->Size,
            PAGE_READWRITE,
            &oldProtect
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwProtectVirtualMemory执行失败 status=%u\n", status);
            // 返回错误代码并清理已分配的资源
            return status;
        }

        // 使用MmCopyVirtualMemory进行跨进程内存拷贝
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
            DbgPrint("lsx:MmCopyVirtualMemory执行失败 status=%u\n", status);
            // 返回错误代码并清理已分配的资源
            return status;
        }

        // 恢复内存页面的原始保护属性
        status = ZwProtectVirtualMemory(
            hTargetProcess,
            (PVOID)memOp->Address,
            &memOp->Size,
            oldProtect,
            &oldProtect
        );
        if (!NT_SUCCESS(status)) {
            DbgPrint("lsx:ZwProtectVirtualMemory执行失败 status=%u\n", status);
            // 返回错误代码并清理已分配的资源
            return status;
        }

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:写内存失败 GetExceptionCode=%u\n", GetExceptionCode());
        // 返回异常代码并清理已分配的资源
        status = GetExceptionCode();
    }

    // 在所有的返回路径上都进行资源清理
    if (resourceAllocated) {
        ZwClose(hTargetProcess);
    }

    return status;
}


NTSTATUS WriteMemoryOld(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    PMDL mdl = IoAllocateMdl((PVOID)memOp->Address, (ULONG)memOp->Size, FALSE, FALSE, NULL);
    if (!mdl) {
        DbgPrint("lsx:IoAllocateMdl执行失败\n");
        ObDereferenceObject(targetProcess);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    __try {
        MmProbeAndLockPages(mdl, KernelMode, IoWriteAccess);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        DbgPrint("lsx:MmProbeAndLockPages执行失败 GetExceptionCode=%u\n", GetExceptionCode());
        IoFreeMdl(mdl);
        ObDereferenceObject(targetProcess);
        return GetExceptionCode();
    }

    PVOID mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
    if (!mappedAddress) {
        DbgPrint("lsx:MmMapLockedPagesSpecifyCache执行失败\n");
        MmUnlockPages(mdl);
        IoFreeMdl(mdl);
        ObDereferenceObject(targetProcess);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    status = MmProtectMdlSystemAddress(mdl, PAGE_READWRITE);
    if (!NT_SUCCESS(status)) {
        DbgPrint("lsx:MmProtectMdlSystemAddress执行失败\n");
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
    DbgPrint("lsx:KWriteProcessMemory执行 pid=%u address=%p buffer=%u\n", memOp->ProcessId, memOp->Address, memOp->Buffer);
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
    // 检查目标进程是否有效
    BOOLEAN ret = MmIsAddressValid((PVOID)memOp->Address);
    if (ret)
    {
        DbgPrint("lsx:MmIsAddressValid执行\n");
        // 写入数据到目标进程内存
        RtlCopyMemory((PVOID)memOp->Address, Mapped, (UINT32)memOp->Size);
    }
    else {
        DbgPrint("lsx:MmIsAddressValid执行错误\n");
        status = STATUS_INVALID_PARAMETER;
    }
    // 分离目标进程的上下文
    KeUnstackDetachProcess(&kapcState);
    MmUnmapLockedPages((PVOID)Mapped, g_pdml);
    IoFreeMdl(g_pdml);

    return status;
}

NTSTATUS KWriteProcessMemory2(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE KapcState;
    PMDL mdl = NULL;

    DbgPrint("lsx:KWriteProcessMemoryOptimized执行 pid=%u address=%p buffer=%u\n",
        memOp->ProcessId, memOp->Address, *(unsigned char*)memOp->Buffer);

    // 附加到目标进程上下文
    KeStackAttachProcess((PVOID)targetProcess, &KapcState);

    // 检查目标地址有效性
    if (!MmIsAddressValid((PVOID)memOp->Address))
    {
        DbgPrint("lsx:MmIsAddressValid执行错误\n");
        status = STATUS_INVALID_ADDRESS;
        goto Cleanup;
    }

    // 分配并初始化MDL
    mdl = IoAllocateMdl((PVOID)memOp->Address, (UINT32)memOp->Size, FALSE, FALSE, NULL);
    if (!mdl)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    MmBuildMdlForNonPagedPool(mdl);
    // 映射内存，指定写入缓存类型以确保正确写入
    PVOID mappedAddress = NULL;
    mappedAddress = MmMapLockedPagesSpecifyCache(mdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
    if (!mappedAddress)
    {
        status = STATUS_MEMORY_NOT_ALLOCATED;
        goto FreeMdl;
    }

    // 执行写入操作
    RtlCopyMemory(mappedAddress, memOp->Buffer, (UINT32)memOp->Size);

    // 解除映射
    MmUnmapLockedPages(mappedAddress, mdl);
Cleanup:
    // 分离进程上下文
    KeUnstackDetachProcess(&KapcState);

FreeMdl:
    if (mdl)
    {
        // 释放MDL
        IoFreeMdl(mdl);
    }

    return status;
}
NTSTATUS KWriteProcessMemory1(PEPROCESS targetProcess, PMEMORY_OPERATION memOp) {
    NTSTATUS status = STATUS_SUCCESS;
    KAPC_STATE kapcState;
    RtlZeroMemory(&kapcState, sizeof(KAPC_STATE));
    DbgPrint("lsx:KWriteProcessMemory1执行 pid=%u address=%p buffer=%u\n", memOp->ProcessId, memOp->Address, *(unsigned char*)memOp->Buffer);
    KeStackAttachProcess((PVOID)targetProcess, &kapcState);
    // 检查目标进程是否有效
    BOOLEAN ret = MmIsAddressValid((PVOID)memOp->Address);
    if (!ret)
    {
        DbgPrint("lsx:MmIsAddressValid执行错误\n");
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

    DbgPrint("lsx:MmIsAddressValid执行\n");
    // 写入数据到目标进程内存
     // 关闭写保护
    KIRQL irql =  WP_OFF();
    RtlCopyMemory(Mapped, memOp->Buffer, memOp->Size);
    KeLowerIrql(irql);
    // 分离目标进程的上下文
    KeUnstackDetachProcess(&kapcState);
    MmUnmapLockedPages((PVOID)Mapped, g_pdml);
    IoFreeMdl(g_pdml);

    return status;
}

NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    DbgPrint("lsx:进入DispatchDeviceControl函数\n");
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