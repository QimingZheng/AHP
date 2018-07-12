import pandas
import math
import time
import random
from IO import *

class Graph(object):
    def __init__(self):
        self.graph = {} # vertex : neighbor_list
        self.master_vertex = {} # vertex : whether thie vertex is a master mirror copy (True/False)
        self.duplicate_vertex = {} # duplicate vertex index & master copy holder
        self.graph_edge_property = {} # vertex : neighbor_edge_property_list
        self.vertex_property = {} # vertex : vetertex_property
        return 

    def Construct(self, G):
        self.graph = G.graph
        self.master_vertex = G.master_vertex
        self.duplicate_vertex = G.duplicate_vertex
        self.graph_edge_property = G.graph_edge_property
        self.vertex_property = G.vertex_property
        return  

    def Mapping(self):
        notchanged = True
        for ver, nei in self.graph.items():
            for j in nei: #self.graph[ver]:
                #print("%d's nei %d : %f"%(ver,j,self.vertex_property[j]))
                if self.vertex_property[ver] > self.vertex_property[j]:
                    notchanged = False
                self.vertex_property[ver] = min(self.vertex_property[j],self.vertex_property[ver])
        return notchanged

    def Reducing(self, Master_mirror_property):
        for ver, master_holder in self.duplicate_vertex.items():
            if master_holder != None:
                self.vertex_property[ver] = Master_mirror_property[ver]

def main():
    my_io = FileIo("test.graph")
    my_io.GraphGeneration()
    G = Graph()
    G.Construct(my_io)
    print (G.graph)
    print (G.vertex_property)
    while(True):
        G.Mapping()
        print (G.vertex_property)
        time.sleep(2)
#main()
def asyn_main(file_name = "test.graph",rank=0,size=2):
    my_io = AsynFileIo(file_name,rank,size)
    my_io.GraphGeneration()
    G = Graph()
    G.Construct(my_io)
    return G
"""
    print (G.graph)
    print (G.vertex_property)
    while(True):
        G.Mapping()
        print (G.vertex_property)
        time.sleep(2)
asyn_main()
"""
