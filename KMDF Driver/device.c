#include <ntddk.h>
#include"device.h"


NTSTATUS CreateDevice(PDRIVER_OBJECT driver) {
    DbgPrint("lsx:进入驱动设备创建函数");
    NTSTATUS status;
    UNICODE_STRING deviceName;
    PDEVICE_OBJECT device = NULL;

    RtlInitUnicodeString(&deviceName, DEVICE_NAME);

    status = IoCreateDevice(driver,
        sizeof(driver->DriverExtension),
        &deviceName,
        FILE_DEVICE_UNKNOWN,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &device);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("lsx:驱动设备创建失败");
        return status;
    }
    DbgPrint("lsx:驱动设备创建成功");
    UNICODE_STRING symbolicLinkName;
    RtlInitUnicodeString(&symbolicLinkName, SYMBOL_NAME);
    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("lsx:符号链接创建失败");
        IoDeleteDevice(device);
        return status;
    }
    DbgPrint("lsx:符号链接创建成功");
    DbgPrint("lsx:离开驱动设备创建函数");
    return status;
}

void DeleteDevice(PDRIVER_OBJECT driver) {
    DbgPrint("lsx:进入驱动设备删除函数");
    if (driver->DeviceObject)
    {
        UNICODE_STRING symbolicLinkName;
        RtlInitUnicodeString(&symbolicLinkName,SYMBOL_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
        DbgPrint("lsx:删除符号链接");
        IoDeleteDevice(driver->DeviceObject);
        DbgPrint("lsx:删除驱动设备");
    }
    DbgPrint("lsx:离开驱动设备删除函数");
}