ISBN-Detect：采用Basler相机采集包含ISBN码的图像，输出识别结果，详情可见`ISBN-Detect说明.md`。

MatProcess：采用OpenCV开源库对输入图像进行处理，输出ISBN数字字符截取结果。

OCR-Num：采用pytorch对印刷体数字数据集DataSet用机器学习判别模型进行训练，将得到的权重矩阵输出为txt文件。


2024/9/4
ISBN-Detect需要使用到OpenCV和pylon软件套装，请自行修改配置。pylon软件套装的安装可见`pylon运行系统.md`文件
经实验ISBN-Detect在目标物体背景为黑色，相机入光量合适的时候准确度较高。
待优化方向：透明材质物体或塑料袋包装物体容易识别不到条形码。

OCR-Num需要使用到pytorch，请自行配置环境依赖。
OCR-Num所采用的印刷体数字数据集DataSet为将MatProcess输出的ISBN数字字符截取结果进行腐蚀、膨胀、截取后得到的各种采样情况下的印刷体数字。
