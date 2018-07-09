#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* List terminator for GKfree() */
#define LTERM                   (void **) 0
#define GK_GRAPH_FMT_METIS      1
#define GRAPH_DEBUG             true 
#define GRAPH_INFO              true 

/* ntxs:          当前进程使用的子图中顶点数
 * nedges:        当前进程使用的子图边条数
 * NODE_STATUS:   顶点状态 */
int ntxs = 0;
int nedges = 0;
enum NODE_STATUS {active, inactive};

static int numbering = 0;
typedef struct graph_t {
  int nvtxs; /* total vertices in in my partition */
  int nedges;   /* total number of neighbors of my vertices */
  int *ivsizes;    /* global ID of each of my vertices */
  int *xadj;    /* xadj[i] is location of start of neighbors for vertex i */
  int *adjncy;      /* adjncys[xadj[i]] is first neighbor of vertex i */
  int *adjloc;     /* process owning each nbor in adjncy */

  int32_t *ivwgts;              /*!< The integer vertex weights */
  int32_t *iadjwgt;             /*!< The integer edge weights */

  /* add for new functions */
  int prenedges;
  int *prexadj;
  int *preadjncy;
  int *prevertexnnbor;
  float *prefvwgts;
  float *prefadjwgt;
  int *prestatus;

  float *fvwgts;
  float *fadjwgt;
  int *status;
} graph_t;

struct BareEdge {
    int from;
    int to;
    bool operator<(const BareEdge &rhs) const {
        if (from < rhs.from)
            return true;
        else if (from == rhs.from) {
            return to < rhs.to;
        }
        return false;
    }
};

struct Edge {
    int vid;
    int fvid;
    int nnbor;
    float fwgt;
    float fewgt;
};

