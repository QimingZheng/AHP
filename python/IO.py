import sys
import os
import time
import random
import time

class FileIo(object):
    def __init__(self, filename):
        self.filename = filename
        self.graph = {} # vertex : neighbor_list
        self.master_vertex = {} # vertex : whether thie vertex is a master mirror copy (True/False)
        self.duplicate_vertex = {} # duplicate vertex index & master copy holder
        self.graph_edge_property = {} # vertex : neighbor_edge_property_list
        self.vertex_property = {} # vertex : vertex_property

        return 

    def read(self): # ONly support graph without edge property
        graph = {}
        _property = {}
        text = open(self.filename).readlines()
        for i in range(len(text)):
            text[i] = text[i][:-1]
        Num = int(text[0].split()[0])
        if (text[0].split()[1] =='False'):
            for i in range(1,1+Num):
                tmp = text[i].split()
                for j in range(len(tmp)):
                    tmp[j]=int(tmp[j])
                graph[i-1] = tmp[1:]
            for i in range(1+Num,2*Num+1):
                _property[(i-1-Num)] = float(text[i])
        return graph, _property

    def GraphGeneration(self):
        _graph, _property = self.read()
        self.graph = _graph
        self.vertex_property = _property 
        for i,j in self.graph.items():
            self.master_vertex[i] = True
            self.duplicate_vertex[i] = None
            self.graph_edge_property[i] = None

class AsynFileIo(object):
    def __init__(self, filename, rank, size):
        self.filename = filename
        self.graph = {} # vertex : neighbor_list, all the vertex in "key" field are master
        self.master_vertex = {} # vertex : whether thie vertex is a master mirror copy (True/False)
        self.duplicate_vertex = {} # duplicate vertex index & master copy holder
        self.graph_edge_property = {} # vertex : neighbor_edge_property_list
        self.vertex_property = {} # vertex : vertex_property
        self.rank = rank
        self.size = size
        return 

    def read(self): # ONly support graph without edge property
        graph = {}
        _property = {}
        text = open(self.filename).readlines()
        for i in range(len(text)):
            text[i] = text[i][:-1]
        Num = int(text[0].split()[0])
        if (text[0].split()[1] =='False'):
            for i in range(1,1+Num):
                if((i-1)%self.size != self.rank):
                    continue
                tmp = text[i].split()
                for j in range(len(tmp)):
                    tmp[j]=int(tmp[j])
                    if(j>0):
                        if not tmp[j] in self.master_vertex:
                            self.master_vertex[tmp[j]] = False
                self.master_vertex[tmp[0]] = True
                graph[i-1] = tmp[1:]
            for i in range(1+Num,2*Num+1):
#                if((i-Num-1)%size != rank):
#                    continue
                if i-Num-1 in self.master_vertex:
                    _property[(i-1-Num)] = float(text[i])
        return graph, _property

    def GraphGeneration(self):
        _graph, _property = self.read()
        self.graph = _graph
        self.vertex_property = _property 
        for i,j in self.graph.items():
            self.master_vertex[i] = True
            self.duplicate_vertex[i] = None
            self.graph_edge_property[i] = None
