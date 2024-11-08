#include "parameter.h"
//应该根据实际情况决定，这些都是可以优化的
configurable cfg;
//你需要配置，可以优化
void mkconfig()
{
    cfg.l=10;
    cfg.w=10;
    cfg.K=1000;
    cfg.bound=20000; //初始bound
    cfg.deltabound=10000;
    cfg.deltaexceed=100;
    cfg.maxbound=80000;
    //转化为了manhattondist
    //lowerbound,upperbound后面算出
    //不用skew了
    //很多边角没有考虑 但感觉出现了本来就不合理：比如maxfanout=1
    //或者驱动的距离过小
    cfg.hk=1/2*constraint.max_fanout;
    cfg.deltaarea=10000;
    cfg.portion=2/3;
}
void initialized()
{
    mkconfig();
    dieArea.l=cfg.l;  
    dieArea.w=cfg.w;
}