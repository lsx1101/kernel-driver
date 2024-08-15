#include<ntifs.h>
#include "protect.h"


static UINT32 processPids[256] = { 0 };
// ��ӱ���PID
void addPid(UINT32 pid) {
	for (size_t i = 0; i < 256; i++)
	{
		if (processPids[i] == 0 || processPids[i] == pid) {
			processPids[i] = pid;
			break;
		}
	}
}
// ��ӱ���PID
void delPid(UINT32 pid) {
	for (size_t i = 0; i < 256; i++)
	{
		if (processPids[i] == pid) {
			processPids[i] = 0;
			break;
		}
	}
}
// ��ӱ���PID
void clearPid() {
	memset(processPids, 0, sizeof(processPids));
}
// �ж�PID�Ƿ񱻱���
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
		//�ں˴���
	}
	else {
		//�û���
		// ��ȡ��������PID
		HANDLE pid = PsGetProcessId((PEPROCESS)infomation->Object); 
		if (isProtect(pid) == 0)
		{
			return OB_PREOP_SUCCESS;
		}

		ACCESS_MASK oldAuthority = infomation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		ACCESS_MASK newAuthority = infomation->Parameters->CreateHandleInformation.DesiredAccess;
		// ��ֹ��������
		oldAuthority &= ~PROCESS_TERMIINATE;
		// ��ֹ������
		oldAuthority &= ~PEOCESS_VM_READ;
		// ��ֹд����
		oldAuthority &= ~PEOCESS_VM_WRITE;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = oldAuthority;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		DbgPrint("lsx:����Ȩ���޸� ����ID=%p ��Ȩ��=%x ��Ȩ��=%x", pid, oldAuthority, newAuthority);
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
	DbgPrint("lsx:��װ���̱��� callbackHanlde=%p", callbackHanlde);
}

void uninstallProcessProtect() {
	if (callbackHanlde != NULL)
	{
		ObUnRegisterCallbacks(callbackHanlde);
		callbackHanlde = NULL;
		DbgPrint("lsx:ж�ؽ��̱���");
	}
}