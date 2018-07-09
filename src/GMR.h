/****************************************************
 * 该类用于定于MapReduce操作的基类
 * *************************************************/
class GMR {
public:
    /* 根据不同的算法, 对图进行初始化 */
    virtual void initGraph(graph_t *graph) = 0;
    /* Map/Reduce编程模型中的Map函数 */
    virtual void map(Vertex &v, std::vector<KV> &kvs) = 0;

    /* 用于将Map/Reduce计算过程中产生的KV list进行排序 */
    /* TODO: 采用OpenMP进行排序优化 */
    virtual void sort(std::list<KV> &kvs) = 0;

    /* Map/Reduce编程模型中的Reduce函数 */
    virtual KV reduce(std::list<KV> &kvs) = 0;

    /* 比较Key/Value的key */
    virtual int keyComp(KV &kv1, KV &kv2) {
        if (kv1.key == kv2.key)
            return 0;
        else if (kv1.key < kv2.key)
            return -1;
        else
            return 1;
    }

    /* 输出算法的执行结果 */
    virtual void printResult(graph_t *graph) { }

    /* 算法需要迭代的次数, 默认为系统设置的最大迭代次数 */
    static size_t algoIterNum;
    /* 算法采取的子图更新方式:累加或者覆盖 */
    static UpdateMode upmode;
};
size_t GMR::algoIterNum = INT_MAX;
UpdateMode GMR::upmode = cover;
