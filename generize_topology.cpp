//需要插入尽可能少的buffer  优化极差+buffer数量
#include "generize_topology.h"
#include<cassert>
#define DEBUG_INFO " File: " __FILE__ ", Line: " << __LINE__
#define COUT(x) cout<<#x<<" is "<<x<<DEBUG_INFO<<endl
//在这下面定义的全局函数全都不用在main中用但是要在这里用


//找到最远的节点，以此作为衡量的标准
//这里问了老师说可以认为clk在左边的中心，如果不能这样的话，需要对聚类的结果排序，
//但是由于可以认为clk在左边的中心，所以不需要排序，直接按照block来算
//但是我还是保守了一点，假设不是在中间
//这是为了选出最优的cases
pair<double,double>getaverage(vector<Node*>tmp)
{
    //assert(tmp.size()<=4);
    double sum_x=0;
    double sum_y=0;
    for(auto i:tmp)
    {
        sum_x+=i->x;
        sum_y+=i->y;
    }
    return make_pair(sum_x/tmp.size(),sum_y/tmp.size());
}
struct Case{
    Node* t;
    int n;         //需要n个节点
    double delay;
    int adjust;    //adjust为1表示h了需要调小  adjust=0表示小了需要调大
   // int fanout;
};
vector<Case*>cases;    //延迟可以接受

//对case排序
bool cmp2(Case*p1,Case*p2)
{
    return (p1->n<p2->n)||(p1->n==p2->n&&p1->t->children.size()<p2->t->children.size());
}

bool checkfanout(DriverNode driver)
{
    return driver.children.size()+1<constraint.max_fanout;
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
double getcenterx(NewBlock*a)
{
    return (2*a->coordinates.first+1)*dieArea2.block_width/2;
}
double getcentery(NewBlock*a)
{
    return (2*a->coordinates.second+1)*dieArea2.block_height/2;
}
bool cmp(NewBlock* a,NewBlock* b)
{
    double RC1=0.69*rc(clk->x,clk->y,getcenterx(a),getcentery(a),0.0,0.0,0.0,0.0);
    double RC2=0.69*rc(clk->x,clk->y,getcenterx(b),getcentery(b),0.0,0.0,0.0,0.0);
    int n=(int)(sqrt((0.69*RC1/constraint.buffer_delay)))-1;
    if(n<0)
    n=0;
    if(0.69*RC1/(n+1)+n*constraint.buffer_delay>0.69*RC1/(n+1+1)+(n+1)*constraint.buffer_delay)
    {
        n=n+1;
    }
    double delay1=0.69*RC1/(n+1)+n*constraint.buffer_delay+a->level*constraint.buffer_delay;
    n=(int)(sqrt((0.69*RC2/constraint.buffer_delay)))-1;
    if(n<0)
    n=0;
    if(0.69*RC2/(n+1)+n*constraint.buffer_delay>0.69*RC2/(n+1+1)+(n+1)*constraint.buffer_delay)
    {
        n=n+1;
    }
    double delay2=0.69*RC2/(n+1)+n*constraint.buffer_delay+b->level*constraint.buffer_delay;
    COUT(a->level);
    COUT(b->level);
    return delay1>delay2;
}
void testdist(NewBlock*b)
{
    //cout<<"testdist"<<endl;
    //cout<<b->coordinates.first<<" "<<b->coordinates.second<<endl;
    //cout<<getcenterx(b)<<" "<<getcentery(b)<<endl;
    //cout<<clk->y<<endl;
    //cout<<ManhattonDistance(clk->x,clk->y,getcenterx(b),getcentery(b),0.0,0.0,0.0,0.0)<<endl;
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
    cout<<dieArea2.block_width<<" "<<dieArea2.block_height<<endl;
    for(auto i:searchorder)
    {
        cout<<i->coordinates.first<<" "<<i->coordinates.second<<endl;
        cout<<getcenterx(i)<<" "<<getcentery(i)<<endl;
    }
}
void defineprebound(NewBlock*block)
{
     //找到最远的block的四个点的坐标中离clk最近的和最远的
    // 获取块的中心坐标
    double center_x = getcenterx(block);
    double center_y = getcentery(block);

    // 定义四个角的坐标
    vector<pair<double, double>> corners = {
        {center_x - dieArea2.block_width / 2, center_y - dieArea2.block_height / 2},
        {center_x , center_y }
    };

    double min_dist = std::numeric_limits<double>::max();
    double max_dist = 0.0;
    pair<double, double> nearest_point, farthest_point;

    for(const auto& corner : corners) {
        double dist = ManhattonDistance(clk->x, clk->y, corner.first, corner.second, 0.0, 0.0, 0.0, 0.0);
        if(dist < min_dist) {
            min_dist = dist;
            nearest_point = corner;
        }
        if(dist > max_dist) {
            max_dist = dist;
            farthest_point = corner;
        }
    }
    double delaymin=0.69*0.5*constraint.net_unit_c*constraint.net_unit_r*min_dist*min_dist;
    double delaymax=0.69*0.5*constraint.net_unit_c*constraint.net_unit_r*max_dist*max_dist;
    cout<<"delaymin is "<<delaymin<<DEBUG_INFO<<endl;
    cout<<"delaymax is "<<delaymax<<DEBUG_INFO<<endl;
    //delay=delay/(n+1)+n*buffer_delay
    //int nmin=(int)(sqrt((delaymin/constraint.buffer_delay)))-1;
    //cfg.low=min(cfg.low,min(delaymin/(nmin+1)+nmin*constraint.buffer_delay,delaymin/(nmin+1+1)+(nmin+1)*constraint.buffer_delay)+block->level*constraint.buffer_delay);
    int nmax=(int)(sqrt((delaymax/constraint.buffer_delay)))-1;
    cfg.up=max(cfg.up,min(delaymax/(nmax+1)+nmax*constraint.buffer_delay,delaymax/(nmax+1+1)+(nmax+1)*constraint.buffer_delay)+0.2+block->level*constraint.buffer_delay);
     COUT(cfg.up);
     //可以在这里修改cfg.low，我尝试过cfg.low=cfg.up-10,可以基本把skew控制在20以内
    cfg.low=cfg.up-20;
    COUT(cfg.low)    ;
    //char a=cin.get();
    // 使用 nearest_point 和 farthest_point 进行后续处理
}
//additionnode的作用改变了  现在是专门用于clk的，然后就是要是clkfanout不够了
//就在clk附近找到一个点，将clk的chdren转移到这个点上

void definexy(Case* tcase,DriverNode*tmp2,Node*tmp1)
{
    BUF2.push_back(tmp2);
    int n=tcase->n;
    cout<<n<<endl;
    cout<<tcase->adjust<<endl;
    pair<double,double>tmp={tmp2->x,tmp2->y};
    if(n==0)
    {
    assert(tcase->adjust==2);
    tmp2->parent=tmp1;
    tmp1->children.push_back(tmp2);
    }
    else if(tcase->adjust==1)
    {
    double p=0; //比例
    //计算p使得满足最后总=ideal
    double s1=tmp1->delay;
    double s2=tcase->delay;
    double s3=cfg.up;
    double s4=constraint.buffer_delay;
    cout<<s1<<" "<<s2<<" "<<s3<<" "<<s4<<endl;
    COUT(n*s2*(-pow(n,2)*s4 - n*s1 + n*s3 - n*s4 - s1 - s2 + s3));
    double p0=(n*s2 - sqrt(n*s2*(-pow(n,2)*s4 - n*s1 + n*s3 - n*s4 - s1 - s2 + s3)))/(n*s2*(n + 1));
    //这里p应该确保有效
    double deltax=(tmp.first-tmp1->x)*p0;
    double deltay=(tmp.second-tmp1->y)*p0;
    Node*children=tmp2;
    for(int i=1;i<=n;i++)
    {
       double x=tmp.first-i*deltax;
       double y=tmp.second-i*deltay;
    //这里忽略了buffer_size的影响
    COUT(p0);
    cout<<"arrive here"<<endl;
    COUT(x);
    COUT(y);
    DriverNode*p=(DriverNode*)findgoodarea(x,y,{});
    cout<<"arrive there"<<endl;
    COUT(p->name);
    BUF2.push_back(p);
    //更新children
        p->children.push_back(children);
        children->parent=p;
    children=p;
    //留个心眼 后面用了fanout吗？全都用children.size()统一
    }
    //这里肯定只有一个
    tmp1->children.push_back(children);
    children->parent=tmp1;
    return ;
    }
    else{
      assert(tcase->adjust==0);
      //这里就直接调节x使得符合预期
      //这里全都要用中心距离？不 幸运的是全是buf
      double ideal=0.5*(cfg.up)+0.5*cfg.low;
    //计算p使得满足最后总=ideal
    double x0=tmp1->x;
    double x1=tmp2->x;
    double y0=tmp1->y;
    double y1=tmp2->y;
    COUT(tmp1->name);
    double s2=(ideal-tmp1->delay-constraint.buffer_delay)/(0.69*0.5*constraint.net_unit_c*constraint.net_unit_r);
    double y=y0>y1?y0:y1;
    double s0=y-y0-x0;
    double s1=y-y1-x1;
    cout<<s0<<" "<<s1<<" "<<s2<<endl;
    double x=-s0/2 - s1/2 + sqrt(-s0*s0 + 2*s0*s1 - s1*s1 + 2*s2)/2;
    COUT(x);
    COUT(y);
    //cout<<p0<<endl;
    //TODO 这里有问题：BUF6047 BUF5878
     

      //这里需要解决positiony
      //要改改
      DriverNode*p=(DriverNode*)findgoodarea(x,y,{});
      COUT(p->name);
      COUT(p->x);
      COUT(p->y);
        BUF2.push_back(p);
           p->children.push_back(tmp2);
           tmp2->parent=p;
         tmp1->children.push_back(p);
         p->parent=tmp1;
    }
}

//这里规定方向必须是tmp1到tmp2
//专门用于clk相连接
void construct(DriverNode*p)
{
    int n=0;
    double RC=rc(clk->x,clk->y,p->x,p->y,0,0,0,0);
    n=(int)(sqrt((0.69*RC/constraint.buffer_delay)))-1;
    if(0.69*RC/(n+1)+n*constraint.buffer_delay>0.69*RC/(n+1+1)+(n+1)*constraint.buffer_delay)
    {
        n=n+1;
    }
    cout<<n<<endl;
    //pair<double,double>tmp=getaverage(tmp2);
    double deltax=(p->x-clk->x)/(n+1);
    double deltay=(p->y-clk->y)/(n+1);
    DriverNode*p0=p;
    BUF2.push_back(p0);
    Node*child=p0;
    for(int i=1;i<=n;i++)
    {
       double x=p0->x-i*deltax;
       double y=p0->y-i*deltay;
    //这里忽略了buffer_size的影响
    DriverNode*p1=(DriverNode*)findgoodarea(x,y,{});
    cout<<x<<" "<<y<<" "<<p1->x<<" "<<p1->y<<endl;
    COUT(p1->name);
    BUF2.push_back(p1);
    //更新children
    p1->children.push_back(child);
    child->parent=p1;
    child=p1;
    //留个心眼 后面用了fanout吗？全都用children.size()统一
    }
    //这里肯定只有一个
    clk->children.push_back(child);
    child->parent=clk;
    computedelay(p0);
}
bool findchicel(DriverNode*buf,DriverNode*p,double delay0)
{
    COUT(findchicel);
  assert(delay0+buf->delay<cfg.low-constraint.buffer_delay);
  //是驱动直接连接上去
            Case*newcase=new Case();
            newcase->adjust=0;
            newcase->delay=delay0;
            newcase->n=1;
            newcase->t=buf;
            cases.push_back(newcase);
            return 1;
}
bool findchiceh(DriverNode*buf,DriverNode*tmp2,double delay0,int n)
{
    COUT(findchiceh);
    COUT(delay0);
    COUT(n);
  assert(delay0+buf->delay>cfg.up);
  assert(n>=1);
  int test=3;
  for(int k=1;k<=n;k++)
  {
     if(delay0/(k+1)+k*constraint.buffer_delay+buf->delay<=cfg.up)
     {
        //cout<<k<<endl;
        //我后面还是约束一下tmp2的size不然不好处理
        Case*newcase=new Case();
        newcase->adjust=1;
        newcase->delay=delay0;
        newcase->n=k;
        newcase->t=buf;
        cases.push_back(newcase);
         return 1;
     }
  }
  cout<<"this could hardly happen"<<endl;
  assert(0);
    return 0;
}
//t没什么作用，其实就是能重载函数
bool construct(DriverNode*buf,DriverNode* p,int t)
{
    //这里的需要delete
    double RC=0;
    COUT(buf->delay);
    COUT(buf->x);
    COUT(buf->y);
    COUT(cfg.low);
    COUT(cfg.up);
    COUT(buf->name);
    COUT(p->x);
    COUT(p->y);
    RC=rc(buf->x,buf->y,p->x,p->y,0,0,0,0);
    //delay=0.69*RC/n+1 + n*bufferdelay
    //原来的delay
    COUT(t);
    double delay0=0.69*RC;
    COUT(delay0);
    if(delay0+buf->delay>=cfg.low&&delay0+buf->delay<=cfg.up)
    {
         Case*newcase=new Case();   
        newcase->adjust=2;  //Dont care
        newcase->delay=delay0;
        newcase->n=0;
        newcase->t=buf;
        cases.push_back(newcase);
        return 1;
    }
    int n=0;
    n=(int)(sqrt((0.69*RC/constraint.buffer_delay)))-1;
    //不管怎样 a,b的值一定能代表最小值
    int a=max(n,0);
    int b=max(n+1,0);
    double delaymin=min(delay0/(a+1)+a*constraint.buffer_delay,delay0/(b+1)+b*constraint.buffer_delay);
    COUT(delaymin);
    if(delaymin+buf->delay>=cfg.up)
    {
        cout<<"failture1"<<endl;
        return 0;
    }
    if(delay0+buf->delay<=cfg.low&&delay0+buf->delay>=cfg.low-constraint.buffer_delay)
    {
        //这个是小了 我要把它变大
        //这是不好优化 所以舍去
        cout<<"failture2"<<endl;
        return 0;
    }
    else
    {
        if(delay0+buf->delay<=cfg.low)
        {
            //这个是小了 我要把它变大
            return findchicel(buf,p,delay0);
        }
        else//
        { 
            cout<<delay0<<" "<<delaymin<<" "<<b<<" "<<buf->delay<<" "<<cfg.up<<endl;
            //大了 我要把它变小
            return findchiceh(buf,p,delay0,b);
        }
    }
}

//对两个block进行特殊处理
void construct(NewBlock*block,int idx,int flag)
{
    pair<double,double>tmp={getcenterx(block),getcentery(block)};
    cout<<tmp.first<<" "<<tmp.second<<endl;
        DriverNode*p=(DriverNode*)findgoodarea(tmp.first,tmp.second,{});
        cout<<p->x<<" "<<p->y<<endl;
        COUT(p->name);
        cout<<block->level<<endl;
        for(auto i:block->topnodes)
        {
            p->children.push_back(i);
            i->parent=p;
            COUT(i->name);
        }
    COUT("extend");
    //保证topnodes<max_fanout
    if(idx==1||(idx==2&&flag))
    {
        //在这个过程中确定low+up
        //add_additionnode(clk); //额外的node 这是为了确保还有备用方案
        construct(p);
        defineprebound(block);
        COUT(cfg.low);
        COUT(cfg.up);
        //cout<<"get here"<<endl;
    }
    else
    {
        double equlow=cfg.low;
        double equup=cfg.up;
        cfg.low=cfg.low-block->level*constraint.buffer_delay;
        cfg.up=cfg.up-block->level*constraint.buffer_delay;
        bool flag=0;
        cases.clear();
        //这里就要判断了
        for(auto buffer:BUF2)   //按照cfg看看能不能匹配
        {
            cout<<"get here"<<endl;
            construct(buffer,p,1);
        }
        construct((DriverNode*)clk,p,1);
        sort(cases.begin(),cases.end(),cmp2);
        cout<<"get definexy"<<endl;
        definexy(cases[0],p,cases[0]->t);
        cout<<"after definexy"<<endl;
        cfg.low=equlow;
        cfg.up=equup;
    }
    //恢复
    computedelay(p);
    COUT(p->name);
    COUT(p->delay);
}
void ConstructInOrder()
{
    getorder();
    //cout<<BUF2.size()<<endl;
    int i=0;
    for(auto block:searchorder)
    {
        cout<<BUF2.size()<<endl;
        if(block->topnodes.size()==0)
        continue;
        i++;
        cout<<i<<endl;
        cout<<block->topnodes.size()<<endl;
        cout<<"--------------------------"<<DEBUG_INFO<<endl;
        int flag=0;
        if(i==2)
        {
            double RC1=0.69*rc(clk->x,clk->y,getcenterx(block),getcentery(block),0.0,0.0,0.0,0.0);
            int n=(int)(sqrt((0.69*RC1/constraint.buffer_delay)))-1;
            if(n<0)
            n=0;
            if(0.69*RC1/(n+1)+n*constraint.buffer_delay>0.69*RC1/(n+1+1)+(n+1)*constraint.buffer_delay)
            {
                n=n+1;
            }
            double delay1=0.69*RC1/(n+1)+n*constraint.buffer_delay+block->level*constraint.buffer_delay;
                if(cfg.up-delay1<4)
            flag=1;
        }
        construct(block,i,flag);
    }
}

