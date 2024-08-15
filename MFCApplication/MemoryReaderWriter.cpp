#include"pch.h"
#include<winioctl.h>
#include <tchar.h>
#include"MemoryReaderWriter.h"

CMemoryReaderWriter::CMemoryReaderWriter() : m_hDriver(INVALID_HANDLE_VALUE) {}

CMemoryReaderWriter::~CMemoryReaderWriter() {
    if (m_hDriver != INVALID_HANDLE_VALUE) {
        CloseDriver();
    }
}

BOOL CMemoryReaderWriter::OpenDriver() {
    m_hDriver = CreateFile(DEVICE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (m_hDriver == INVALID_HANDLE_VALUE) {
        _tprintf(_T("无法打开驱动程序，错误码=%lu\n"), GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL CMemoryReaderWriter::CloseDriver() {
    if (m_hDriver != INVALID_HANDLE_VALUE) {
        CloseHandle(m_hDriver);
        m_hDriver = INVALID_HANDLE_VALUE;
    }
    return TRUE;
}

BOOL CMemoryReaderWriter::ReadMemory(ULONG processId, ULONG_PTR address, LPVOID lpBuffer, SIZE_T nSize,MEMORY_DATA_TYPE dataType) {
    if (m_hDriver == INVALID_HANDLE_VALUE) {
        _tprintf(_T("驱动程序未打开\n"));
        return FALSE;
    }

    MEMORY_OPERATION memOp = { 0 };
    memOp.ProcessId = processId;
    memOp.Address = address;
    memOp.Buffer = lpBuffer;
    memOp.Size = nSize;
    memOp.Type = dataType;

    DWORD bytesReturned;
    BOOL result = DeviceIoControl(m_hDriver,
        IOCTL_MEMORY_READ,
        &memOp, sizeof(memOp),
        &memOp, sizeof(memOp),
        &bytesReturned, NULL);
    if (!result) {
        _tprintf(_T("读取内存失败，错误码=%lu\n"), GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL CMemoryReaderWriter::WriteMemory(ULONG processId, ULONG_PTR address, LPCVOID lpBuffer, SIZE_T nSize) {
    if (m_hDriver == INVALID_HANDLE_VALUE) {
        _tprintf(_T("驱动程序未打开\n"));
        return FALSE;
    }

    MEMORY_OPERATION memOp = { 0 };
    memOp.ProcessId = processId;
    memOp.Address = address;
    memOp.Buffer = (PVOID)lpBuffer;
    memOp.Size = nSize;

    DWORD bytesReturned;
    BOOL result = DeviceIoControl(m_hDriver,
        IOCTL_MEMORY_WRITE,
        &memOp, sizeof(memOp),
        &memOp, sizeof(memOp),
        &bytesReturned, NULL);
    if (!result) {
        _tprintf(_T("写入内存失败，错误码=%lu\n"), GetLastError());
        return FALSE;
    }
    return TRUE;
}

BOOL CMemoryReaderWriter::ReadByte(ULONG processId,ULONG_PTR address, BYTE& value) {
    return ReadMemory(processId, address, &value, sizeof(BYTE), DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadWord(ULONG processId,ULONG_PTR address, WORD& value) {
    return ReadMemory(processId,address, &value, sizeof(WORD), DATA_TYPE_WORD);
}

BOOL CMemoryReaderWriter::ReadDword(ULONG processId,ULONG_PTR address, DWORD& value) {
    return ReadMemory(processId,address, &value, sizeof(DWORD), DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadLong(ULONG processId, ULONG_PTR address, ULONG_PTR& value) {
    return ReadMemory(processId, address, &value, sizeof(ULONG_PTR), DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadFloat(ULONG processId,ULONG_PTR address, float& value) {
    return ReadMemory(processId,address, &value, sizeof(float), DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadDouble(ULONG processId,ULONG_PTR address, double& value) {
    return ReadMemory(processId,address, &value, sizeof(double), DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadString(ULONG processId,ULONG_PTR address, LPSTR lpBuffer, SIZE_T nSize) {
    return ReadMemory(processId,address, lpBuffer, nSize, DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::ReadByteArray(ULONG processId, ULONG_PTR address, BYTE* lpBuffer, SIZE_T nSize) {
    return ReadMemory(processId,address, lpBuffer, nSize, DATA_TYPE_BYTE);
}

BOOL CMemoryReaderWriter::WriteByte(ULONG processId,ULONG_PTR address, BYTE value) {
    return WriteMemory(processId,address, &value, sizeof(BYTE));
}

BOOL CMemoryReaderWriter::WriteWord(ULONG processId,ULONG_PTR address, WORD value) {
    return WriteMemory(processId,address, &value, sizeof(WORD));
}

BOOL CMemoryReaderWriter::WriteDword(ULONG processId,ULONG_PTR address, DWORD value) {
    return WriteMemory(processId,address, &value, sizeof(DWORD));
}

BOOL CMemoryReaderWriter::WriteFloat(ULONG processId,ULONG_PTR address, float value) {
    return WriteMemory(processId,address, &value, sizeof(float));
}

BOOL CMemoryReaderWriter::WriteDouble(ULONG processId,ULONG_PTR address, double value) {
    return WriteMemory(processId,address, &value, sizeof(double));
}

BOOL CMemoryReaderWriter::WriteString(ULONG processId,ULONG_PTR address, LPCSTR lpBuffer, SIZE_T nSize) {
    return WriteMemory(processId,address, lpBuffer, nSize);
}

BOOL CMemoryReaderWriter::WriteByteArray(ULONG processId,ULONG_PTR address, const BYTE* lpBuffer, SIZE_T nSize) {
    return WriteMemory(processId,address, lpBuffer, nSize);
}
