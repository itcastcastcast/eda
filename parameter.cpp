#include "parameter.h"
//应该根据实际情况决定，这些都是可以优化的
configurable cfg;
//你需要配置，可以优化
void mkconfig()
{
    cfg.l=5;
    cfg.w=2;
    cfg.K=1000;
    cfg.bound=20000; //初始bound
    cfg.deltabound=10000;
    cfg.deltaexceed=100;
    cfg.maxbound=800000;
}
void initialized()
{
    mkconfig();
    dieArea.l=cfg.l;  
    dieArea.w=cfg.w;
}