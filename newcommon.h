#ifndef NEWCOMMON
#define NEWCOMMON
#include "common.h"
//别忘了更新父子关系
//特意向老师确认了，我感觉老师的意思是max_rc/max_fanout已经足够大了，所以不需要考虑这个问题
struct NewBlock
{
    vector<Node*>topnodes; //最上层的Node 数量需要比较少(与fanout对应) 能够直接连接到BUF
    //                   
    //
    vector<Cluster*>clusters;
    vector<Node*>candidates;
    vector<Node*>nodes;   //有哪些点 Buffer和FF,计算面积的时候要用
    pair<int,int>coordinates;
    int level=0;   //level标志了聚类的层次
};
extern vector<DriverNode*>BUF1;
extern vector<DriverNode*>BUF2;
     //candidate包含buffer和ff
//TODO 后面可能需要加一些进一步优化的，因为可能一次性聚类聚不完？case是完全可以完成的,或许可以通过cfg来确保这种情况不发生/小概率
extern map<pair<int,int>,NewBlock*>blocks2;
//目前这个是基于blocks找的，暂时不知道是否需要基于blocks2的
extern Node* findgoodarea(Cluster*cluster,int Block_x,int Block_y);
extern DieArea dieArea2;
extern void updatedelay(Node*node);
extern vector<NewBlock*>searchorder;
Node* findgoodarea(double x,double y,vector<Node*>nodes); //这里就是直接找可以符合的+偏离最小
extern void computedelay(NewBlock*block);
extern double computedelay(Node*node);
extern vector<Node*>candidates;

//后面adjust可以调整：牺牲overlap 优化最大和最小latency之差
#endif