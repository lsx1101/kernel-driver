#pragma once

BOOL LoadDriver(CString serviceName, CString driverName);
BOOL UnloadDriver(CString serviceName);

BOOL LoadDriverNew(LPCWSTR driverName, LPCWSTR driverPath);
BOOL LoadDriver1(LPCWSTR driverName, LPCWSTR driverPath);
BOOL UnloadDriverNew(LPCWSTR driverName);
BOOL UnloadDriver1(LPCWSTR driverName);
void ShowErrorMsg(LPCWSTR errorMsg);
BOOL DeleteServiceRegistryKey(LPCWSTR serviceName);