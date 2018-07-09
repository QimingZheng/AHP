/*************************************************************************/
/*! 此文件主要用来定义调度业务逻辑的调度函数:
 * computing(), updateGraph()
*/
/**************************************************************************/

/* 判断是否在控制台打印调试信息 */
#define INFO   false 
#define DEBUG  false 

/* 子图更新的方式:
 * accu: 累计方式, 将新值累加在原值之上;
 * cover: 覆写方式, 新值覆盖掉原值 */
enum UpdateMode {accu, cover};

/* timerRecorder:  迭代步的各个子过程耗时
 * totalRecvBytes: 目前为止, 接收到的字节数
 * maxMemory:      目前为止, 消耗的最大内存 */
std::map<std::string, double> timeRecorder;
long totalRecvBytes = 0;
long totalMaxMem    = 0;

/* threahold       : 迭代精度
 * remainDeviation : 当前迭代步的残余误差
 * iterNum         : 当目前为止, 迭代的次数
 * MAX_ITERATION   : 系统允许的最大迭代次数
 * convergentVertex: 本子图已经收敛的顶点个数
 * MAX_NEIGHBORSIZE: 图顶点允许的最大邻居数量 */
float threshold         = 0.0001;
float remainDeviation   = FLT_MAX;
int iterNum             = 0;
int MAX_ITERATION       = 10000;
size_t convergentVertex = 0;
const size_t MAX_NEIGHBORSIZE = 102400; 

/* Map/Reduce编程模型中的键值对,用于作为Map输出和Reduce输入输出 */
struct KV {
    int key; float value;
    /* skey, svalue作为结构体KV中辅助存储键值的单元 */
    int skey; std::list<int> svalue;
    KV() {}
    KV(int key, float value) : key(key), value(value), skey(-1) {}
    KV(int key, int skey, float value) : key(key), value(value), skey(skey) {}
};

/* 用于对KV的key进行排序的lambda函数 */
auto KVComp = [](KV kv1, KV kv2) -> bool {
    if (kv1.key < kv2.key)
        return true;
    else if (kv1.key == kv2.key) {
        if (kv1.skey < kv2.skey)
            return true;
        else
            return false;
    }
    else
        return false;
};

bool edgeComp(Edge e1, Edge e2) {
    if (e1.vid < e2.vid)
        return true;
    else if (e1.vid == e2.vid) {
        return e1.fvid < e2.fvid;
    }
    return false;
}

/* 记录当前程序执行到当前位置的MPI_Wtime */
void recordTick(std::string tickname) {
    timeRecorder[tickname] = MPI_Wtime();
}

/* 用于从csr(Compressed Sparse Row)中生成Vertex顶点进行map/reduce */
/* 用于业务逻辑计算，而非图的表示 */
struct Vertex {
    int id, loc, neighborSize, prenvtxs;
    std::vector<int> neighbors, neighborsloc, previd, prevertexnnbor;
    std::vector<float> prefwgt, prefewgt, edgewgt;
    float value;
    bool operator==(int key) {
        return this->id == key;
    }
    bool operator<(int key) {
        return id < key;
    }
};
