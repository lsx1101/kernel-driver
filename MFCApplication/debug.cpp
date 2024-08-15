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
        // ��ÿ���ֽ�ת��Ϊ��λʮ�������ַ���׷�ӵ��ַ���
        hexStr.AppendFormat("\\x%02X", byteArray[i]);
    }
    // ʹ��CStringAȷ�����ΪANSI���룬������OutputDebugStringA
    OutputDebugStringA(hexStr);
}
