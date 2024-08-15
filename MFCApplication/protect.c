#include<ntifs.h>
#include "protect.h"

OB_PREOP_CALLBACK_STATUS
process_callback(
	PVOID RegistContext,
	POB_PRE_OPERATION_INFORMATION infomation
) {
	DbgPrint("lsx:进程保护 process=%p", infomation->Object);
	if (infomation->KernelHandle)
	{

	}
	else {
		ACCESS_MASK oldAuthority = infomation->Parameters->CreateHandleInformation.OriginalDesiredAccess;
		ACCESS_MASK newAuthority = infomation->Parameters->CreateHandleInformation.DesiredAccess;
		// 阻止结束进程
		oldAuthority &= ~PROCESS_TERMIINATE;
		// 阻止读进程
		oldAuthority &= ~PEOCESS_VM_READ;
		// 阻止写进程
		oldAuthority &= ~PEOCESS_VM_WRITE;
		infomation->Parameters->CreateHandleInformation.DesiredAccess = oldAuthority;
		DbgPrint("lsx:进程权限修改 旧权限=%x 新权限=%x", oldAuthority, newAuthority);
	}
	return OB_PREOP_SUCCESS;
}

void installProcessProtect() {
	
}

void uninstallProcessProtect() {

}