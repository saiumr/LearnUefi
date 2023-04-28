# PCD  

在 EDK2 中，每个 PCD 数据由四个部分组成：

- Token Space Guid
- Pcd Token
- Pcd Size
- Pcd Data

其中，**Token Space Guid** 是一个 GUID，用来唯一标识所属的 **Token Space**。Pcd Token 是一个 UINTN 类型的数值，代表该 PCD 数据在 Token Space 中的标识。Pcd Size 是一个 UINTN 类型的数值，代表该 PCD 数据的大小。Pcd Data 是一个指针，指向包含该 PCD 数据的内存区域。  

在 EDK2 中，每个 PCD 数据都有一个唯一的 Token Space Guid，用于标识该 PCD 数据所属的 Token Space。在 MdeModulePkg 中，Token Space Guid 被定义为以下变量：gEfiMdeModulePkgTokenSpaceGuid。  

当我们使用 PCD 操作函数时，例如 PcdGetXXX()，EDK2 在内部会根据传入的 Token Space Guid 和 Pcd Token，在 EDK2 PCD 数据库中查找对应的 PCD 数据。EDK2 中的 PCD 数据库实际上是一个类似散列表的结构，**根据 Token Space Guid 和 Pcd Token 计算出来的哈希值**来查找对应的 PCD 数据。  

因此，EDK2 将 gEfiMdeModulePkgTokenSpaceGuid 和一块数据区域联系起来的方法就是使用这个 Token Space Guid 标识这块数据，将数据存储在 PCD 数据库中，然后通过 PCD 操作函数来访问和修改这块数据。  
