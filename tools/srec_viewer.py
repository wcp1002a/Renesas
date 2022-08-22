import cv2
import numpy as np
import random

DISPLAY_W = 1920
DISPLAY_H = 205
filename = 'r-car.srec'
filename = 'p_output.srec'

def parse_data(srec_string):
# parse data from srec record
# S3157C333630 FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF E5
    datas = []
    if srec_string[0] == 'S':
        rec_type = srec_string[0:2]
        address_bits = 0
        if rec_type == 'S0':
            print('skip')
        elif rec_type == 'S1':
            address_bits = 16
        elif rec_type == 'S2':
            address_bits = 24
        elif rec_type == 'S3':
            address_bits = 32
        if address_bits > 0:
            offset = 2 + 2 + int(address_bits/4) # S-type(2) + ByteCount(2) + Address fieled(n)
            data_size = int(srec_string[2:4], 16) - int(address_bits/8) - 1 # address: 4 bytes, checksum: 1 bytes
            for i in range(data_size):
                datas.append(int(srec_string[offset+i*2:offset+i*2+2], 16))
    return datas

img_datas = []
with open(filename) as file_object:
    for line in file_object:
        datas = parse_data(line)
        if len(datas) > 0:
            img_datas += datas

# print(img_datas)
print(len(img_datas))
img = []
n = 0
for i in range(len(img_datas)):
    if i % 4 != 3:
        img.append(img_datas[i])
    else:
        n = n + 1
        if(img_datas[i] != 0xFF):
            print('n={}'.format(n))
            print(i, hex(img_datas[i]))
print(len(img))
# w = 480
# img = img[w*3:] + img[0:w*3]
img = np.array(img, np.uint8)
img = np.reshape(img, (DISPLAY_H, DISPLAY_W, 3))
# img = np.reshape(img, (410, 960, 3))
cv2.imshow('img', img)
cv2.waitKey(0)

print(img.shape)

# cv2.imwrite('pp.bmp', img)