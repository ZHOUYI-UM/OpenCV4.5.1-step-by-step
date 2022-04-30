# att_faces
Copyright © 2002 AT&T Laboratories Cambridge.
The Database of Faces contains a set of face images taken between April 1992 and April 1994 at the lab.

# create_csv.py
## What is create_csv.py
This python file is provide by OpenCV4.5.1 contrib, the direction is D:\opencv4.5.1\opencv_contrib-4.5.1\modules\face\samples\etc in my computer.
We can use this python file to generate csv file, which will be used in face modules.
The original python file can only be used in python 2.7.
I do little change, so create_csv.py can be used in python 3.

## How to use create_csv.py
The usages is 
python create_csv.py "att_faces">at.txt

## The struct of at.txt
Underline is the context of at.txt
att_faces\s1/1.pgm;0
att_faces\s1/10.pgm;0
...
att_faces\s1/9.pgm;0
att_faces\s10/1.pgm;1
att_faces\s10/10.pgm;1
...
Use ';' to seperate the context, the number is the class id.

## How to use at.txt
This file will be used in C++.

# trainer.py
## What is trainer.py
This python file can run in python3.
You can use this file to generate the OpenCV LBPHFaceRecognizer data.

## How to use trainer.py
same as create_csv.py.
python create_csv.py "att_faces"

## train.yml
the OpenCV LBPHFaceRecognizer data.