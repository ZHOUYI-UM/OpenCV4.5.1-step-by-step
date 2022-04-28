# OpenCV4.5.1-step-by-step
从零开始学习OpenCV。环境Win10+Qt5.13.2+MinGW+OpenCV4.5.1

要特别注意代码中使用了一些绝对路径，需要根据自己 OpenCV 路径进行修改。

目前我机器上 OpenCV 路劲为 D:\opencv4.5.1\opencv-build

# Step 1: Compile OpenCV4.5.1 using MinGW in Win10
For more detail, please refer to this URL[Win10 环境下搭建 Qt5.13.2 + OpenCV4.5.1 + MinGW 开发环境] (https://blog.csdn.net/justidle/article/details/121488711)。

# Step 2: Project 1, First Qt project using OpenCV to read a picture
In this project, we will open a picture and show this picture in QMainWindow.
Using QtCreator 4.10.1 to generate a "Qt Widgets Application", use the default selection, But in "Kit Selection", please use "MinGW 64-bit" not use "MSVC2017 64bit".

# Step 3: Project 2, Using OpenCV to face and eyes detection
In this project, we will use opencv to face and eyes detection. 
Using haarcascade_frontalface_alt.xml to do face detection.
Using haarcascade_eye_tree_eyeglasses.xml to do eyes detection.
This to files are provided by OpenCV, you can get it in the direction "sources/data/haarcascades".

# Step 4: Project 3, Using OpenCV to open a camera
In this project, get vedio from camera.

# Step 5: Project 4, face detection from vedio
In this project, using a 100ms timer to detect face from vedio.

# Step 6: Project 5, collect face data from vedio
In this project, we detect a face in vedio,  and save this pic every 500ms.