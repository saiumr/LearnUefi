### DOCUMENTS  
[BMP文件格式](https://www.cnblogs.com/l2rf/p/5643352.html)  
[GIF文件格式1](https://www.jianshu.com/p/df52f1511cf8)  
[GIF文件格式2](https://blog.csdn.net/Swallow_he/article/details/76165202)  

### ESSENTIALS  
#### BMP  
+ 文件头(14Bytes) + 信息头(40Bytes) + [调色板] + 点阵数据  
+ 有一些数值是固定的，而且当一些位设置完值之后有些数值就确定了  
+ 1，4，8位图像才会使用调色板数据，16,24,32位图像不需要调色板数据，即调色板最多只需要256项（索引0 - 255）  
+ 图像分辨率指图像中存储的信息量，是每**英寸**图像内有多少个像素点，分辨率的单位为PPI (Pixels Per Inch)，通常叫做像素每英寸

关于最后的计算看这里：[扫描行所占的字节数计算公式](https://blog.csdn.net/cocoasprite/article/details/52793418)，简单来说就是为了把不满四字节的扫描行补满，这样每个扫描行的字节数都是4的倍数，符合Windows的规范  
> Windows规定一个扫描行所占的字节数必须是4的倍数(即以long为单位)，不足的以0填充  
一个扫描行所占的字节数计算方法:   
DataSizePerLine= (biWidth\*biBitCount+31)/8; // 一v个扫描行所占的字节数  
DataSizePerLine= DataSizePerLine/4\*4; // 字节数必须是4的倍数  
位图数据的大小(不压缩情况下):  
DataSize= DataSizePerLine* biHeight;  
  

