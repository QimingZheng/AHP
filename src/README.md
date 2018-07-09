# Main structure

## Graph 抽象类(graph.h)
用数组的形式保存图结构
局部顶点x---全局顶点ivsizes[x]
局部定点x的邻居：adjncy[xadj[x]]-->adjncy[xadj[x+1]]

边：
vid- 邻接顶点
fvid- 本顶点


## FILE IO (io.h)
文件IO部分：
要求数据文件在集群各节点上的位置相同；读入按照hash的方式均匀分配图的节点/边
0号进程额外承担文件读入错误时error发布，终止进程的任务
SendBuffer保存active的边信息

## KV store (KVstore.h)

