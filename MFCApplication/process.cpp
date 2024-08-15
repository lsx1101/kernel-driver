#include"pch.h"
#include <tlhelp32.h>
#include"process.h"
DWORD GetPIDByName(const TCHAR* processName) {
	HANDLE hSnapshot;
	PROCESSENTRY32 pe32;
	DWORD pid = 0;

	// 必须初始化
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 获取所有进程的快照
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE) {
		// 查找第一个进程
		if (Process32First(hSnapshot, &pe32)) {
			// 遍历所有进程
			do {
				// 比较进程名
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