# 概述

​	<img src="C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903151724745.png" alt="image-20240903151724745" style="zoom: 80%;" />

​	ISBN-Detect是一款用MFC开发的用于识别ISBN码的程序，使用Blaser相机采集图像，而后显示识别出的图像中的ISBN码。程序主要包括三个部分：

​	1.图像采集部分，采用Blaser提供的SDK操作相机和显示图像。

​	2.图像处理部分，使用OpenCV对采集到的图像进行处理，提取出ISBN码数字区域。

​	3.字符识别部分，使用pytorch对印刷体数字数据集进行训练。

​	以下描述这三个部分的具体逻辑。

# 图像采集

​	图像采集部分要注意的点如下：

>- 由于opencv不支持usb3.0协议，采用opencv的imshow()实时显示Blaser相机采集的图像会出现卡顿、延迟等情况。可使用Blaser提供的SDK中的CPylonImageWindow类接口显示图像。
>- 使用MFC的定时器实时显示图像也会出现较为严重的延迟情况，使用线程实时显示图像则不会。

​	采集图像窗口如图。

<img src="C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903152007644.png" alt="image-20240903152007644" style="zoom:67%;" />

# 图像处理

​	通过OpenCV对上述采集到的图像进行处理，主要包括三个部分：

​	1.图像预处理。

​	2.截取条形码区域。

​	3.截取每个数字字符

## 图像预处理

​	图像预处理主要包括四个步骤：

​	1.三通道RGB图像转为单通道灰度图。

​	2.图像二值化。

​	3.使用Sobel算子求梯度图。

​	4.使用霍夫直线检测算法获取图像直线。

![image-20240903152926280](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903152926280.png)

![image-20240903154751023](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903154751023.png)

![image-20240903152838829](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903152838829.png)

![image-20240903154923501](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903154923501.png)

## 截取条形码区域

​	截取条形码区域主要包括步骤：

​	1.旋转图像至条形码垂直。

​	2.获取条形码区域的上下边界。

​	3.获取条形码区域的左右边界。

​	4.截取每个数字字符并排列。

### 旋转图像至条形码垂直

​	通过直线检测可知道每条直线的角度（直线法线和图像x轴之间的角度），距离（直线到图像原点的距离）。对这些角度求直方图，取频率最大的那个bin求平均值，可以得到去除一些非条形码直线的影响，求出条形码直线法线和图像x轴之间的角度，进而求出图像应该顺时针旋转多少度才能使得条形码直线垂直。

![image-20240903160448690](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903160448690.png)

### 获取条形码的上下边界

​	旋转图像后，可以通过图像每一行（即同一高度）的直线数来估计条形码上下边界。可以看出条形码上边界的上方直线数要远小于上边界的下方直线数，下边界的上方直线数要远大于下方直线数。因此可统计出每一行的直线数，然后计算直线数的直方图，取频率最大的那个bin的平均值作为阈值来指示条形码高度的直线数，当直线数小于k·阈值时不为条形码高度，大于k·阈值时为条形码高度。如图为判断出的条形码上下边界。

![image-20240903152848950](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903152848950.png)

### 获取条形码左右边界

​	如下图为截取条形码上下边界之间的图像区域的二值图。

![image-20240903162245418](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903162245418.png)

​	由图可以看出条形码区域的分布特点有：

​	1.条形码有黑条纹和白条纹，黑条纹为整列黑像素，白条纹为整列白像素。

​	2.条形码区域的黑条纹和白条纹的宽度相对较小。

​	3.第一个白条纹的左侧必定是黑像素区域，黑条纹的左侧必定是白条纹。

​	按照此分布特点，条形码的左右边界检测方法可按以下步骤进行：

​	1.统计图像每列的白像素点数，黑像素点数，判断该列是否为全白/全黑列。

​	2.全白/全黑列的连续行数大于阈值或不为全白/全黑列则为非条形码区域，小于阈值则为白/黑条纹。

​	3.在非条形码区域后第一个白条纹为左边界，白条纹后为非条形码区域则为右边界。

​	如图为按照上下边界和左右边界截取出的条形码区域。

![image-20240903171002640](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903171002640.png)

### 截取数字字符

​	数字区域位于条形码区域之下，且数字区域高度和条形码区域高度具有一定的比例关系，截取出的数字区域如图。

![image-20240903171337836](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903171337836.png)

​	使用OpenCV识别数字区域所有形状的轮廓，并用矩形框起来。如图，可以看出，除了数字会被框出，还有条形码部分也会被框出。这些部分具有宽度小、几乎整个矩形均为黑像素的特点，利用这些特点可以滤除条形码部分。

<img src="C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903172058542.png" alt="image-20240903172058542" style="zoom:50%;" />

​	滤除条形码部分后，将剩余的矩形框中的图像截取出来如下图。

![image-20240903153240975](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903153240975.png)

# 字符识别

​	首先使用OpenCV对截取到的印刷体数字进行处理（腐蚀、膨胀、剪裁），保存为印刷体数字数据集。使用pytorch建立机器学习判别模型，对数据集进行训练。如图为误差函数值和迭代次数之间的关系，在误差函数值小于1时停止迭代。

![image-20240903175120149](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903175120149.png)

​	将训练结果（即权重矩阵）保存为一个txt文件，在程序中读取该文件，并输入截取到的数字图像进行识别，可以得到结果如下。

![image-20240903170845334](C:\Users\llll\AppData\Roaming\Typora\typora-user-images\image-20240903170845334.png)