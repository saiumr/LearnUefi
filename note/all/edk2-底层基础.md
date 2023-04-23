# edk2-底层基础  

底层原理介绍见 [edk2-底层原理](./edk2-底层原理.md)  

注：在下面的介绍里，gBS和gRT只阐述了每部分的功能，并没有一一列举每个成员的功能，我将他们放到[edk2-底层原理](./edk2-底层原理.md)中了，但也不是全部，重要的我会优先记录  

## 基础结构  

在[MdeModulePkg/Core/Dxe/Hand/Handle.h](../../../MdeModulePkg/Core/Dxe/Hand/Handle.h)中有以下重要结构，其中`IHANDLE`就是`EFI_HANDLE`的结构，`EFI_HANDLE`在底层实现里是个`void *`指针，见[MdePkg/Include/Uefi/UefiBaseType.h](../../../MdePkg/Include/Uefi/UefiBaseType.h)，但被转换当做`IHANDLE`指针来用，这样就隐藏了`IHANDLE`的底层实现  

```c
///
/// IHANDLE - contains a list of protocol handles
///
typedef struct {
  UINTN               Signature;
  /// All handles list of IHANDLE
  LIST_ENTRY          AllHandles;
  /// List of PROTOCOL_INTERFACE's for this handle
  LIST_ENTRY          Protocols;      
  UINTN               LocateRequest;
  /// The Handle Database Key value when this handle was last created or modified
  UINT64              Key;
} IHANDLE;

///
/// PROTOCOL_ENTRY - each different protocol has 1 entry in the protocol
/// database.  Each handler that supports this protocol is listed, along
/// with a list of registered notifies.
///
typedef struct {
  UINTN               Signature;
  /// Link Entry inserted to mProtocolDatabase
  LIST_ENTRY          AllEntries;  
  /// ID of the protocol
  EFI_GUID            ProtocolID;  
  /// All protocol interfaces
  LIST_ENTRY          Protocols;     
  /// Registerd notification handlers
  LIST_ENTRY          Notify;                 
} PROTOCOL_ENTRY;

///
/// PROTOCOL_INTERFACE - each protocol installed on a handle is tracked
/// with a protocol interface structure
///
typedef struct {
  UINTN                       Signature;
  /// Link on IHANDLE.Protocols
  LIST_ENTRY                  Link;   
  /// Back pointer
  IHANDLE                     *Handle;  
  /// Link on PROTOCOL_ENTRY.Protocols
  LIST_ENTRY                  ByProtocol; 
  /// The protocol ID
  PROTOCOL_ENTRY              *Protocol;  
  /// The interface value
  VOID                        *Interface; 
  /// OPEN_PROTOCOL_DATA list
  LIST_ENTRY                  OpenList;       
  UINTN                       OpenListCount;

} PROTOCOL_INTERFACE;

typedef struct {
  UINTN                       Signature;
  ///Link on PROTOCOL_INTERFACE.OpenList
  LIST_ENTRY                  Link;      

  EFI_HANDLE                  AgentHandle;
  EFI_HANDLE                  ControllerHandle;
  UINT32                      Attributes;
  UINT32                      OpenCount;
} OPEN_PROTOCOL_DATA;

///
/// PROTOCOL_NOTIFY - used for each register notification for a protocol
///
typedef struct {
  UINTN               Signature;
  PROTOCOL_ENTRY      *Protocol;
  /// All notifications for this protocol
  LIST_ENTRY          Link;                   
  /// Event to notify
  EFI_EVENT           Event;    
  /// Last position notified
  LIST_ENTRY          *Position;              
} PROTOCOL_NOTIFY;
```

其中`LIST_ENTRY`是一个双向链表，上面的结构中`IHANDLE`和`PROTOCOL_INTERFACE`尤为重要  

```c
///
/// LIST_ENTRY structure definition.
///
typedef struct _LIST_ENTRY LIST_ENTRY;

///
/// _LIST_ENTRY structure definition.
///
struct _LIST_ENTRY {
  LIST_ENTRY  *ForwardLink;
  LIST_ENTRY  *BackLink;
};
```

`IHANDLE` 和 `PROTOCOL_ENTRY` 用于记录协议句柄和协议的注册。它们被封装在 `Handle Database` 中（见本文最后一节[Database](#database)），用于管理系统中所有的句柄。以下是对每个结构体的解释  

### IHANDLE

- `Signature` 是一个用于验证数据结构完整性的值。
- `AllHandles` 是双向链表，用于包含所有 `IHANDLE` 数据结构的列表。
- `Protocols` 是一个双向链表，用于包含一个句柄所支持的所有协议/接口。一个句柄可以支持多个协议接口。
- `LocateRequest` 是一个标志位，用于标记句柄是否被定位过。
- `Key` 是句柄的数据库键值，用于表示句柄最近一次被创建或修改时的状态。这个值是用于增量变化通知的。

### PROTOCOL_ENTRY

- `Signature` 是一个用于验证数据结构完整性的值。
- `AllEntries` 是双向链表，用于包含所有 `PROTOCOL_ENTRY` 数据结构的列表。
- `ProtocolID` 是一个唯一标识协议的 `EFI_GUID` 结构。
- `Protocols` 是一个双向链表，包含支持该协议的所有协议接口。
- `Notify` 是一个双向链表，包含注册了该协议的通知处理程序的列表。

### PROTOCOL_INTERFACE

- `Signature` 是一个用于验证数据结构完整性的值。
- `Link` 是指向 `IHANDLE.Protocols` 中一个协议接口的双向链表。
- `Handle` 是指向支持该协议的句柄的指针。
- `ByProtocol` 是指向支持相同协议的 `PROTOCOL_ENTRY.Protocols` 中协议接口的双向链表。
- `Protocol` 是指向 `PROTOCOL_ENTRY` 数据结构的指针。该数据结构描述了此协议的完整信息。
- `Interface` 是指向与协议关联的数据的指针。这是一个通用的指针，允许数据被存储和访问。
- `OpenList` 是一个双向链表，包含使用该协议的所有打开信息。

### OPEN_PROTOCOL_DATA

- `Signature` 是一个用于验证数据结构完整性的值。
- `Link` 是指向 `PROTOCOL_INTERFACE.OpenList` 的双向链表。
- `AgentHandle` 是打开协议者句柄。
- `ControllerHandle` 是协议所涉及的句柄。
- `Attributes` 包含有关协议打开的信息和标志。
- `OpenCount` 是该协议接口的打开计数。

### PROTOCOL_NOTIFY

- `Signature` 是一个用于验证数据结构完整性的值。
- `Protocol` 指向一个 `PROTOCOL_ENTRY` 数据结构的指针，描述了此协议的完整信息。
- `Link` 是指向 `PROTOCOL_ENTRY.Notify` 的双向链表。
- `Event` 是一个指向 `EFI_EVENT` 的指针，指示注册通知时分配的事件。
- `Position` 是表示发送通知位置的指向 `PROTOCOL_INTERFACE` 数据结构的指针。

## 基础服务  

在[MdePkg/Include/Library/UefiBootServicesTableLib.h](../../../MdePkg/Include/Library/UefiBootServicesTableLib.h)中有  

```c
///
/// Cache the Image Handle
///
extern EFI_HANDLE  gImageHandle;

///
/// Cache pointer to the EFI System Table
///
extern EFI_SYSTEM_TABLE  *gST;

///
/// Cache pointer to the EFI Boot Services Table
///
extern EFI_BOOT_SERVICES  *gBS;
```

在[MdePkg/Include/Library/UefiRuntimeServicesTableLib.h](../../../MdePkg/Include/Library/UefiRuntimeServicesTableLib.h)中有

```c
///
/// Cached copy of the EFI Runtime Services Table
///
extern EFI_RUNTIME_SERVICES  *gRT;
```

这些全局变量，它们用于通过指向系统表（system table）、引导服务（boot services）和运行时服务（runtime services）的指针（pointer）来访问UEFI（Unified Extensible Firmware Interface）的各种功能。  

`gImageHandle`是当前UEFI映像（image）的句柄（handle）[^1]，这个句柄是在启动时由UEFI分配的。程序员可以使用这个句柄来访问映像自己的信息和服务。  

[^1]: 在UEFI中，每个可执行映像（例如操作系统内核、EFI应用程序）都有一个唯一的句柄。gImageHandle变量保存的是当前正在执行的可执行映像的句柄。例如，如果你在可执行映像A中调用了一个EFI服务，那么gImageHandle就代表了A这个映像的句柄。如果在接下来的操作中切换到了可执行映像B，并调用了一个EFI服务，那么gImageHandle就会改成代表B这个映像的句柄。本质上，gImageHandle就是一个指针，指向了当前正在执行的可执行映像的相关信息  

`gST`是指向EFI系统表（system table）的指针，用于访问EFI系统中各种信息的数据结构和相关函数的指针。例如，EFI系统表包含了一些重要的UEFI数据结构，如BootServices，RuntimeServices，ConIn，ConOut，和ErrOut。  

`gBS`是指向EFI引导服务表（boot services table）的指针。引导服务包含很多UEFI系统初始化时需要用到的服务函数和数据结构。例如，gBS中有一系列的函数，如LoadImage，StartImage，和Exit，可以用来加载，启动和结束UEFI映像。  

`gRT`是指向EFI运行时服务（runtime services）表的指针。运行时服务是一些不依赖于UEFI系统启动的服务，例如打开，关闭，读取和写入文件等操作。 gRT中提供了一系列的函数，如GetTime，SetTime，和GetVariable，可以用来操作非易失性内存（NVRAM）中数据的读写，以及访问时间、描述器和内存映射等资源。  

### gST: 全局系统表  

```c
///
/// Contains a set of GUID/pointer pairs comprised of the ConfigurationTable field in the
/// EFI System Table.
///
typedef struct {
  ///
  /// The 128-bit GUID value that uniquely identifies the system configuration table.
  ///
  EFI_GUID                          VendorGuid;
  ///
  /// A pointer to the table associated with VendorGuid.
  ///
  VOID                              *VendorTable;
} EFI_CONFIGURATION_TABLE;

///
/// EFI System Table
///
typedef struct {
  ///
  /// The table header for the EFI System Table.
  ///
  EFI_TABLE_HEADER                  Hdr;
  ///
  /// A pointer to a null terminated string that identifies the vendor
  /// that produces the system firmware for the platform.
  ///
  CHAR16                            *FirmwareVendor;
  ///
  /// A firmware vendor specific value that identifies the revision
  /// of the system firmware for the platform.
  ///
  UINT32                            FirmwareRevision;
  ///
  /// The handle for the active console input device. This handle must support
  /// EFI_SIMPLE_TEXT_INPUT_PROTOCOL and EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL.
  ///
  EFI_HANDLE                        ConsoleInHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_INPUT_PROTOCOL interface that is
  /// associated with ConsoleInHandle.
  ///
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL    *ConIn;
  ///
  /// The handle for the active console output device.
  ///
  EFI_HANDLE                        ConsoleOutHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
  /// that is associated with ConsoleOutHandle.
  ///
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  ///
  /// The handle for the active standard error console device.
  /// This handle must support the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL.
  ///
  EFI_HANDLE                        StandardErrorHandle;
  ///
  /// A pointer to the EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL interface
  /// that is associated with StandardErrorHandle.
  ///
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *StdErr;
  ///
  /// A pointer to the EFI Runtime Services Table.
  ///
  EFI_RUNTIME_SERVICES              *RuntimeServices;
  ///
  /// A pointer to the EFI Boot Services Table.
  ///
  EFI_BOOT_SERVICES                 *BootServices;
  ///
  /// The number of system configuration tables in the buffer ConfigurationTable.
  ///
  UINTN                             NumberOfTableEntries;
  ///
  /// A pointer to the system configuration tables.
  /// The number of entries in the table is NumberOfTableEntries.
  ///
  EFI_CONFIGURATION_TABLE           *ConfigurationTable;
} EFI_SYSTEM_TABLE;
```

该结构体定义了用于访问 EFI 系统表的相关信息，包括：  

|Member|Explain|
|:--|:--|
|EFI_CONFIGURATION_TABLE|用于包含一组 GUID/指针对，其中包括 EFI 系统表中的 ConfigurationTable 字段。|
|VendorGuid|一个 128 位的 GUID 值，用于唯一标识系统配置表。|
|VendorTable|与 VendorGuid 相关联的表的指针。|
|EFI_TABLE_HEADER|EFI 系统表的表头。|
|FirmwareVendor|一个指向空结尾字符串的指针，用于标识为该平台生产系统固件的供应商。|
|FirmwareRevision|一个指示系统固件版本号的厂商特定值。|
|ConsoleInHandle|活动控制台输入设备的句柄。此句柄必须支持 EFI_SIMPLE_TEXT_INPUT_PROTOCOL 和 EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL。|
|ConIn|与 ConsoleInHandle 关联的 EFI_SIMPLE_TEXT_INPUT_PROTOCOL 接口的指针。|
|ConsoleOutHandle|活动控制台输出设备的句柄。|
|ConOut|与 ConsoleOutHandle 关联的 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL 接口的指针。|
|StandardErrorHandle|活动标准错误控制台设备的句柄。此句柄必须支持 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL。|
|StdErr|与 StandardErrorHandle 关联的 EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL 接口的指针。|
|RuntimeServices|指向 EFI Runtime Services 的指针。|
|BootServices|指向 EFI Boot Services 的指针。|
|NumberOfTableEntries|缓冲区 ConfigurationTable 中的系统配置表数量。|
|ConfigurationTable|指向系统配置表的指针。表中的条目数为 NumberOfTableEntries。|

### gBS：启动时服务  

来源于下面的结构体，其中一些服务也有相应的lib实现  

```c
typedef struct {
///
/// The table header for the EFI Boot Services Table.
///
EFI_TABLE_HEADER Hdr;

//
// Task Priority Services
//
EFI_RAISE_TPL RaiseTPL;
EFI_RESTORE_TPL RestoreTPL;

//
// Memory Services
//
EFI_ALLOCATE_PAGES AllocatePages;
EFI_FREE_PAGES FreePages;
EFI_GET_MEMORY_MAP GetMemoryMap;
EFI_ALLOCATE_POOL AllocatePool;
EFI_FREE_POOL FreePool;

//
// Event & Timer Services
//
EFI_CREATE_EVENT CreateEvent;
EFI_SET_TIMER SetTimer;
EFI_WAIT_FOR_EVENT WaitForEvent;
EFI_SIGNAL_EVENT SignalEvent;
EFI_CLOSE_EVENT CloseEvent;
EFI_CHECK_EVENT CheckEvent;

//
// Protocol Handler Services
//
EFI_INSTALL_PROTOCOL_INTERFACE InstallProtocolInterface;
EFI_REINSTALL_PROTOCOL_INTERFACE ReinstallProtocolInterface;
EFI_UNINSTALL_PROTOCOL_INTERFACE UninstallProtocolInterface;
EFI_HANDLE_PROTOCOL HandleProtocol;
VOID *Reserved;
EFI_REGISTER_PROTOCOL_NOTIFY RegisterProtocolNotify;
EFI_LOCATE_HANDLE LocateHandle;
EFI_LOCATE_DEVICE_PATH LocateDevicePath;
EFI_INSTALL_CONFIGURATION_TABLE InstallConfigurationTable;

//
// Image Services
//
EFI_IMAGE_LOAD LoadImage;
EFI_IMAGE_START StartImage;
EFI_EXIT Exit;
EFI_IMAGE_UNLOAD UnloadImage;
EFI_EXIT_BOOT_SERVICES ExitBootServices;

//
// Miscellaneous Services
//
EFI_GET_NEXT_MONOTONIC_COUNT GetNextMonotonicCount;
EFI_STALL Stall;
EFI_SET_WATCHDOG_TIMER SetWatchdogTimer;

//
// DriverSupport Services
//
EFI_CONNECT_CONTROLLER ConnectController;
EFI_DISCONNECT_CONTROLLER DisconnectController;

//
// Open and Close Protocol Services
//
EFI_OPEN_PROTOCOL OpenProtocol;
EFI_CLOSE_PROTOCOL CloseProtocol;
EFI_OPEN_PROTOCOL_INFORMATION OpenProtocolInformation;

//
// Library Services
//
EFI_PROTOCOLS_PER_HANDLE ProtocolsPerHandle;
EFI_LOCATE_HANDLE_BUFFER LocateHandleBuffer;
EFI_LOCATE_PROTOCOL LocateProtocol;
EFI_INSTALL_MULTIPLE_PROTOCOL_INTERFACES InstallMultipleProtocolInterfaces;
EFI_UNINSTALL_MULTIPLE_PROTOCOL_INTERFACES UninstallMultipleProtocolInterfaces;

//
// 32-bit CRC Services
//
EFI_CALCULATE_CRC32 CalculateCrc32;

//
// Miscellaneous Services
//
EFI_COPY_MEM CopyMem;
EFI_SET_MEM SetMem;
EFI_CREATE_EVENT_EX CreateEventEx;
} EFI_BOOT_SERVICES;
```

|Member Class|Explain|
|:--|:--|
|EFI_TABLE_HEADER Hdr| 结构体的第一个成员是EFI_TABLE_HEADER类型的结构体，它包含了指向gBS中其他成员的指针、gBS版本信息等表相关的信息。|
|Task Priority Services|这一部分包含了操作系统处理中断的一些服务，其中RaiseTPL用于升高当前处理器的中断屏蔽级别，RestoreTPL则是还原中断屏蔽级别。|
|Memory Services|这一部分提供了分配内存、释放内存、获取内存映射等内存管理相关的操作服务。|
|Event & Timer Services|这一部分提供了操作系统在事件队列和定时器相关的服务，包括创建事件、等待事件、激发事件等|
|Protocol Handler Services|这个部分提供了处理协议相关的服务，包括向句柄安装协议、卸载协议、注册协议通知、查找句柄以及在设备路径上查找句柄|
|Image Services|这一部分提供了操作系统启动某个EFI映像文件以及卸载、退出等相关服务|
|Miscellaneous Services|这一部分提供了一些功能服务，包括计算CRC校验值、拷贝内存、设置内存、创建事件等|
|DriverSupport Services|这一部分提供了连接/断开控制器、映射子系统等服务|
|Open and Close Protocol Services|这一部分提供了打开和关闭协议、查询协议信息等相关操作|
|Library Services|这一部分提供了操作EFI协议栈的一些高层次函数，包括定位句柄、协议、安装多协议接口等|
|32-bit CRC Services|这一部分提供了计算32位CRC校验值的服务|
|Miscellaneous Services|这一部分提供了一些高级别的内存拷贝、设置服务以及创建事件扩展（CreateEventEx）等|  

### gRT: 运行时服务  

```c
typedef struct {
///
/// The table header for the EFI Runtime Services Table.
///
EFI_TABLE_HEADER Hdr;

//
// Time Services
//
EFI_GET_TIME GetTime;
EFI_SET_TIME SetTime;
EFI_GET_WAKEUP_TIME GetWakeupTime;
EFI_SET_WAKEUP_TIME SetWakeupTime;

//
// Virtual Memory Services
//
EFI_SET_VIRTUAL_ADDRESS_MAP SetVirtualAddressMap;
EFI_CONVERT_POINTER ConvertPointer;

//
// Variable Services
//
EFI_GET_VARIABLE GetVariable;
EFI_GET_NEXT_VARIABLE_NAME GetNextVariableName;
EFI_SET_VARIABLE SetVariable;

//
// Miscellaneous Services
//
EFI_GET_NEXT_HIGH_MONO_COUNT GetNextHighMonotonicCount;
EFI_RESET_SYSTEM ResetSystem;

//
// UEFI 2.0 Capsule Services
//
EFI_UPDATE_CAPSULE UpdateCapsule;
EFI_QUERY_CAPSULE_CAPABILITIES QueryCapsuleCapabilities;

//
// Miscellaneous UEFI 2.0 Service
//
EFI_QUERY_VARIABLE_INFO QueryVariableInfo;
} EFI_RUNTIME_SERVICES;
```

|Member Class|Explain|
|:--|:--|
|Hdr|EFI_TABLE_HEADER结构体是 EDK2 中常用的一个标准结构体，包含表格的类型，版本等信息。|
|Time Services|这个部分包括获取时间、设置时间，获取唤醒时间和设置唤醒时间等时间相关的服务函数。|
|Virtual Memory Services|这个部分提供了虚拟地址到物理地址的映射、修改映像关系以及转换指针等服务。|
|Variable Services|这个部分提供了读取、写入、删除、查询系统变量及其配置信息等服务。|
|Miscellaneous Services|这个部分提供了重置系统、获取下一个高精度计数等服务。|
|UEFI 2.0 Capsule Services|这个部分提供了UEFI 2.0中胶囊式更新相关的服务，包括更新胶囊和查询胶囊序列号等服务。|
|Miscellaneous UEFI 2.0 Service|这个部分提供了查询系统变量信息或者查询支持的系统变量规格版本等服务。|

## Database  

Handle Database和Protocol Database在UEFI系统中是非常重要的概念。  

它们是两个链表，存储了所有的Handle和protocol，它们的表头在底层实现中是这样的  

```c
LIST_ENTRY      mProtocolDatabase     = INITIALIZE_LIST_HEAD_VARIABLE (mProtocolDatabase);
LIST_ENTRY      gHandleList           = INITIALIZE_LIST_HEAD_VARIABLE (gHandleList);
```

其中`INITIALIZE_LIST_HEAD_VARIABLE`宏是用来初始化双向链表的，它长这样  

```c
#define INITIALIZE_LIST_HEAD_VARIABLE(ListHead)  {&(ListHead), &(ListHead)}
```

下面介绍一下怎么通过gBS找到所有Handle和Protocol

Handle Database是一个表，用于存储所有有效的Handle。每个Handle代表一个设备或驱动程序，有时也被称为EFI_HANDLE。Handle Database是由UEFI Firmware在启动时创建和初始化的，可通过gBS的成员访问。以下是一个示例C代码，演示如何获取Handle Database中的所有Handle：  

```c
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EfiHandles.h>

EFI_STATUS GetHandles()
{
    EFI_HANDLE* HandleBuffer;
    UINTN HandleCount;
    EFI_STATUS Status;

    // Get the handle database
    Status = gBS->LocateHandleBuffer(
                AllHandles,
                NULL,
                NULL,
                &HandleCount,
                &HandleBuffer);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    // Iterate through the handle buffer and print all the handles
    for (UINTN Index = 0; Index < HandleCount; Index++)
    {
        Print(L"Handle #%d: %p\n", Index, HandleBuffer[Index]);
    }

    // Free the handle buffer
    gBS->FreePool(HandleBuffer);

    return EFI_SUCCESS;
}
```

Protocol Database是另一个表，用于存储所有已安装的Protocol。每个Protocol都有一个GUID（全局唯一标识符），用于识别Protocol。Protocol Database提供了一种机制，通过该机制，不同的驱动程序和应用程序可以通过GUID来查找和使用已安装的Protocol。同样，gBS中提供了多个函数来与Protocol Database进行交互。以下是一个示例C代码，演示如何获取某个Protocol的Handle：  

```c
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/MyCustomProtocol.h>

EFI_STATUS GetCustomProtocolHandle()
{
    EFI_HANDLE Handle;
    EFI_STATUS Status;

    // Get the handle for the MyCustomProtocol
    Status = gBS->LocateHandle(
                ByProtocol,
                &gMyCustomProtocolGuid,
                NULL,
                &Handle);

    if (EFI_ERROR(Status))
    {
        return Status;
    }

    // Use the handle to access the protocol functions
    // ...

    return EFI_SUCCESS;
}
```

上面是使用gBS进行Handle和Protocol访问的示例C代码。在实际开发中，这些函数和数据结构是非常重要的，特别是在驱动程序和应用程序中使用。
