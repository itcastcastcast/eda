#include "generize_topology.h"
//在这下面定义的全局函数全都不用在main中用但是要在这里用
Node*additonnode;//没有加进BUF2，这个是为了备用，一般不用，只有fanout全部超了才用

//找到最远的节点，以此作为衡量的标准
//这里问了老师说可以认为clk在左边的中心，如果不能这样的话，需要对聚类的结果排序，
//但是由于可以认为clk在左边的中心，所以不需要排序，直接按照block来算
//但是我还是保守了一点，假设不是在中间
//这是为了选出最优的cases
vector<pair<Node*,pair<int,int>>>cases1;    //延迟可以接受
vector<pair<Node*,pair<int,int>>>cases2;    //延迟超过一定范围
//pair<int,int> 分别是buffersize和最开头那个的fanout 
bool cmp2(pair<Node*,pair<int,int>>&p1,pair<Node*,pair<int,int>>&p2)
{
    return (p1.second.first<p2.second.first)||(p1.second.first==p2.second.first&&p1.second.second<p2.second.second);
}

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
void add_additionnode(Node*node)
{ //初始添加 addtionnode只有一个 没了就要迅速创造
    Node*newnode=findgoodarea(node->x,node->y); //这是找最近的点
    node->children.push_back(newnode);
    newnode->parent=node;
    additonnode=newnode;
}
//这里规定方向必须是tmp1到tmp2
pair<double,double> getaverage(vector<Node*>tmp)
{
    double x=0;
    double y=0;
    for(auto i:tmp)
    {
        x+=i->x;
        y+=i->y;
    }
    x=x/tmp.size();
    y=y/tmp.size();
    return make_pair(x,y);
}
void construct(DriverNode*tmp1,vector<Node*>tmp2)
{
    double RC=0;
    for(auto j:tmp2)
    {
       RC+=rc(tmp1,j);
    }
    RC=RC/tmp2.size();
    //delay=0.69*RC/n+1 + n*bufferdelay
    int n=0;
    n=(int)(0.69*RC/constraint.buffer_delay)-1;
    cfg.ideal=0.69*RC/(n+1) + n*constraint.buffer_delay;
    pair<double,double>tmp=getaverage(tmp2);
    double deltax=(tmp.first-tmp1->x)/n;
    double deltay=(tmp.second-tmp1->y)/n;
    vector<Node*>children=tmp2;
    for(int i=1;i<=n;i++)
    {
       double x=tmp.first-i*deltax;
       double y=tmp.second-i*deltay;
    //这里忽略了buffer_size的影响
    DriverNode*p=findgoodarea(x,y);
    BUF2.push_back(p);
    //更新children
    for(auto j:children)
    {
        p->children.push_back(j);
        j->parent=p;
    }
    children.clear();
    children.push_back(p);
    p->fanout=children.size();
    }
    //这里肯定只有一个
    tmp1->children.push_back(children[0]);
    tmp1->fanout=1;
}
//t没什么作用，其实就是能重载函数
bool construct(DriverNode*buf,vector<Node*>tmp2,int t)
{
    assert(buf->children.size()!=0);
    if(buf->children.size()+tmp2.size()>=constraint.max_fanout)
    {
        return 0;
    }
    double RC=0; //其实大概率tmp2 size为1
    for(auto j:tmp2)
    {
       RC+=rc(buf,j);
    }
    RC=RC/tmp2.size();
    //delay=0.69*RC/n+1 + n*bufferdelay
    double delay=0.69*RC;
    double bias=cfg.ideal-(buf->delay+delay);
    if(bias>=-constraint.buffer_delay&&bias<=0)
    {
          cases1.push_back(make_pair(buf,make_pair(0,buf->fanout)));
    }
}
//对两个block进行特殊处理
void construct(NewBlock*block)
{
    if(BUF2.size()==0)
    {
        add_additionnode(clk); //额外的node 这是为了确保还有备用方案
        construct((DriverNode*)clk,block->topnodes);
    }
    else
    {
        bool flag=0;
        //这里就要判断了
        for(auto buffer:BUF2)   //按照cfg看看能不能匹配
        {
            if(construct(buffer,block->topnodes,1))
                  flag=1;
        }
        if(flag==0)
        {
            //启用备胎
        }
    }
    computedelay(block);
}
void ConstructInOrder()
{
    int i=0;
    for(auto block:searchorder)
    {
        i++;
        construct(block);
    }
}

