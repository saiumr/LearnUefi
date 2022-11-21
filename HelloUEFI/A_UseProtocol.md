# UEFI Development Note

## 使用protocol

1. 打开Protocol  
2. 使用Protocol提供的服务  
3. 关闭Protocol  

Boot Service中的Protocol服务如下  

| 服务                      | 解释                                         |
|:----------------------- |:------------------------------------------ |
| OpenProtocol            | 打开Protocol                                 |
| HandleProtocol          | 打开Protocol，OpenProtocol的简化版                |
| LocateProtocol          | 找出系统中指定Protocol的第一个实例                      |
| LocateHandleBuffer      | 找出支持指定Protocol的所有Handle，系统负责分配内存，调用者负责释放内存 |
| LocateHandle            | 找出支持指定Protocol的所有Handle，调用者负责分配和释放内存       |
| OpenProtocolInformation | 返回指定Protocol的打开信息                          |
| ProtocolsPerHandle      | 找出指定Handle上安装的所有Protocol                   |
| CloseProtocol           | 关闭Protocol（需要AgentHandle）                  |



### 打开Protocol

```c
// 需要提供Protocol的Handle
// 需要AgentHandle接管（由AgentHandle打开）
// 如果是驱动程序AngentHandle是驱动绑定Handle，另外还需要ControllerHandle  
// Attributes
gBS->OpenProtocol(...);  

typedef
EFI_STATUS
(EFIAPI *EFI_OPEN_PROTOCOL)(
  IN  EFI_HANDLE                Handle,
  IN  EFI_GUID                  *Protocol,
  OUT VOID                      **Interface  OPTIONAL,
  IN  EFI_HANDLE                AgentHandle,
  IN  EFI_HANDLE                ControllerHandle,
  IN  UINT32                    Attributes
  );

// 只关心提供Protocol的Handle
gBS->HandleProtocol(...);  

typedef
EFI_STATUS
(EFIAPI *EFI_HANDLE_PROTOCOL)(
  IN  EFI_HANDLE               Handle,
  IN  EFI_GUID                 *Protocol,
  OUT VOID                     **Interface
  );  

// 只想找到Protocol（系统中只有一个或者只想找到第一个） 
// 不关心是什么设备或者说只有我想要的那个设备有Protocol，就不用Handle了
gBS->LocateProtocol(...);

typedef
EFI_STATUS
(EFIAPI *EFI_LOCATE_PROTOCOL)(
  IN  EFI_GUID  *Protocol,
  IN  VOID      *Registration  OPTIONAL,
  OUT VOID      **Interface
  );
```

### 使用Protocol

```C
EFI_STATUS Status;
EFI_HANDLE ImageHandle;
IN EFI_HANDLE ControllerHandle;  // 设备或控制器Handle  
EFI_XXX_PROTOCOL xxxProtocolInterface;
extern EFI_GUID gEfixxxProtocolGuid;
EFI_DRIVER_BINDING_PROTOCOL *This;

// Application
Status = gBS->OpenProtocol (
    ControllerHandle,  
    &gEfixxxProtocolGuid,  
    &xxxProtocolInterface,
    ImageHandle,  
    NULL, 
    Attribute
);  

// Driver
Status = gBS->OpenProtocol (
    ControllerHandle,  
    &gEfixxxProtocolGuid,  
    &xxxProtocolInterface,
    This->DriverBindingHandle,  
    ControllerHandle, 
    Attribute
);  
```

### 关闭Protocol

```C
// 关闭通过OpenProtocol(...)打开的Protocol  
// 需要Handle和AgentHandle  
gBS->CloseProtocol(...);  
```

### 其他与Protocol相关的服务

```C
// 找到支持Protocol的Handle（指定寻找的方式）  
gBS->LocateHandleBuffer(...);  
// 获得指定设备支持的所有Protocol  
gBS->ProtocolsPerHandle(...);  
// 获得指定设备上指定Protocol的打开信息  
gBS->OpenProtocolInformation(...);
```

### 记得释放Buffer

```C
gBS->FreeBuffer(...);
```