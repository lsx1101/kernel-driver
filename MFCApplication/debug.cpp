#include "pch.h"
#include <afx.h>
#include <Windows.h>


void DebugOutput(LPCTSTR lpszFormat, ...) {
	va_list args;
	va_start(args, lpszFormat);

	CString str;
	str.FormatV(lpszFormat, args);
	OutputDebugString(str);

	va_end(args);
}

void DebugOutput(const unsigned char* byteArray, size_t length) {
    CStringA hexStr;
    for (size_t i = 0; i < length; ++i) {
        // 将每个字节转换为两位十六进制字符并追加到字符串
        hexStr.AppendFormat("\\x%02X", byteArray[i]);
    }
    // 使用CStringA确保输出为ANSI编码，适用于OutputDebugStringA
    OutputDebugStringA(hexStr);
}
