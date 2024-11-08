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
void testdist(NewBlock*b)
{
    cout<<b->coordinates.first<<" "<<b->coordinates.second<<endl;
    //cout<<clk->x<<" "<<clk->y<<endl;
    //cout<<getcenterx(b)<<endl;
    //cout<<getcentery(b)<<endl;
    //cout<<ManhattonDistance(clk->x,clk->y,getcenterx(b),getcentery(b),0.0,0.0,0.0,0.0)<<endl;
}
//这里我改变主意了，直接取中间作为最大值，getorder需要，但是最远的dist是不需要的
//也就是下面的不需要
void getorder()
{
   int x=clk->x;
   int y=clk->y;
   searchorder.clear();
    for(auto i=blocks2.begin();i!=blocks2.end();i++)
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

void construct(Node*tmp1,Node*tmp2)
{
    double RC=rc(tmp1,tmp2);
    //delay=2*0.69*RC/n+1 + n*bufferdelay
    int n=0;
    double tmin=sqrt(2*0.69*RC*constraint.buffer_delay)/(2*0.69*2);
    double deltax=0;
    double deltay=0;
    if(tmin<cfg.portion*constraint.max_net_rc)
    {
        n=(int)(sqrt(2*0.69*RC/constraint.buffer_delay))-1;
    }
    else
    {
        n=RC/(cfg.portion*constraint.max_net_rc);
    }
    deltax=(tmp2->x-tmp1->x)/(n+1);
    deltay=(tmp2->y-tmp1->y)/(n+1);
    for(int i=1;i<=n;i++)
    {
       double x=tmp1->x+i*deltax;
       double y=tmp1->y+i*deltay;
    DriverNode*p=findgoodarea(x,y);
    BUF2.push_back(p);
    }
}
//对两个block进行特殊处理
void construct(NewBlock*block,int type)
{
    if(type==1)
    {
        
    }
    else
    {
        for(auto i:BUF2)
        {
            
        }
    }
}
void ConstructInOrder()
{
    int i=0;
    for(auto block:searchorder)
    {
        i++;
        construct(block,i<=1);
    }
}

