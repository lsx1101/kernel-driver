#include "pch.h"
#include<winsvc.h>
#include <string>
#include <cstring>
#include "driver.h"

BOOL LoadDriver(CString ServiceName, CString SysFilePath) {

	char buf[2048] = { 0 };
	sprintf_s(buf, "lsx:加载驱动文件->服务名=%ws 文件路径=%ws\n", ServiceName.GetString(), SysFilePath.GetString());
	OutputDebugStringA(buf);
	// 获取驱动文件路径
	/*char driverPath[256] = {0};
	GetFullPathNameA(driverName, 256, driverPath, NULL);*/
	
	// 打开驱动管理器
	SC_HANDLE serviceMgrHandle = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	sprintf_s(buf, "lsx:打开驱动管理器 句柄=%p GetLastError=%d\n", serviceMgrHandle, GetLastError());
	OutputDebugStringA(buf);
	// 创建驱动服务
	SC_HANDLE serviceHandle = CreateServiceW(
		serviceMgrHandle,
		ServiceName.GetBuffer(0),
		ServiceName.GetBuffer(0),
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL,
		SysFilePath.GetBuffer(0),
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
	sprintf_s(buf, "lsx:创建驱动服务 句柄=%p GetLastError=%d\n", serviceHandle, GetLastError());
	OutputDebugStringA(buf);
	if (GetLastError() == ERROR_SERVICE_EXISTS)
	{
		serviceHandle = OpenServiceW(serviceMgrHandle, ServiceName.GetBuffer(0), SERVICE_START);
	}
	BOOL ret = StartServiceW(serviceHandle, NULL, NULL);
	if (serviceHandle)
	{
		CloseServiceHandle(serviceHandle);
	}
	if (serviceMgrHandle)
	{
		CloseServiceHandle(serviceMgrHandle);
	}
	return ret;
}

BOOL UnloadDriver(CString serviceName) {
	char buf[2048] = { 0 };
	BOOL ret = FALSE;
	SC_HANDLE serviceMgrHandle = NULL;
	SC_HANDLE serviceHandle = NULL;
	SERVICE_STATUS serviceStatus;
	// 创建驱动服务
	serviceMgrHandle = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (serviceMgrHandle == NULL)
	{
		sprintf_s(buf, "lsx:卸载管理器打开失败 驱动管理器句柄=%p GetLastError=%d\n", serviceMgrHandle, GetLastError());
		OutputDebugStringA(buf);
		ret = FALSE;
		goto BeforeLeave;
	}
	serviceHandle = OpenServiceW(serviceMgrHandle, serviceName.GetBuffer(0), SERVICE_ALL_ACCESS);
	if (serviceHandle == NULL)
	{
		sprintf_s(buf, "lsx:驱动服务打开失败 驱动服务句柄=%p GetLastError=%d\n", serviceHandle, GetLastError());
		OutputDebugStringA(buf);
		ret = FALSE;
		goto BeforeLeave;
	}
	if (!ControlService(serviceHandle, SERVICE_CONTROL_STOP, &serviceStatus)) {
		sprintf_s(buf, "lsx:驱动服务停止失败 GetLastError=%d\n", GetLastError());
		OutputDebugStringA(buf);
		ret = FALSE;
		goto BeforeLeave;
	}
	if (!DeleteService(serviceHandle)) {
		sprintf_s(buf, "lsx:驱动卸载失败 GetLastError=%d\n", GetLastError());
		OutputDebugStringA(buf);
		ret = FALSE;
		goto BeforeLeave;
	}
	sprintf_s(buf, "lsx:驱动卸载成功\n");
	OutputDebugStringA(buf);
	ret = TRUE;
BeforeLeave:
	if (serviceHandle)
	{
		CloseServiceHandle(serviceHandle);
	}
	if (serviceMgrHandle)
	{
		CloseServiceHandle(serviceMgrHandle);
	}
	return ret;
}




BOOL LoadDriverNew(LPCWSTR driverName, LPCWSTR driverPath) {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		ShowErrorMsg(_T("打开服务控制管理器失败"));
		return FALSE;
	}

	SC_HANDLE hService = CreateService(
		hSCManager,
		driverName,
		driverName,
		SERVICE_START | DELETE | SERVICE_STOP,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		driverPath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (hService == NULL) {
		if (GetLastError() == ERROR_SERVICE_EXISTS) {
			hService = OpenService(hSCManager, driverName, SERVICE_ALL_ACCESS);
			if (hService == NULL) {
				ShowErrorMsg(_T("打开服务失败"));
				CloseServiceHandle(hSCManager);
				return FALSE;
			}
		}
		else {
			ShowErrorMsg(_T("创建服务失败"));
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
	}

	BOOL result = StartService(hService, 0, NULL);
	if (!result && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING) {
		ShowErrorMsg(_T("启动服务失败"));
		DeleteService(hService);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}


BOOL UnloadDriverNew(LPCWSTR driverName) {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		ShowErrorMsg(_T("OpenSCManager failed"));
		return FALSE;
	}

	SC_HANDLE hService = OpenService(hSCManager, driverName, SERVICE_ALL_ACCESS);
	if (hService == NULL) {
		ShowErrorMsg(_T("OpenService failed"));
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	SERVICE_STATUS serviceStatus;
	ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus);

	BOOL result = DeleteService(hService);
	if (!result) {
		ShowErrorMsg(_T("DeleteService failed"));
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return result;
}

void ShowErrorMsg(LPCWSTR errorMsg) {
	DWORD errorCode = GetLastError();
	CString msg;
	msg.Format(_T("%s GetLastError=%lu"), errorMsg, errorCode);
	AfxMessageBox(msg);
}


BOOL LoadDriver1(LPCWSTR driverName, LPCWSTR driverPath) {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		ShowErrorMsg(_T("打开服务控制管理器失败"));
		return FALSE;
	}

	SC_HANDLE hService = CreateService(
		hSCManager,
		driverName,
		driverName,
		SERVICE_START | DELETE | SERVICE_STOP,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		driverPath,
		NULL, NULL, NULL, NULL, NULL
	);

	if (hService == NULL) {
		DWORD errorCode = GetLastError();
		if (errorCode == ERROR_SERVICE_EXISTS) {
			hService = OpenService(hSCManager, driverName, SERVICE_ALL_ACCESS);
			if (hService == NULL) {
				ShowErrorMsg(_T("打开服务失败"));
				CloseServiceHandle(hSCManager);
				return FALSE;
			}
		}
		else if (errorCode == ERROR_SERVICE_MARKED_FOR_DELETE) {
			ShowErrorMsg(_T("服务已被标记为删除，请重新启动系统后重试"));
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		else {
			ShowErrorMsg(_T("创建服务失败"));
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
	}

	BOOL result = StartService(hService, 0, NULL);
	if (!result && GetLastError() != ERROR_SERVICE_ALREADY_RUNNING) {
		ShowErrorMsg(_T("启动服务失败"));
		DeleteService(hService);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}

BOOL UnloadDriver1(LPCWSTR driverName) {
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hSCManager == NULL) {
		ShowErrorMsg(_T("打开服务控制管理器失败"));
		return FALSE;
	}

	SC_HANDLE hService = OpenService(hSCManager, driverName, SERVICE_ALL_ACCESS);
	if (hService == NULL) {
		ShowErrorMsg(_T("打开服务失败"));
		CloseServiceHandle(hSCManager);
		return FALSE;
	}

	// 停止服务
	SERVICE_STATUS_PROCESS ssp;
	DWORD bytesNeeded;
	if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded)) {
		if (ssp.dwCurrentState != SERVICE_STOPPED) {
			ControlService(hService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp);
			Sleep(1000); // 等待服务停止
			QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &bytesNeeded);
			if (ssp.dwCurrentState != SERVICE_STOPPED) {
				ShowErrorMsg(_T("停止服务失败"));
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return FALSE;
			}
		}
	}
	else {
		ShowErrorMsg(_T("查询服务状态失败"));
	}

	// 删除服务
	/*BOOL result = DeleteService(hService);
	if (!result) {
		ShowErrorMsg(_T("删除服务失败"));
	}*/
	CloseServiceHandle(hService);
	CloseServiceHandle(hSCManager);

	return TRUE;
}




