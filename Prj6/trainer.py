#coding=gbk
import os
import sys
import os.path
import cv2 as cv
from PIL import Image
import numpy as np
 
def getImageAndlabels(path):
    #������������
    facesSamples = []
    #�˱�ǩ
    ids = []
    #��ȡ���е���Ƭ�����ƣ�os.listdir��ȡ��Ŀ¼���ļ������Ʒ���һ���б�os.path.join����Ŀ¼���ļ���������γ��������ļ�·����
    imagePaths = [os.path.join(path,f) for  f in os.listdir(path)]
    #����������������ע���Լ��ļ������·����Ӣ������
    face_detect = cv.CascadeClassifier('haarcascade_frontalface_default.xml')
    ##face_detect = cv.CascadeClassifier(r'D:/python 3.10.1/OpenCV/haarcascade_frontalface_default.xml')
    #ѭ����ȡ��Ƭ��������
    for imagePath in imagePaths:
        #�ûҶȵķ�ʽ����Ƭ
        PIL_img = Image.open(imagePath).convert('L')
        #����Ƭת��Ϊ�������ʶ�������OpenCV��BGR--0-255��
        img_numpy = np.array(PIL_img,'uint8')
        #��ȡͼ��������������ֵ
        faces = face_detect.detectMultiScale(img_numpy)
        #���ļ�������.�����зָ�
        id = int(os.path.split(imagePath)[1].split('.')[0])
        #��ֹ������ͼ��
        for x,y,w,h in faces:
            ids.append(id)
            facesSamples.append(img_numpy[y:y+h,x:x+w])
    return facesSamples,ids
 
if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("usage: train <base_path>")
        sys.exit(1)

    #����LBPH�㷨���������ݽ��д���
    recognizer = cv.face.LBPHFaceRecognizer_create()

    BASE_PATH=sys.argv[1]
    for dirname, dirnames, filenames in os.walk(BASE_PATH):
        for subdirname in dirnames:
            #����ͼƬ��ŵ��ļ���
            subject_path = os.path.join(dirname, subdirname)
            #print(subject_path)

            faces, ids = getImageAndlabels(subject_path)
            #ѵ������
            recognizer.train(faces, np.array(ids))

    #��ѵ����ϵͳ�������ض��ļ���
    recognizer.write('trainer.yml')
