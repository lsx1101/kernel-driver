#include <ntddk.h>
#include"device.h"


NTSTATUS CreateDevice(PDRIVER_OBJECT driver) {
    DbgPrint("lsx:���������豸��������");
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
        DbgPrint("lsx:�����豸����ʧ��");
        return status;
    }
    DbgPrint("lsx:�����豸�����ɹ�");
    UNICODE_STRING symbolicLinkName;
    RtlInitUnicodeString(&symbolicLinkName, SYMBOL_NAME);
    status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);
    if (status != STATUS_SUCCESS)
    {
        DbgPrint("lsx:�������Ӵ���ʧ��");
        IoDeleteDevice(device);
        return status;
    }
    DbgPrint("lsx:�������Ӵ����ɹ�");
    DbgPrint("lsx:�뿪�����豸��������");
    return status;
}

void DeleteDevice(PDRIVER_OBJECT driver) {
    DbgPrint("lsx:���������豸ɾ������");
    if (driver->DeviceObject)
    {
        UNICODE_STRING symbolicLinkName;
        RtlInitUnicodeString(&symbolicLinkName,SYMBOL_NAME);
        IoDeleteSymbolicLink(&symbolicLinkName);
        DbgPrint("lsx:ɾ����������");
        IoDeleteDevice(driver->DeviceObject);
        DbgPrint("lsx:ɾ�������豸");
    }
    DbgPrint("lsx:�뿪�����豸ɾ������");
}