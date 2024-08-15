#include<ntifs.h>
#include "protect.h"


static UINT32 processPids[256] = { 0 };
// 添加保护PID
void addPid(UINT32 pid) {
	for (size_t i = 0; i < 256; i++)
	{
		if (processPids[i] == 0 || processPids[i] == pid) {
			processPids[i] = pid;
			break;
		}
	}
}
// 添加保护PID
void delPid(UINT32 pid) {
	for (size_t i = 0; i < 256; i++)
	{
		if (processPids[i] == pid) {
			processPids[i] = 0;
			break;
		}
	}
}
// 添加保护PID
void clearPid() {
	memset(processPids, 0, sizeof(processPids));
}
// 判断PID是否被保护
BOOLEAN isProtect(UINT32 pid) {
	if (pid == 0)
	{
		return 0;
	}
	for (size_t i = 0; i < 256; i++)
	{
		if (processPids[i] == pid) {
			return 1;
		}
	}
	return 0;
}

OB_PREOP_CALLBACK_STATUS
process_callback(
	PVOID RegistContext,
	POB_PRE_OPERATION_INFORMATION infomation
) {
	if (infomation->KernelHandle)
	{
		//内核创建
	}
	else {
		//用户层
		// 获取保护进程PID
		HANDLE pid = PsGetProcessId((PEPROCESS)infomation->Object); 
		if (isProtect(pid) == 0)
		{
			return OB_PREOP_SUCCESS;
		}

		ACCESS_MASK oldAuthority = infomation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		ACCESS_MASK newAuthority = infomation->Parameters->CreateHandleInformation.DesiredAccess;
		// 阻止结束进程
		oldAuthority &= ~PROCESS_TERMIINATE;
		// 阻止读进程
		oldAuthority &= ~PEOCESS_VM_READ;
		// 阻止写进程
		oldAuthority &= ~PEOCESS_VM_WRITE;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = oldAuthority;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		DbgPrint("lsx:进程权限修改 进程ID=%p 旧权限=%x 新权限=%x", pid, oldAuthority, newAuthority);
	}
	return OB_PREOP_SUCCESS;
}
HANDLE callbackHanlde = NULL;
void installProcessProtect() {
	OB_CALLBACK_REGISTRATION callback = { 0 };
	OB_OPERATION_REGISTRATION operation = { 0 };
	RtlInitUnicodeString(&callback.Altitude, L"321000");
	callback.RegistrationContext = NULL;
	callback.Version = OB_FLT_REGISTRATION_VERSION;
	callback.OperationRegistrationCount = 1;
	callback.OperationRegistration = &operation;
	operation.ObjectType = PsProcessType;
	operation.Operations = OB_OPERATION_HANDLE_CREATE;
	operation.PostOperation = NULL;
	operation.PreOperation = process_callback;
	ObRegisterCallbacks(&callback, &callbackHanlde);
	DbgPrint("lsx:安装进程保护 callbackHanlde=%p", callbackHanlde);
}

void uninstallProcessProtect() {
	if (callbackHanlde != NULL)
	{
		ObUnRegisterCallbacks(callbackHanlde);
		callbackHanlde = NULL;
		DbgPrint("lsx:卸载进程保护");
	}
}