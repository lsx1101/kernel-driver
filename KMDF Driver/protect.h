#pragma once
#define PROCESS_TERMIINATE					(0x0001)
#define PROCESS_CREATE_THREAD				(0x0002)
#define PROCESS_SET_SESSIONID				(0x0004)
#define PROCESS_VM_OPERATION				(0x0008)
#define PEOCESS_VM_READ						(0x0010)
#define PEOCESS_VM_WRITE					(0x0020)
#define PROCESS_DUP_HANDLE					(0x0040)
#define PROCESS_CREATE_PROCESS				(0x0080)
#define PEOCESS_SET_QUOTA					(0x0100)
#define PROCESS_SET_INFORMATION				(0x0200)
#define PROCESS_QUEEY_INFORMATION			(0x0400)
#define PROCESS_SUSPEND_RESUME				(0x0800)
#define PEOCESS_QUERY_LIMITED_INFORMATION	(0x1000)
#define PROCESS_SET_LIMITED_INFORMATION		(0x2000)
void installProcessProtect();
void uninstallProcessProtect();
void addPid(UINT32 pid);
void delPid(UINT32 pid);
void clearPid();
BOOLEAN isProtect(UINT32 pid);