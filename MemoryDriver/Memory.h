#pragma once

#define IOCTL_MEMORY_READ CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_MEMORY_WRITE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef enum _MEMORY_DATA_TYPE {
    DATA_TYPE_BINARY,
    DATA_TYPE_BYTE,
    DATA_TYPE_WORD,
    DATA_TYPE_DWORD,
    DATA_TYPE_FLOAT,
    DATA_TYPE_DOUBLE,
    DATA_TYPE_STRING,
    DATA_TYPE_BYTE_ARRAY
} MEMORY_DATA_TYPE;

typedef struct _MEMORY_OPERATION {
    ULONG_PTR Address;      // Memory address to read/write
    PVOID Buffer;           // Buffer to store read data or data to write
    SIZE_T Size;            // Size of data to read/write
    ULONG ProcessId;        // Target process ID
    MEMORY_DATA_TYPE Type;  // Type of data
} MEMORY_OPERATION, * PMEMORY_OPERATION;

NTKERNELAPI CHAR* PsGetProcessImageFileName(PEPROCESS Process);
NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);

NTSTATUS DispatchDeviceControl(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);

KIRQL WP_OFF();
UINT64 WP_ON(KIRQL irql);

