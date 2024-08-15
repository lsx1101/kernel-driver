#pragma once
#include <windows.h>

#define DEVICE_NAME L"\\??\\MemoryDriver"

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
    LPVOID Buffer;           // Buffer to store read data or data to write
    SIZE_T Size;            // Size of data to read/write
    ULONG ProcessId;        // Target process ID
    MEMORY_DATA_TYPE Type;  // Type of data
} MEMORY_OPERATION, * PMEMORY_OPERATION;

class CMemoryReaderWriter {
public:
    CMemoryReaderWriter();
    ~CMemoryReaderWriter();

    BOOL OpenDriver();
    BOOL CloseDriver();

    BOOL ReadMemory(ULONG processId,ULONG_PTR address, LPVOID lpBuffer, SIZE_T nSize, MEMORY_DATA_TYPE dataType);
    BOOL WriteMemory(ULONG processId, ULONG_PTR address, LPCVOID lpBuffer, SIZE_T nSize);

    BOOL ReadByte(ULONG processId, ULONG_PTR address, BYTE& value);
    BOOL ReadWord(ULONG processId, ULONG_PTR address, WORD& value);
    BOOL ReadDword(ULONG processId, ULONG_PTR address, DWORD& value);
    BOOL ReadLong(ULONG processId, ULONG_PTR address, ULONG_PTR& value);
    BOOL ReadFloat(ULONG processId, ULONG_PTR address, float& value);
    BOOL ReadDouble(ULONG processId, ULONG_PTR address, double& value);
    BOOL ReadString(ULONG processId, ULONG_PTR address, LPSTR lpBuffer, SIZE_T nSize);
    BOOL ReadByteArray(ULONG processId, ULONG_PTR address, BYTE* lpBuffer, SIZE_T nSize);

    BOOL WriteByte(ULONG processId, ULONG_PTR address, BYTE value);
    BOOL WriteWord(ULONG processId, ULONG_PTR address, WORD value);
    BOOL WriteDword(ULONG processId, ULONG_PTR address, DWORD value);
    BOOL WriteFloat(ULONG processId, ULONG_PTR address, float value);
    BOOL WriteDouble(ULONG processId, ULONG_PTR address, double value);
    BOOL WriteString(ULONG processId, ULONG_PTR address, LPCSTR lpBuffer, SIZE_T nSize);
    BOOL WriteByteArray(ULONG processId, ULONG_PTR address, const BYTE* lpBuffer, SIZE_T nSize);

private:
    HANDLE m_hDriver;
};
