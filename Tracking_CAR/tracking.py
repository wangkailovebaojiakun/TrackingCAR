import numpy as np
import cv2
import pylab
import os
import imtools
#set path
img_dir = os.getcwd()+'/image'
imlist = imtools.get_imlist(img_dir,'.jpg')
imlist.sort()
i = 0
for frame in range( 1,len(imlist)):
    img = cv2.imread(imlist[frame])
    
    cv2.imshow('img',img)
    cv2.waitKey(40)
