import random
import time
import math
from graph import *
from IO import *


class Master(object):
    def __init__(self, rank, size, comm):
        self.rank = rank
        self.size = size
        self.Global_Convergence = False
        self.graph = {}
        self.comm = comm
        self.global_status = [False]*size
        self.told = [False]*size
        self.All_told = False

    def Run(self):
        while(not self.Global_Convergence or not self.All_told):
            for i in range(self.size):
                Request = self.comm.irecv(source = i)
                time.sleep(0.1)
                Request = Request.wait()
                if Request != None:
                    #if Request[-1] == True:
                    #    self.global_status[i] = True
                    del Request[-1]
                    Response = {}
                    for key, val in Request.items():
                        if key in self.graph:
                            self.graph[key] = min(self.graph[key],val)
                            Response[key] = self.graph[key]
                        else:
                            self.graph[key] = val
                            Response[key] = self.graph[key]
                    Response[-1] = self.Global_Convergence
                    self.comm.send(Response, dest = i)
                    if Response[-1] == True:
                        self.told[i]=True
            self.Global_Convergence = True
            self.All_told = True
            for i in range(self.size):
                if self.global_status[i] == False:
                    self.Global_Convergence = False
                    break 
            for i in range(self.size):
                if self.told[i] == False:
                    self.All_told = False
                    break 
            #print("Master", self.graph)
            print("Master",max(self.graph.values()))
