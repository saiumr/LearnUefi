# PCI和PCIE的  

## PCI/PCIe扫盲  

> 感谢**Felix**大佬辛勤付出，感谢**LogicJitterGibbs**前辈精简总结  

最近又看到精贴，速速贴到笔记里面，帮助需要的人（首先是我自己需要），我觉得这些文章写的特别好，下面两个链接，总结篇是转载基础篇的内容的，贯穿了部分原作者的内容，推荐看了一部分基础篇的文章之后再去看总结篇（指刚刚学习的新人），至少看到第二阶段的第二篇文章  

[PCIe扫盲-基础篇](http://blog.chinaaet.com/justlxy/p/5100053251)  
[PCIe扫盲-总结篇](https://zhuanlan.zhihu.com/p/445866515)  

下面的内容可看可不看，因为看上面的文章足够让你了解到该学习什么，接下来完全可以靠自己啦~（指自己想办法去学习相关内容，不是让你独狼似的一个人闭门造车，多搜多看**多问**）  

PS：实际上笔记是写给自己的，但总是喜欢写成对话的形式（笑~）  

## 概念  

PCI配置空间和PCIe配置空间都是用来存储设备的配置信息的。它们都包含设备的硬件信息，如设备ID、供应商ID、中断线和基地址寄存器等。  

PCI配置空间是针对传统的PCI总线设计的，它的大小为256个字节。而PCIe配置空间是针对PCIe总线设计的，它的大小为4KB。  

除了大小不同，PCIe配置空间还引入了一些新的功能和改进。例如，它支持扩展配置空间，允许设备提供更多的配置信息。此外，它还支持高级错误报告和虚拟化等功能。  

## IO空间-IO端口和IO内存  

[yimuxi的博客](https://www.cnblogs.com/yi-mu-xi/p/10939735.html)  

## 以前记录的部分  

> - 在MMIO中，IO设备和内存共享同一个地址总线，因此它们的地址空间是相同的; 而在PMIO中，IO设备和内存的地址空间是隔离的。
> - 在MMIO中，无论是访问内存还是访问IO设备，都使用相同的指令； 而在PMIO中，CPU使用特殊的指令访问IO设备，在Intel微处理器中，使用的指令是IN和OUT。  
>
> 1、 IO空间有限；虽说X86处理器的IO空间可以寻址4G；但是windows系统里给外设分配的IO空间共64KB; 所以就要求单个PCI设备若使用了IO空间，其大小不要超过256字节；memory空间则没有这个限制，受限于操作系统能够管理的内存大小；  
> 2、 在驱动程序里面访问IO空间，可以直接使用IO指令访问；在驱动程序里面访问memory 空间，需要先将其物理地址(physical address)映射到虚拟地址(virtual address)，然后才能访问；  
> 3 、memory 空间访问的时候可以支持burst, IO空间访问的时候只能一个一个访问，所以从性能上看，memory空间访问的性能要高于IO空间。  
>
> IO内存空间，是指IO的内存和寄存器都通过映射到内存空间中，访问的时候直接用内存访问即可。
>
> IO地址空间，访问需要专门的I/O指令，最大为64K（目前只有X86还存在IO端口访问，为了兼容，现在已经建议使用MMIO）
>
> PCIe Spec中明确指出，IO地址空间只是为了兼容早期的PCI设备（Legacy Device），在新设计中都应当使用MMIO，因为IO地址空间可能会被新版本的PCI Spec所抛弃

[System address map initialization in x86/x64 architecture part 1: PCI-based systems - Infosec Resources](https://resources.infosecinstitute.com/topic/system-address-map-initialization-in-x86x64-architecture-part-1-pci-based-systems/)  

[Pinczakko's Guide to Award BIOS Reverse Engineering - Pinczakko Official Website (google.com)](https://sites.google.com/site/pinczakko/pinczakko-s-guide-to-award-bios-reverse-engineering#PCI_BUS)  看PCI Bus那里

[System address map initialization in x86/x64 architecture part 2: PCI express-based systems - Infosec Resources](https://resources.infosecinstitute.com/topic/system-address-map-initialization-x86x64-architecture-part-2-pci-express-based-systems/?utm_source=tuicool&utm_medium=referral)  

[访问PCIe配置空间寄存器 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/349877381#:~:text=PCI%E8%AE%BE%E5%A4%87%E4%BD%BF%E7%94%A8IO%E7%A9%BA%E9%97%B4%E7%9A%84CF8%20%28Configuration%20Address%20Port%29%2FCFC%20%28Configuration%20Data,Port%29%E5%9C%B0%E5%9D%80%E6%9D%A5%E8%AE%BF%E9%97%AE%E9%85%8D%E7%BD%AE%E7%A9%BA%E9%97%B4%E3%80%82%20PCIe%E5%B0%86%E9%85%8D%E7%BD%AE%E7%A9%BA%E9%97%B4%E6%89%A9%E5%B1%95%E5%88%B04KB%EF%BC%8C%E5%8E%9F%E6%9D%A5CF8%2FCFC%E7%9A%84%E8%AE%BF%E9%97%AE%E6%96%B9%E5%BC%8F%E4%BB%8D%E7%84%B6%E5%8F%AF%E4%BB%A5%E8%AE%BF%E9%97%AE%E6%89%80%E6%9C%89PCIe%E9%85%8D%E7%BD%AE%E7%A9%BA%E9%97%B4%E7%9A%84%E5%89%8D256Byte%2C%E6%AF%94%E5%A6%82%E6%88%91%E4%BB%AC%E6%83%B3%E8%AE%BF%E9%97%AE4%2F0%2F0%20%EF%BC%88B%2FD%2FF%20%EF%BC%89%20%E7%9A%84Vendor%20ID%EF%BC%8C%E6%88%91%E4%BB%AC%E5%88%99%E9%9C%80%E8%A6%81%3A)

[深入PCI与PCIe之一：硬件篇 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/26172972)

[深入PCI与PCIe之二：软件篇 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/26244141)

[总线特定信息](https://docs.oracle.com/cd/E19253-01/819-7057/hwovr-25520/index.html)

[[转]IO空间，IO端口，MMIO - yimuxi - 博客园 (cnblogs.com)](https://www.cnblogs.com/yi-mu-xi/p/10939735.html)

[PCIe扫盲——Memory & IO 地址空间 - 简书 (jianshu.com)](https://www.jianshu.com/p/8ca541612dd5)

[浅谈内存映射I/O(MMIO)与端口映射I/O(PMIO)的区别 - veli - 博客园 (cnblogs.com)](https://www.cnblogs.com/idorax/p/7691334.html)

[老男孩读PCIe之一：从PCIe速度说起 (ssdfans.com)](http://www.ssdfans.com/?p=8211)

[[PCI/PCIe总线] 一、 基础 - 程序员大本营 (pianshen.com)](https://www.pianshen.com/article/88961259412/)

[第六章 PCI --Linux Kernel核心中文手册 (embeddedlinux.org.cn)](http://www.embeddedlinux.org.cn/uclinuxchina/6.htm)

[PCI的IO空间和memory空间_fgupupup的博客](https://blog.csdn.net/fengxiaocheng/article/details/103258791)

[PCIE的内存地址空间、I/O地址空间和配置地址空间 - yang_xs - 博客园 (cnblogs.com)](https://www.cnblogs.com/yangxingsha/p/11551472.html)

[直接内存访问DMA](https://docs.oracle.com/cd/E19253-01/819-7057/6n91f8su6/index.html)

[PCI 设备详解一 - jack.chen - 博客园 (cnblogs.com)](https://www.cnblogs.com/ck1020/p/5942703.html)

关于PCI，看UEFI spec page725也许更好，上面列的文章头两篇讲的很深入  

规范里有很详细的解释，关于Host Bridge、Root Bridge、PCI Segment的关系，以及他们拓扑关系和设备之间共享PCI Configuration Space、PCI I/O Space、PCI Memory Space、PCI Prefetchable Memory Space的联系  

从我的理解看，PCI设备除了共享PCI Configuration Space和RB相关，其他都和HB相关  

上面提到的PCI Configuration Space应该是Type1类型（桥设备）  
