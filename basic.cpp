#include "common.h"
#include "newcommon.h"
void addnode(Node*node);
int global_var;
int step=0;
double ManhattonDistance(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return fabs(x1+xsize1/2-x2-xsize2/2)+fabs(y1+ysize1/2-y2-ysize2/2);
}
double rc(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return ManhattonDistance(x1,y1,x2,y2,xsize1,xsize2,ysize1,ysize2)*ManhattonDistance(x1,y1,x2,y2,xsize1,xsize2,ysize1,ysize2)*0.5*constraint.net_unit_r*constraint.net_unit_c;
}
//直接丢进两个节点计算距离
double ManhattonDistance(Node *tmp1,Node*tmp2)
{
    if(tmp1->type=="BUF"&&tmp2->type=="BUF")
    {
        return ManhattonDistance(tmp1->x,tmp1->y,tmp2->x,tmp2->y,bufSize.width,bufSize.width,bufSize.height,bufSize.height);
    }
    else if(tmp1->type=="BUF"&&tmp2->type=="FF")
    {
        return ManhattonDistance(tmp1->x,tmp1->y,tmp2->x,tmp2->y,bufSize.width,ffSize.width,bufSize.height,ffSize.height);
    }
    else if(tmp1->type=="FF"&&tmp2->type=="BUF")
    {
        return ManhattonDistance(tmp1->x,tmp1->y,tmp2->x,tmp2->y,ffSize.width,bufSize.width,ffSize.height,bufSize.height);
    }
    else if(tmp1->type=="CLK"&&tmp2->type=="BUF")
    {
        return ManhattonDistance(tmp1->x,tmp1->y,tmp2->x,tmp2->y,0,bufSize.width,0,bufSize.height);
    }
    else if(tmp1->type=="BUF"&&tmp2->type=="CLK")
    {
        return ManhattonDistance(tmp1->x,tmp1->y,tmp2->x,tmp2->y,bufSize.width,0,bufSize.height,0);
    }
    else if(tmp1->type=="CLK" && tmp2->type=="FF")
    {
        return ManhattonDistance(tmp1->x, tmp1->y, tmp2->x, tmp2->y, 0, ffSize.width, 0, ffSize.height);
    }
    else if(tmp1->type=="FF" && tmp2->type=="CLK")
    {
        return ManhattonDistance(tmp1->x, tmp1->y, tmp2->x, tmp2->y, ffSize.width, 0, ffSize.height, 0);
    }
    else
    {
        ////cout<<"error"<<endl;
        return -1;
    }
}

    //和上面的ManhattonDistance类似
    double rc(Node *tmp1, Node*tmp2)
    {
        if(tmp1->type=="BUF" && tmp2->type=="BUF")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, bufSize.width, bufSize.width, bufSize.height, bufSize.height);
        }
        else if(tmp1->type=="BUF" && tmp2->type=="FF")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, bufSize.width, ffSize.width, bufSize.height, ffSize.height);
        }
        else if(tmp1->type=="FF" && tmp2->type=="BUF")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, ffSize.width, bufSize.width, ffSize.height, bufSize.height);
        }
        else if(tmp1->type=="CLK" && tmp2->type=="BUF")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, 0, bufSize.width, 0, bufSize.height);
        }
        else if(tmp1->type=="BUF" && tmp2->type=="CLK")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, bufSize.width, 0, bufSize.height, 0);
        }
        else if(tmp1->type=="CLK" && tmp2->type=="FF")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, 0, ffSize.width, 0, ffSize.height);
        }
        else if(tmp1->type=="FF" && tmp2->type=="CLK")
        {
            return rc(tmp1->x, tmp1->y, tmp2->x, tmp2->y, ffSize.width, 0, ffSize.height, 0);
        }
        else
        {
            ////cout << "error" << endl;
            return -1;
        }
    }
double delay(Node*tmp1,Node*tmp2)
{
    return 0.69*rc(tmp1,tmp2);
}
DriverNode* initBuffer(double x, double y){
    global_var++;
    DriverNode* buffer=new DriverNode();
    buffer->x = x;
    buffer->y = y;
    buffer->width = bufSize.width;
    buffer->height = bufSize.height;
    buffer->type = "BUF";
    buffer->name = "BUF"+to_string(global_var);
    //cout<<buffer->name<<endl;
    buffer->fanout = 0;
    buffer->rc = 0.0;
    buffer->parent = nullptr;
    buffer->children={};
    buffer->delay=0.0;
    buffer->updelay=0;  //还没有计算delay
    buffer->cluster_id=-1;//不需要用
    buffer->block_x=static_cast<int>((x - dieArea.xmin) / dieArea2.block_width);
    buffer->block_y=static_cast<int>((y - dieArea.ymin) / dieArea2.block_height);
    buffer->end=0;
    #ifdef DEBUGinitBuffer
    ////cout<<"initBuffer"<<endl;
    ////cout<<"buffer->block_x is "<<buffer->block_x<<endl;
    ////cout<<"buffer->block_y is "<<buffer->block_y<<endl;
    #endif
    addnode((Node*)buffer);
    return buffer;
}
void addnode(Node*node,double x,double y)
{
    double p=node->x+x;
    double q=node->y+y;
    int block_x=static_cast<int>((p - dieArea2.xmin) / dieArea2.block_width);
    int block_y=static_cast<int>((q - dieArea2.ymin) / dieArea2.block_height);
    //这应该很定存在x  还是可能不存在的
    if(blocks2.find(make_pair(block_x,block_y))==blocks2.end())
    {
        ////cout<<block_x<<" "<<block_y<<endl;
        NewBlock *block=new NewBlock();
        block->coordinates.first=block_x;
        block->coordinates.second=block_y;
        blocks2[make_pair(block_x,block_y)]=block;
    }
    auto&nodes=blocks2[make_pair(block_x,block_y)]->nodes;
    if(find(nodes.begin(),nodes.end(),node)==nodes.end())
    {
        nodes.push_back(node);
    }
}
void addnode(Node*node)
{
    //判断Node的左下，左上，右下，右上的坐标，以此判断加入blocks2的哪里
    addnode(node,0,0);
    addnode(node,0,node->height);
    addnode(node,node->width,0);
    addnode(node,node->width,node->height);
}
void updatedelay(Node*node)
{   
    node->delay=0.69*rc(node,node->parent)+node->parent->delay;
    node->updelay=1;
    for(auto child:node->children)
    {
        child->delay=0.69*rc(child,node)+node->delay;
        if(child->end==1)  //FF是终点 暂时不优化
        {
            continue;
        }
        updatedelay(child);
    }
}
double computedelay(Node*node)
{
    //node->updelay==1 已经更新delay
    if(node->updelay==1 || node->type=="CLK")
    {
        return node->delay;
    }
    else
    {
    node->delay=0.69*rc(node,node->parent)+constraint.buffer_delay+computedelay(node->parent);
    //cout<<"yeshere1"<<endl;
    node->updelay=1;
    return node->delay;
    }
}

pair<int,int> inbound(int x,int y,int block_x,int block_y)
{
    int blocklx=((int)dieArea2.block_width+1)*block_x;
    int blockly=((int)dieArea2.block_height+1)*block_y;
    int blockux=((int)dieArea2.block_width-1)*(block_x+1);
    int blockuy=((int)dieArea2.block_height-1)*(block_y+1);
    int xl=x;
    int xu=x+bufSize.width;
    int yd=y;
    int yu=y+bufSize.height;
    pair<int,int>tmp;
    tmp.first=x;
    tmp.second=y;
    if(xl<blocklx)
    {
        tmp.first=blocklx;
    }
    if(xu>blockux)
    {
        tmp.first=blockux-bufSize.width;
    }
    if(yd<blockly)
    {
        tmp.second=blockly;
    }
    if(yu>blockuy)
    {
       tmp.second=blockuy-bufSize.height;
    }
    return tmp;
}
//t用来重载用
bool inbound(int x,int y,int block_x,int block_y,int t)
{
    int blocklx=((int)dieArea2.block_width+1)*block_x;
    int blockly=((int)dieArea2.block_height+1)*block_y;
    int blockux=((int)dieArea2.block_width-1)*(block_x+1);
    int blockuy=((int)dieArea2.block_height-1)*(block_y+1);
    int xl=x;
    int xu=x+bufSize.width;
    int yd=y;
    int yu=y+bufSize.height;
    if(xl<blocklx)
    {
        return false;
    }
    if(xu>blockux)
    {
        return false;
    }
    if(yd<blockly)
    {
        return false;
    }
    if(yu>blockuy)
    {
       return false;
    }
    return true;
}
int basicx=dieArea2.block_width/200;   //搜索步长
int basicy=dieArea2.block_height/200;  //搜索的次数
int time0=100000;
map<pair<int,int>,bool>searched;
//坐标
vector<pair<double,double>>axis;
//overlap
vector<double>overlap;
auto compare = [](pair<pair<int,int>,double> a, pair<pair<int,int>,double> b) -> bool {
    return a.second > b.second; // rc 较小的优先级高
};
priority_queue<pair<pair<int, int>, double>, vector<pair<pair<int, int>, double>>, decltype(compare)> q(compare);
int xs;  //xsize
int ys;
void divide()
{
    while (!q.empty()) {
        q.pop();
    }
    axis.clear();
    overlap.clear();
    basicx=dieArea2.block_width/1000;   //搜索步长
    basicy=dieArea2.block_height/1000;  
    xs=(int)dieArea2.block_width/basicx+1;
    ys=(int)dieArea2.block_height/basicy+1;
    searched.clear();
    #ifdef DEBUGdivide
    ////cout<<"basicx is "<<basicx<<endl;
    ////cout<<"basicy is "<<basicy<<endl;
    ////cout<<"time0 is "<<time0<<endl;
    ////cout<<"x is "<<xs<<endl;
    ////cout<<"y is "<<ys<<endl;
    //char a=cin.get();
    #endif
}
//TODO:test overlap is ok?
//TODO 我现在有点怀疑我只test buffer-FF  没有test buffer-buffer?
double checkoverlapfordebug(double x,double y)
{
    //assume x,y is a buf look if could overlap?
    //x,y应该是左下角坐标？
    cout<<"now is checkoverlapfordebug!"<<endl;
    double sum =0;
    double x0min=x;
    double x0max=x+bufSize.width;
    double y0min=y;
    double y0max=y+bufSize.height;
   for(auto ff:FFs)
   {
      double x1min=ff->x;
      double x1max=ff->x+ff->width;
      double y1min=ff->y;
      double y1max=ff->y+ff->height;
      if(x0min>=x1max||x0max<=x1min)
      {
            continue;
      }
        if(y0min>=y1max||y0max<=y1min)
        {
                continue;
        }
        double x0l=max(x0min,x1min);
        double x0r=min(x0max,x1max);
        double y0d=max(y0min,y1min);
        double y0u=min(y0max,y1max);
        sum+=(x0r-x0l)*(y0u-y0d);
   }
   cout<<"sum is "<<sum<<endl;
   return sum;
}
double checkoverlap(NewBlock*block,pair<pair<double,double>,double>element)
{
    //////cout<<"-----------------------------------------------------------"<<endl;
    double overlaparea=0;
    vector<Node*>t;
    t.assign(4,nullptr);
    double x2l=element.first.first;
    double x2d=element.first.second;
    double x2r=x2l+bufSize.width;
    double x2u=x2d+bufSize.height;
    //cout<<"I m going to cout block->nodes.size()"<<endl;
    //cout<<block->nodes.size()<<endl;
    for(auto node:block->nodes)
    {
        //4个node
        double x1l=node->x;
        double x1d=node->y;
        double x1r=node->x+node->width;
        double x1u=node->y+node->height;
        if(x1l<=x2l&&x1r>=x2l&&x1d<=x2d&&x1u>=x2d)
        {
            /*////cout<<"yes1"<<endl;
            ////cout<<x1l<<" "<<x1r<<" "<<x1d<<" "<<x1u<<endl;
            ////cout<<x2l<<" "<<x2r<<" "<<x2d<<" "<<x2u<<endl;
            ////cout<<node->name<<endl;*/
            t[0]=node;
        }
        if(x1l<=x2l&&x1r>=x2l&&x1d<=x2u&&x1u>=x2u)
        {
            /*////cout<<"yes2"<<endl;
            ////cout<<x1l<<" "<<x1r<<" "<<x1d<<" "<<x1u<<endl;
            ////cout<<x2l<<" "<<x2r<<" "<<x2d<<" "<<x2u<<endl;
            ////cout<<node->name<<endl;*/
            t[1]=node;
        }
        if(x1l<=x2r&&x1r>=x2r&&x1d<=x2d&&x1u>=x2d)
        {
            /*////cout<<"yes3"<<endl;
            ////cout<<x1l<<" "<<x1r<<" "<<x1d<<" "<<x1u<<endl;
            ////cout<<x2l<<" "<<x2r<<" "<<x2d<<" "<<x2u<<endl;
            ////cout<<node->name<<endl;*/
            t[2]=node;
        }
        if(x1l<=x2r&&x1r>=x2r&&x1d<=x2u&&x1u>=x2u)
        {
            /*////cout<<"yes4"<<endl;
            ////cout<<x1l<<" "<<x1r<<" "<<x1d<<" "<<x1u<<endl;
            ////cout<<x2l<<" "<<x2r<<" "<<x2d<<" "<<x2u<<endl;
            ////cout<<node->name<<endl;*/
            t[3]=node;
        }
    }
    if(t[1]==t[0]) t[1]=nullptr;
    if(t[2]==t[0]||t[2]==t[1]) t[2]=nullptr;
    if(t[3]==t[0]||t[3]==t[1]||t[3]==t[2]) t[3]=nullptr;
    for(int i=0;i<4;i++)
    {
       if(t[i]!=nullptr)
       {
          double sxl=max(t[i]->x,x2l);
          double sxr=min(t[i]->x+t[i]->width,x2r);
          double sxu=min(t[i]->y+t[i]->height,x2u);
          double sxd=max(t[i]->y,x2d);
          ////cout<<"sxu-sxd: "<<sxu-sxd<<" "<<"sxr-sxl: "<<sxr-sxl<<endl;
          overlaparea+=(sxu-sxd)*(sxr-sxl);
       }
    }
    return overlaparea;
}
//TODO:这里有问题吧  searched[block_x-1][block_y-1]?
bool valid(int x,int y,int block_x,int block_y,int step)
{
    //检查是否访问过这一块地方
    pair<int,int>a=make_pair(x,y);
    //////cout<<"beginsearch: "<<a.first<<" "<<a.second<<endl;
    if(!inbound(x,y,block_x,block_y,1))
    {
        return false;
    }
    
    else{
        if(searched.find(a) != searched.end()&&searched[a]==1)
        {
            #ifdef DEBUGsearch
            ////cout<<"invalid: have srached it "<<x<<" "<<y<<endl;
            //////cout<<a.first<<" "<<a.second<<endl;
            //////cout<<"--------------------------"<<endl;
            #endif
            return false;
        }
        else
        {
            searched[a]=1;
            return true;
        }
    }
}
//注意所有坐标都是左下角坐标
bool searchs(int x,int y,int block_x,int block_y)
{
    divide();
    //////cout<<"searchs"<<endl;
    //cout<<"x "<<x<<" y "<<y<<endl;
    //cout<<"block_x "<<block_x<<" block_y "<<block_y<<endl;
    pair<int,int>tmp=inbound(x,y,block_x,block_y);
    x=tmp.first;
    y=tmp.second;
    //cout<<x<<" "<<y<<" "<<endl;
    //////cout<<"newx "<<x<<" newy "<<y<<endl;
    q.push(make_pair(make_pair(x,y),0.0));
    step=0;
    while(q.size()&&(step<time0))
    {
        #ifdef DEBUGsearch
        ////cout<<"this is step: "<<step<<endl;
        #endif
        step++;
        auto element=q.top();
        ////cout<<"searchs "<<element.first.first<<" "<<element.first.second<<endl;
        q.pop();
        //这是blocks1的blcok_x和block_y
        if(blocks2.find(make_pair(block_x,block_y))==blocks2.end())
          {
            NewBlock *block=new NewBlock();
            block->coordinates.first=block_x;
            block->coordinates.second=block_y;
            blocks2[make_pair(block_x,block_y)]=block;
            }
        NewBlock*block=blocks2[make_pair(block_x,block_y)];
        double overlaparea=checkoverlap(block,element);
        ////cout<<"overlaparea is "<<overlaparea<<endl;
        if(overlaparea<bufSize.height*bufSize.width*2/100)
        {
           //将当前x,y作为最正确x,y丢进去
            axis.push_back(make_pair(element.first.first,element.first.second));
            overlap.push_back(overlaparea);
            return 1;
        }
        else
        {
            if(overlap.size()==0||overlaparea<overlap[overlap.size()-1])
            {
                axis.push_back(make_pair(element.first.first,element.first.second));
                overlap.push_back(overlaparea);
            }
            //确定下次要找到位置
            ////cout<<element.second;
            if(valid(element.first.first-basicx,element.first.second,block_x,block_y,step))
            {
                q.push(make_pair(make_pair(element.first.first-basicx,element.first.second),element.second+basicx));
            }
            if(valid(element.first.first+basicx,element.first.second,block_x,block_y,step))
            {
                q.push(make_pair(make_pair(element.first.first+basicx,element.first.second),element.second+basicx));
            }
            if(valid(element.first.first,element.first.second-basicy,block_x,block_y,step))
            {
                q.push(make_pair(make_pair(element.first.first,element.first.second-basicy),element.second+basicy));
            }
            if(valid(element.first.first,element.first.second+basicy,block_x,block_y,step))
            {
                q.push(make_pair(make_pair(element.first.first,element.first.second+basicy),element.second+basicy));
            }
        }
    }
    
    return 0;
}
Node *findgoodarea(Cluster*cluster,int block_x,int block_y)
{
    
    #ifdef DEBUGNfindgoodarea
    ////cout<<"this is original: "<<cluster->center.x<<" "<<cluster->center.y<<endl;
    #endif
    double area=0;
    searchs(cluster->center.x,cluster->center.y,block_x,block_y);
    ////cout<<cluster->center.x<<" "<<cluster->center.y<<endl;
    //char a=cin.get();
    double maxoverlap=0; 
    DriverNode* p=nullptr;
    bool flag=false;
    ////cout<<overlap.size()<<endl;
    while(true)
    {
        for(int i=0;i<axis.size();i++)
        {
            if(overlap[i]<=maxoverlap)
            {
                double idealx=axis[i].first;
                double idealy=axis[i].second;
                #ifdef DEBUGNfindgoodarea
                ////cout<<"this is original: "<<idealx<<" "<<idealy<<endl;
                #endif
                p=initBuffer(idealx,idealy);
                p->children=cluster->members;
                flag=true;
                break;
            }
        }
        maxoverlap=maxoverlap+bufSize.height*bufSize.width/1000;
        if(flag)
        {
            break;
        }
    }
    #ifdef DEBUGNfindgoodarea
    ////cout<<"could return"<<endl;
    #endif
    return (Node*)p;
}

//到时候对clk特殊处理一下,使得addnode在block内
Node* findgoodarea(double x,double y,vector<Node*>nodes)
{
    DriverNode* p=nullptr;
    if(x<dieArea2.xmin||x>dieArea2.xmax||y<dieArea2.ymin||y>dieArea2.ymax)
    {
        p=initBuffer(x,y);
        p->children=nodes;
        return (Node*)p;
    }
  int block_x=static_cast<int>((x - dieArea2.xmin) / dieArea2.block_width);
  int block_y=static_cast<int>((y - dieArea2.ymin) / dieArea2.block_height);
    searchs(x,y,block_x,block_y);
    double maxoverlap=0;
    bool flag=false;
    while(true)
    {
        for(int i=0;i<axis.size();i++)
        {
            if(overlap[i]<=maxoverlap)
            {
                double idealx=axis[i].first;
                double idealy=axis[i].second;
                p=initBuffer(idealx,idealy);
                p->children=nodes;
                flag=true;
                break;
            }
        }
        maxoverlap=maxoverlap+bufSize.height*bufSize.width/1000;
        if(flag)
        {
            break;
        }
    }
    return (Node*)p;
}