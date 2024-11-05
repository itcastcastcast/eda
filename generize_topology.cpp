#include "generize_topology.h"
//找到最远的节点，以此作为衡量的标准
//这里问了老师说可以认为clk在左边的中心，如果不能这样的话，需要对聚类的结果排序，
//但是由于可以认为clk在左边的中心，所以不需要排序，直接按照block来算
//但是我还是保守了一点，假设不是在中间
bool checkfanout(DriverNode driver)
{
    return driver.fanout+1<constraint.max_fanout;
}

bool checkrc(Node* driver,double x,double y,double xsize,double ysize)
{
    DriverNode * p=(DriverNode*)driver;
    if(driver->type=="CLK")
    {
    return (p->rc+rc(driver->x,driver->y,x,y,0.0,0.0,xsize,ysize))<constraint.max_net_rc;
    }
    if(driver->type=="BUF")
    {
    return (p->rc+rc(driver->x,driver->y,x,y,p->width,p->height,xsize,ysize))<constraint.max_net_rc;
    }
    else
    {
        std::cerr << "Error: Unsupported driver type '" << driver->type << "'." << std::endl;
        std::exit(EXIT_FAILURE); // 或者返回错误状态
    }
}
double getcenterx(Block*a)
{
    return (2*a->coordinates.first+1)*dieArea.block_width/2;
}
double getcentery(Block*a)
{
    return (2*a->coordinates.second+1)*dieArea.block_height/2;
}
bool cmp(Block* a,Block* b)
{
    return ManhattonDistance(clk->x,clk->y,getcenterx(a),getcentery(a),0.0,0.0,0.0,0.0)>ManhattonDistance(clk->x,clk->y,getcenterx(b),getcentery(b),0.0,0.0,0.0,0.0);
}
void testdist(Block*b)
{
    cout<<b->coordinates.first<<" "<<b->coordinates.second<<endl;
    //cout<<clk->x<<" "<<clk->y<<endl;
    //cout<<getcenterx(b)<<endl;
    //cout<<getcentery(b)<<endl;
    //cout<<ManhattonDistance(clk->x,clk->y,getcenterx(b),getcentery(b),0.0,0.0,0.0,0.0)<<endl;
}
void getorder()
{
   int x=clk->x;
   int y=clk->y;
   searchorder.clear();
    for(auto i=blocks.begin();i!=blocks.end();i++)
    {
         searchorder.push_back(i->second);
    }
    sort(searchorder.begin(),searchorder.end(),cmp);
    //打印sort的结果：
    cout<<dieArea.block_width<<dieArea.block_height<<endl;
    for(auto i:searchorder)
    {
        testdist(i);
    }
}
//取出2个，算出最远dist
double getfarestdist() 
{
    double maxdist=0;
    int max_b=0; //哪个blcok优先;
    int max_id;//block中的哪个cluster优先；
    for(int i=0;i<2;i++)
    {
        auto&a=searchorder[i];
        //我已经投入很多聚类了，距离中心的距离一定不会太远
        for(auto j:a->clusters)
        {
            double dist=ManhattonDistance(clk->x,clk->y,j->center.x,j->center.y,0.0,0.0,0.0,0.0);
            if(dist>maxdist)
            {
                maxdist=dist;
                max_b=i;
            }
        }
    }
    return maxdist;                                                                                                                                                   
}