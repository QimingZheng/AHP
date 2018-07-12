import random
import time
import math
from graph import *
from IO import *

class Worker(object):
    def __init__(self, file_name, rank, size, comm):
        self.rank = rank
        self.size = size
        self.Converge = False
        self.Global_Convergence = False
        self.file_name = file_name
        self.graph = None
        self.comm = comm
    def Local_Graph(self):
        self.graph = asyn_main(self.file_name, self.rank, self.size)

    def Local_Step(self):
        while( not self.Global_Convergence):    
            self.Converge = False
            while( not self.Converge):
                self.Converge = self.graph.Mapping()
            send_message = self.graph.vertex_property
            send_message[-1] = self.Converge
            self.comm.send(send_message, dest = self.size)
            response = self.comm.recv(source = self.size)
            self.Global_Convergence = response[-1]
            del response[-1] 
            self.graph.vertex_property = response
            #print("Worker %d"%self.rank,self.graph.vertex_property,self.graph.master_vertex)
            print("Worker %d"%self.rank, max(self.graph.vertex_property.values()))


"""
def Map(graph):
    for key,val in graph.items():
        graph[key] = min(graph[key],random.randint(1,100))

def Request(graph):
    re = {}
    for key,val in graph.items():
        re[key] = val
        break
    return re
"""
