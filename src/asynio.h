#include<string.h>
#include<stdio.h>
using namespace std;
/* 获取发往其他各运算节点的字节数 */
int *Asyn_getSendBufferSize(const graph_t *graph, const int psize, const int rank, 
        int *sendcounts) {
    memset(sendcounts, 0, psize * sizeof(int));
    /* 先遍历一次需要发送的数据，确定需要和每个节点交换的数据 */
    for (int i=0; i<graph->nvtxs; i++) {
        /* 如果当前顶点vertex为iactive, 则不用发送 */
        if (graph->status[i] == inactive) continue;

        /* visited 用于记录当前遍历的顶点是否已经发送给节点adjloc[j] */
        for (int j=graph->xadj[i]; j<graph->xadj[i+1]; j++) {
            /* 如果当前顶点的终止顶点为在当前节点, 则不必发送 */
            //if (graph->adjloc[j] == rank) continue;
            sendcounts[graph->adjloc[j]]++;
        }
    }

    /* 发送数据格式为: neighbor_i, v.id, v.wgt, v.edge_i.wgt */
    return sendcounts;
}

/* 将要发送的数据从graph中拷贝到发送缓存sb中 */
Edge *Asyn_getSendbuffer(graph_t *graph, int *sdispls, 
        int psize, int rank, Edge *sb) {
    /* 将要发送顶点拷贝到对应的缓存: 内存拷贝(是否有方法减少拷贝?) */
    int *offsets = (int*)calloc(sizeof(int), psize);
    for (int i = 0; i < graph->nvtxs; i++) {
        /* 如果当前顶点vertex为iactive, 则不用发送 */
        if (graph->status[i] == inactive) continue;

        Edge edge;
        for (int j = graph->xadj[i]; j< graph->xadj[i+1]; j++) {
            //if (graph->adjloc[j] == rank) continue;
            edge.vid = graph->adjncy[j];
            edge.fvid = graph->ivsizes[i];
            edge.nnbor = graph->xadj[i + 1] - graph->xadj[i];
            edge.fwgt = graph->fvwgts[i];
            edge.fewgt = graph->fadjwgt[j];
            sb[sdispls[graph->adjloc[j]] + offsets[graph->adjloc[j]]] = edge;
            offsets[graph->adjloc[j]]++;
        }
    }
    if (offsets) free(offsets);
    return sb;
}

void read_buffersize_convergence(int rank, int i, int &edge_size){
	edge_size = 0;
	std::ifstream fin;
	std::string file_name = "." + std::to_string(rank) + "-" +std::to_string(i);
	if(!checkfileexists(file_name)) return;
	fin.open(file_name);
	fin >> edge_size;
}

void write_buffer(int rank_1,int rank_2, int start_pos, int size, Edge buffer[]){
	std::ofstream fout;
	std::string file_name = "." + std::to_string(rank_1) + "-" + std::to_string(rank_2);
	fout.open(file_name,ios::out);
	fout<< size << std::endl;
	if(size==0) return;
	for(int i=0;i<size;i++)
	{
		fout<< buffer[start_pos+i].vid<<" "<<buffer[start_pos+i].fvid<<" "<<buffer[start_pos+i].nnbor
			<<" "<<buffer[start_pos+i].fwgt<<" "<<buffer[start_pos+i].fewgt<<std::endl;
	}
}

void read_buffer(int rank_1,int rank_2, int start_pos, int size, Edge buffer[]){
	std::ifstream fin;
	std::string file_name = "." + std::to_string(rank_1) + "-" + std::to_string(rank_2);
	fin.open(file_name);
	int tmp;
	fin >> tmp;
	if (size != tmp) {std::cout<<"Error! File not consented!";}
	if(size==0) return;
	for(int i=0;i<size;i++)
	{
		Edge edge;
		fin >> edge.vid >> edge.fvid >> edge.nnbor >> edge.fwgt >> edge.fewgt;
		//buffer[start_pos+i]=edge;
	}
	fin.close();
	remove(file_name.c_str());
}

