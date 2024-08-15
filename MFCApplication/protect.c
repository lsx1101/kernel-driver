#include<ntifs.h>
#include "protect.h"

OB_PREOP_CALLBACK_STATUS
process_callback(
	PVOID RegistContext,
	POB_PRE_OPERATION_INFORMATION infomation
) {
	DbgPrint("lsx:���̱��� process=%p", infomation->Object);
	if (infomation->KernelHandle)
	{

	}
	else {
		ACCESS_MASK oldAuthority = infomation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		ACCESS_MASK newAuthority = infomation->Parameters->CreateHandleInformation.DesiredAccess;
		// ��ֹ��������
		oldAuthority &= ~PROCESS_TERMIINATE;
		// ��ֹ������
		oldAuthority &= ~PEOCESS_VM_READ;
		// ��ֹд����
		oldAuthority &= ~PEOCESS_VM_WRITE;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = oldAuthority;
		DbgPrint("lsx:����Ȩ���޸� ��Ȩ��=%x ��Ȩ��=%x", oldAuthority, newAuthority);
	}
	return OB_PREOP_SUCCESS;
}

void installProcessProtect() {
	
}

void uninstallProcessProtect() {

}