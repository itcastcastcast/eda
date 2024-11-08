#ifndef NEWCOMMON
#define NEWCOMMON
#include "common.h"
//别忘了更新父子关系
struct NewBlock
{
    vector<Node*>topnodes; //最上层的Node 数量需要比较少(与fanout对应) 能够直接连接到BUF
    //                   buffer
    //                    /\
    //                   /  \
    //                buffer buffer
    //                 /\      /\
    //                /  \    /  \
    //              candidates candidates candidates   //最底层是candidates
    //
    vector<Cluster*>clusters;
    vector<Node*>candidates;
    vector<Node*>nodes;   //有哪些点 Buffer和FF,计算面积的时候要用
    pair<int,int>coordinates;
};
extern vector<DriverNode*>BUF1;
extern vector<DriverNode*>BUF2;
extern vector<Node*>candidates;     //candidate包含buffer和ff
//TODO 后面可能需要加一些进一步优化的，因为可能一次性聚类聚不完？case是完全可以完成的,或许可以通过cfg来确保这种情况不发生/小概率
extern map<pair<int,int>,NewBlock*>blocks2;
//目前这个是基于blocks找的，暂时不知道是否需要基于blocks2的
extern Node* findgoodarea(Cluster*cluster,int Block_x,int Block_y);
extern DieArea dieArea2;
extern void addnodes(Node*node);
extern vector<NewBlock*>searchorder;
DriverNode *findgoodarea(double x,double y);
#endif