import random
import time
import os
import sys

file_name = sys.argv[1]
print (file_name)
vertex_num = int(sys.argv[2])
density = float(sys.argv[3])

F = open(file_name,'w')
F.write(str(vertex_num)+" "+"False")
for i in range(vertex_num):
    F.write(str(i))
    for j in range(vertex_num):
        ra = random.randint(0,100000)
        if(ra%100000<=density*100000):
            F.write(" "+str(j))
    F.write("\n")


for i in range(vertex_num):
    F.write(str(i)+'\n')

