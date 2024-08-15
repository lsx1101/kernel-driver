#include <ntddk.h>
#include"driver.h"

// 编程方式绕过签名检查
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

/*函数声明-开始*/

/*IRP请求函数*/
NTSTATUS IRPHandle(PDEVICE_OBJECT driver, PIRP irp);
/*驱动设备创建函数*/
NTSTATUS CreateDevice(PDRIVER_OBJECT driver);
/*驱动设备删除函数*/
void DeleteDevice(PDRIVER_OBJECT driver);

/*函数声明-结束*/

/*驱动卸载函数*/
VOID DriverUnload(_In_ PDRIVER_OBJECT driver) {
    // 卸载驱动的清理代码
    DbgPrint("lsx:进入驱动卸载函数");
    uninstallProcessProtect();
    DeleteDevice(driver);
}
/*驱动入口函数*/
NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT driver,
    _In_ PUNICODE_STRING registryPath
) {
    DbgPrint("lsx:进入驱动入口函数");
    BypassCheckSign(driver);
    // 初始化代码
    driver->DriverUnload = DriverUnload;
    // 为驱动程序的几个关键IRP处理例程分配处理函数
    // IRP_MJ_CREATE：处理设备打开请求
    driver->MajorFunction[IRP_MJ_CREATE] = IRPHandle;
    // IRP_MJ_CLOSE：处理设备关闭请求
    driver->MajorFunction[IRP_MJ_CLOSE] = IRPHandle;
    // IRP_MJ_DEVICE_CONTROL：处理设备控制请求，通常用于执行特定于设备的操作
    driver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRPHandle;
    // 调用CreateDevice函数创建设备对象，并将其与驱动程序关联
    // 这是驱动程序初始化的重要步骤，允许用户模式应用程序通过设备名访问驱动程序提供的服务
    CreateDevice(driver);

    installProcessProtect();
    return STATUS_SUCCESS;
}