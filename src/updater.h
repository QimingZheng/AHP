/*************************************************************************/
/*! 此文件主要用来定义调度业务逻辑的调度函数:
 * computing(), updateGraph()
*/
/**************************************************************************/

#include "KVstore.h"
#include "GMR.h"
#include<string.h>
/* 将其他节点传递过来的数据更新到本节点顶点的前驱中 */
void updateGraph(graph_t *graph, Edge *rb, int rbsize, int rank) {
    int i = 0, j = 0, m = 0, n = 0;
    graph->prexadj[0] = 0;
    graph->prestatus[0] = inactive;
    while (i < graph->nvtxs && j < rbsize) {
        if (graph->ivsizes[i] < rb[j].vid) {
            /* 第一次同步数据之后, 需要将prexadj填充 */
            if (graph->prexadj[i + 1] == 0)
                graph->prexadj[i + 1] = m;
            i++;
            /* 先将其设置为inactive, 如果有更新前驱再设置为active */
            //if (i < graph->nvtxs) graph->prestatus[i] = inactive;
        }
        else if (graph->ivsizes[i] > rb[j].vid) {
            perror("接收的前驱数据有误.\n");
            MPI_Finalize();
            exit(1);
        }
        else {
            /* 将其他节点传递过来的前驱, 更新到graph的前驱缓存中 */
            for (n = graph->prexadj[i]; n < graph->prexadj[i + 1]; n++) {
                if(graph->preadjncy[n] == rb[j].fvid) {
                    graph->prevertexnnbor[n] = rb[j].nnbor;
                    graph->prefvwgts[n] = rb[j].fwgt;
                    graph->prefadjwgt[n] = rb[j].fewgt;
                    break;
                }
            }
            /* 如果从其他节点传来的前驱还未在preadjncy中, 则将其加入 */
            if (n >= graph->prexadj[i + 1]) {
                graph->prevertexnnbor[m] = rb[j].nnbor;
                graph->preadjncy[m]      = rb[j].fvid;
                graph->prefvwgts[m]      = rb[j].fwgt;
                graph->prefadjwgt[m]     = rb[j].fewgt;
                m++;
            }
            graph->prestatus[i] = active;
            j++;
        }
    }
    if (graph->prexadj[graph->nvtxs] == 0) graph->prexadj[graph->nvtxs] = m;
}

/* 将图中指定id的顶点的值进行更新, 并返回迭代是否结束 */
void updateGraph(int rank, graph_t *graph, std::list<KV> &reduceResult, UpdateMode upmode) {
    int i = 0;
    float currentMaxDeviation = 0.0;
    auto iter = reduceResult.begin();
    /* 将reduceResult中id与graph中vertex.id相同key值更新到vertex.value */
    while (i < graph->nvtxs && iter != reduceResult.end() ) {
        if (iter->key > graph->ivsizes[i]) i++;
        else if (graph->ivsizes[i] > iter->key) iter++;
        else {
            /* 计算误差, 并和老值进行比较, 判断迭代是否结束 */
            float deviation = fabs(iter->value - graph->fvwgts[i]);
            /* 与老值进行比较, 如果变化小于threhold,则将vertex.status设置为inactive */
            if (deviation > threshold) {
                if (deviation > currentMaxDeviation) currentMaxDeviation = deviation;
                if(DEBUG) printf("迭代误差: v%d: fabs(%f - %f) = %f\n", iter->key,
                        iter->value, graph->fvwgts[i], deviation);
                if(graph->status[i] == inactive) {
                    graph->status[i] = active;
                    convergentVertex--;
                }
            }
            else {
                if(graph->status[i] == active) {
                    convergentVertex++;
                    graph->status[i] = inactive;
                }
            }
            if (upmode == accu) 
                graph->fvwgts[i] += iter->value;
            else if (upmode == cover)
                graph->fvwgts[i] = iter->value;
            /* 对同一个键值, 可能有多个规约结果, 所以只自增递归结果的迭代器 */
            iter++; //i++;
        }
    }

    /* 如果当前误差小于全局残余误差则更新全局残余误差 */
    if (currentMaxDeviation < remainDeviation) remainDeviation = currentMaxDeviation;
    if(INFO) printf("迭代残余误差: %f\n", remainDeviation);
}

/*将单个节点内的顶点映射为Key/value, 对Key排序后，再进行规约*/
void computing(int rank, graph_t *graph, GMR *gmr, std::list<KV> &reduceResult) {
    std::vector<KV> kvs;
    recordTick("bgraphmap");
    for (int i=0; i<graph->nvtxs; i++) {
        Vertex vertex;
        /* 判断其前驱是否有过更改, 第一轮刚收到数据，全部计算 */
        if (/*iterNum != 0 && */graph->prestatus[i] == inactive) {
            if (graph->status[i] == active) {
                graph->status[i] = inactive;
                convergentVertex++;
            }
            continue;
        }
        graph->prestatus[i] = inactive;

        vertex.id = graph->ivsizes[i];
        vertex.value = graph->fvwgts[i];
        vertex.neighborSize = graph->xadj[i+1] - graph->xadj[i];
        vertex.prenvtxs = graph->prexadj[i + 1] - graph->prexadj[i];
        int neighbor_sn = 0;
        for (int j=graph->xadj[i]; j<graph->xadj[i+1]; j++, neighbor_sn++) {
            vertex.neighbors.push_back(graph->adjncy[j]);
            /* 将边的权重从图中顶点拷贝到vertex.edgewgt[k++]中 */
            vertex.edgewgt.push_back(graph->fadjwgt[j]);
        }
        neighbor_sn = 0;
        for (int j = graph->prexadj[i]; j < graph->prexadj[i + 1]; j++, neighbor_sn++) {
            vertex.previd.push_back(graph->preadjncy[j]);
            vertex.prefwgt.push_back(graph->prefvwgts[j]);
            vertex.prefewgt.push_back(graph->prefadjwgt[j]);
            vertex.prevertexnnbor.push_back(graph->prevertexnnbor[j]);
        }

        /* if (vertex.neighborSize > 0) */gmr->map(vertex, kvs);
    }
    recordTick("egraphmap");

    /* 由接收缓冲区数据构造一个个顶点(vertex), 再交给map处理 */
    /* 产生的Key/value，只记录本节点的顶点的,采用Bloom Filter验证 */
    recordTick("brecvbuffermap");
    recordTick("erecvbuffermap");

    /* 对map产生的key/value list进行排序 */
    recordTick("bsort");
    //kvs.sort(KVComp);
//     sort(kvs.begin(), kvs.end(), KVComp);
    recordTick("esort");

    recordTick("breduce");
    std::list<KV> sameKeylist;
    //std::list<KV> reduceResult;
    for (KV kv : kvs) {
        if(sameKeylist.size() > 0 && gmr->keyComp(kv, sameKeylist.front()) != 0) {
            KV tmpkv = gmr->reduce(sameKeylist);
            reduceResult.push_back(tmpkv);
            sameKeylist.clear();
        }
        sameKeylist.push_back(kv);
    }
    reduceResult.push_back(gmr->reduce(sameKeylist));
    sameKeylist.clear();
    recordTick("ereduce");
}

/* 打印计算的过程中的信息: 迭代次数, 各个步骤耗时 */
void printTimeConsume(int rank) {
    float convergence = 100.00;
    if (ntxs > 0) 
        convergence = 1.0 * convergentVertex / ntxs * 100;
    printf("P-%d, %dth(%-6.2f%%), D:%ef, Time:%f=(%f[count] + %f[data]"
            "+ %f[comp](%f[map] + %f[sort] + %f[red] + %f[update])"
            " + %f[barr] + %f(updpre))\n", rank, iterNum, convergence,
            remainDeviation, timeRecorder["eiteration"] - timeRecorder["bcomputing"],
            timeRecorder["eexchangecounts"] - timeRecorder["bexchangecounts"],
            timeRecorder["eexchangedata"] - timeRecorder["bexchangedata"],
            timeRecorder["ecomputing"] - timeRecorder["bcomputing"],
            timeRecorder["erecvbuffermap"] - timeRecorder["bgraphmap"],
            timeRecorder["esort"] - timeRecorder["bsort"], 
            timeRecorder["ereduce"] - timeRecorder["breduce"], 
            timeRecorder["eupdategraph"] - timeRecorder["bupdategraph"], 
            timeRecorder["eiteration"] - timeRecorder["eupdategraph"],
            timeRecorder["eupdatepre"] - timeRecorder["bupdatepre"]);
}

int checkfileexist(char *fname) {
    std::ifstream fin;
    fin.open(fname);
    if (!fin) {
        printf("文件不存在.\n");
        return 0;
    }
    else {
        return 1;
    }
}
int checkfileexists(std::string fname) {
    std::ifstream fin;
    fin.open(fname);
    if (!fin) {
        printf("文件不存在.\n");
        return 0;
    }
    else {
        return 1;
    }
}
