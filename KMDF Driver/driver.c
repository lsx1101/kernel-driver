#include <ntddk.h>
#include"driver.h"

// ��̷�ʽ�ƹ�ǩ�����
BOOLEAN BypassCheckSign(PDRIVER_OBJECT pDriverObject)
{
#ifdef _WIN64
    typedef struct _KLDR_DATA_TABLE_ENTRY
    {
        LIST_ENTRY listEntry;
        ULONG64 __Undefined1;
        ULONG64 __Undefined2;
        ULONG64 __Undefined3;
        ULONG64 NonPagedDebugInfo;
        ULONG64 DllBase;
        ULONG64 EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING path;
        UNICODE_STRING name;
        ULONG   Flags;
        USHORT  LoadCount;
        USHORT  __Undefined5;
        ULONG64 __Undefined6;
        ULONG   CheckSum;
        ULONG   __padding1;
        ULONG   TimeDateStamp;
        ULONG   __padding2;
    } KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;
#else
    typedef struct _KLDR_DATA_TABLE_ENTRY
    {
        LIST_ENTRY listEntry;
        ULONG unknown1;
        ULONG unknown2;
        ULONG unknown3;
        ULONG unknown4;
        ULONG unknown5;
        ULONG unknown6;
        ULONG unknown7;
        UNICODE_STRING path;
        UNICODE_STRING name;
        ULONG   Flags;
    } KLDR_DATA_TABLE_ENTRY, * PKLDR_DATA_TABLE_ENTRY;
#endif
    PKLDR_DATA_TABLE_ENTRY pLdrData = (PKLDR_DATA_TABLE_ENTRY)pDriverObject->DriverSection;
    pLdrData->Flags = pLdrData->Flags | 0x20;
    return TRUE;
}

/*��������-��ʼ*/

/*IRP������*/
NTSTATUS IRPHandle(PDEVICE_OBJECT driver, PIRP irp);
/*�����豸��������*/
NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
/*�����豸ɾ������*/
void DeleteDevice(PDRIVER_OBJECT driver);

/*��������-����*/

/*����ж�غ���*/
VOID DriverUnload(_In_ PDRIVER_OBJECT driver) {
    // ж���������������
    DbgPrint("lsx:��������ж�غ���");
    uninstallProcessProtect();
    DeleteDevice(driver);
}
/*������ں���*/
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driver,
    _In_ PUNICODE_STRING registryPath
) {
    DbgPrint("lsx:����������ں���");
    BypassCheckSign(driver);
    // ��ʼ������
    driver->DriverUnload = DriverUnload;
    // Ϊ��������ļ����ؼ�IRP�������̷��䴦����
    // IRP_MJ_CREATE�������豸������
    driver->MajorFunction[IRP_MJ_CREATE] = IRPHandle;
    // IRP_MJ_CLOSE�������豸�ر�����
    driver->MajorFunction[IRP_MJ_CLOSE] = IRPHandle;
    // IRP_MJ_DEVICE_CONTROL�������豸��������ͨ������ִ���ض����豸�Ĳ���
    driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRPHandle;
    // ����CreateDevice���������豸���󣬲������������������
    // �������������ʼ������Ҫ���裬�����û�ģʽӦ�ó���ͨ���豸���������������ṩ�ķ���
    CreateDevice(driver);

    installProcessProtect();
    return STATUS_SUCCESS;
}