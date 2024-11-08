#include "common.h"
#include "newcommon.h"
double ManhattonDistance(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return fabs(x1+xsize1/2-x2-xsize2/2)+fabs(y1+ysize1/2-y2-ysize2/2);
}
double rc(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2)
{
    return 0.5*constraint.net_unit_r*constraint.net_unit_c*ManhattonDistance(x1,y1,x2,y2,xsize1,xsize2,ysize1,ysize2);
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
        cout<<"error"<<endl;
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
            cout << "error" << endl;
            return -1;
        }
    }
double delay(Node*tmp1,Node*tmp2)
{
    return 2*0.69*constraint.net_unit_c*ManhattonDistance(tmp1,tmp2);
}
Node* initBuffer(double x, double y){
    DriverNode* buffer=new DriverNode();
    buffer->x = x;
    buffer->y = y;
    buffer->width = bufSize.width;
    buffer->height = bufSize.height;
    buffer->type = "BUF";
    buffer->name = "BUF";
    buffer->fanout = 0;
    buffer->rc = 0.0;
    buffer->parent = nullptr;
    buffer->children={};
    buffer->delay=0.0;
    buffer->cluster_id=-1;//不需要用
    buffer->block_x=static_cast<int>((x - dieArea.xmin) / dieArea2.block_width);
    buffer->block_y=static_cast<int>((y - dieArea.ymin) / dieArea2.block_height);
    return (Node*)buffer;
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
DriverNode *findgoodarea(Cluster*cluster,int block_x,int block_y,double area)
{
//设置了p的parent,fanout,rc等等
}
Node *findgoodarea(Cluster*cluster,int block_x,int block_y)
{
    double area=0;
    DriverNode*p=findgoodarea(cluster,block_x,block_y,area);
        while(p->rc>constraint.max_net_rc)
        {
            area+=cfg.deltaarea;
            delete p;
            p=findgoodarea(cluster,block_x,block_y,area);
        }
    return (Node*)p;
}
DriverNode *findgoodarea(double x,double y)//仅仅是搜索最近的，没有要求必须在block内部
{

}
