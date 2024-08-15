#pragma once

#define DEVICE_NAME L"\\DEVICE\\MyDriver"
#define SYMBOL_NAME L"\\??\\MyDriver"

NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
void DeleteDevice(PDRIVER_OBJECT driver);