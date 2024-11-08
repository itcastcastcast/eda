#include "highlevelcluster.h"
void initialbuffer1()
{
    for(auto entry:blocks)
    {
        for(auto cluster:entry.second->clusters)
        {
            if(cluster->members.size()==0)
            {
                continue;
            }
            else if(cluster->members.size()==1)
            {
                candidates.push_back(cluster->members[0]);
            }
            else
            {
                //还没写，找到一个合法的好地方
                //这里面要用init buffer,并且要连接parent和children
                //返回的也是最顶层的聚类
                Node *p=findgoodarea(cluster,entry.first.first,entry.first.second);
                candidates.push_back(p);
                BUF1.push_back((DriverNode*)p);
            }
        }
    }
}
void divideIntoBlocks()
{
   blocks2.clear();
   dieArea2.xmax=dieArea.xmax;
   dieArea2.xmin=dieArea.xmin;
   dieArea2.ymin=dieArea.ymin;
   dieArea2.ymax=dieArea.ymax;
   dieArea2.w=int(dieArea.xmax/(cfg.bound))+1;
   dieArea2.l=int(dieArea.ymax/(cfg.bound))+1;
   dieArea2.block_height=dieArea2.ymax/dieArea2.l;
   dieArea2.block_width=dieArea2.xmax/dieArea2.w;
    for(auto member:candidates)
    {
    int x=int(member->x/dieArea2.block_width);
    int y=int(member->y/dieArea2.block_height);
    if(blocks2.find(make_pair(x,y))==blocks2.end())
    {
            NewBlock *block=new NewBlock();
            block->coordinates.first=x;
            block->coordinates.second=y;
            blocks2[make_pair(x,y)]=block;
    }
    blocks2[make_pair(x,y)]->candidates.push_back(member);
    }
}
//不用删除new的部分
void clearclusters()
{
    for(auto entry:blocks2)
    {
        entry.second->clusters.clear();
    }
}
void clearcandidates()
{
    for(auto entry:blocks2)
    {
        entry.second->candidates.clear();
    }
}
void updatecandidates()
{
    clearcandidates();
    for(auto entry:blocks2)
    {
        entry.second->candidates.clear();
        for(auto cluster:entry.second->clusters)
        {
            if(cluster->members.size()==0)
            {
                continue;
            }
            else if(cluster->members.size()==1)
            {
                candidates.push_back(cluster->members[0]);
            }
            else
            {
                //还没写，找到一个合法的好地方
                //这里面要用init buffer
                //简化吧，强制在blocks1中找
                Node *p=findgoodarea(cluster,entry.first.first,entry.first.second);
                addnodes(p);
                candidates.push_back(p);
                BUF1.push_back((DriverNode*)p);
            }
        }
    }
}

void updateclusters()
{
    //这里要判断各种条件,并且聚类
    clearclusters();
    
}


void updatetopnodes()
{
    for(auto entry:blocks2)
    {
        entry.second->topnodes.clear();
        for(auto cluster:entry.second->clusters)
        {
            if(cluster->members.size()==0)
            {
                continue;
            }
            else if(cluster->members.size()==1)
            {
                candidates.push_back(cluster->members[0]);
            }
            else
            {
                //还没写，找到一个合法的好地方
                //这里面要用init buffer
                //简化吧，强制在blocks1中找
                Node *p=findgoodarea(cluster,entry.first.first,entry.first.second);
                candidates.push_back(p);
                BUF1.push_back((DriverNode*)p);
            }
        }
    }
}
void highlevelcluster()
{
    //我想干脆在这里一次到底吧，判断能不能聚类（反正大概率也发生不了
    //如果fanout<max_fanout（应该可以直接判断，通过cfg，我能够约束fanout<max_fanout就一定可以
    //看看能不能满足要求
    //如果不能，就寻找最长的距离，再往前
    //如果能，就继续
    //如果fanout>max_fanout
    //进一步聚类
   
    for(auto entry:blocks2)
    {
       updateclusters();
       while(blocks2[entry.first]->clusters.size()>cfg.hk)
       {
           updatecandidates();
           updateclusters();
       }
       updatetopnodes();
    }
}
