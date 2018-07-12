from mpi4py import MPI
import time
from Worker import *
from Master import *
from IO import *
from graph import *

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

if rank == size-1: # master process
    master = Master(rank, size-1, comm)
    master.Run()
else:
    worker = Worker("../graph/large.graph", rank, size-1, comm)
    worker.Local_Graph()
    worker.Local_Step()
