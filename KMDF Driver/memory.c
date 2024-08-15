#include<ntifs.h>
#include "memory.h"

BOOLEAN KReadProcessMemory(
	IN PEPROCESS process,
	IN PVOID address,
	IN UINT32 length,
	IN OUT PVOID buffer
){
	KAPC_STATE kapcState;
	RtlZeroMemory(&kapcState, sizeof(KAPC_STATE));

	PVOID tempBufKernel = ExAllocatePool(NonPagedPool, length);

	KeStackAttachProcess((PVOID)process, &kapcState);
	BOOLEAN ret = MmIsAddressValid(address);
	if (ret)
	{
		RtlCopyMemory(tempBufKernel, address, length);

	}
	KeUnstackDetachProcess(&kapcState);
	RtlCompareMemory(buffer, tempBufKernel, length);
	ExFreePool(tempBufKernel);
	return ret;
}

int ReadProcessMemoryForPid(
	UINT32 pid,
	PVOID pBase,
	PVOID buffer,
	UINT32 size
) {
	PEPROCESS selectProcess = NULL;
	if (PsLookupProcessByProcessId((PVOID)(UINT_PTR)(pid),&selectProcess) == STATUS_SUCCESS)
	{
		BOOLEAN ret = KReadProcessMemory(selectProcess, (PVOID)pBase, size, buffer);
		if (ret)
		{
			return size;
		}
	}
	return STATUS_SUCCESS;
}