#include"pch.h"
#include <tlhelp32.h>
#include"process.h"
DWORD GetPIDByName(const TCHAR* processName) {
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	DWORD pid = 0;

	// �����ʼ��
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// ��ȡ���н��̵Ŀ���
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		// ���ҵ�һ������
		if (Process32First(hSnapshot, &pe32)) {
			// �������н���
			do {
				// �ȽϽ�����
				if (_tcscmp(pe32.szExeFile, processName) == 0) {
					pid = pe32.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	return pid;
}