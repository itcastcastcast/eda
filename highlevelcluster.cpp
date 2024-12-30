#include "highlevelcluster.h"
extern double checkoverlapfordebug(double x,double y);
extern void addnode(Node*node);
extern void appendSVGCustomNode(double L, double W, Node* node, const std::string& filename,string color);
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
            /*else if(cluster->members.size()==1)
            {
                candidates.push_back(cluster->members[0]);
                cout<<cluster->members[0]->x<<" "<<cluster->members[0]->y<<endl;
            }*/
            else
            {
                //还没写，找到一个合法的好地方
                //这里面要用init buffer,并且要连接parent和children
                //返回的也是最顶层的聚类
                int block_x=cluster->center.x/dieArea2.block_width;
                int block_y=cluster->center.y/dieArea2.block_height;
                //int block_x=0;
                //int block_y=0;
                //cout<<"这里一定要注意"<<endl;
                //cout<<block_x<<" "<<block_y<<endl;
                Node *p=findgoodarea(cluster,block_x,block_y);
                //cout<<"这是最新的"<<endl;
                //cout<<p->x<<" "<<p->y<<endl;
                //cout<<"这是原始的"<<endl;
                //cout<<cluster->center.x<<" "<<cluster->center.y<<endl;
                //checkoverlapfordebug(p->x,p->y);
                candidates.push_back(p);
                BUF1.push_back((DriverNode*)p);
            }
        }
    }
    for(auto member:candidates)
    {
    int x=int(member->x/dieArea2.block_width);
    int y=int(member->y/dieArea2.block_height);
    if(blocks2.find(make_pair(x,y))==blocks2.end())
    {
            NewBlock *block=new NewBlock();
            block->coordinates.first=x;
            block->coordinates.second=y;
            block->level=0;
            //cout<<x<<" "<<y<<endl;
            blocks2[make_pair(x,y)]=block;
    }
    blocks2[make_pair(x,y)]->candidates.push_back(member);
    }
}

void divideIntoBlocks()
{
   blocks2.clear();
   dieArea2.xmax=dieArea.xmax;
   dieArea2.xmin=dieArea.xmin;
   dieArea2.ymin=dieArea.ymin;
   dieArea2.ymax=dieArea.ymax;
   
    
   dieArea2.w=int(dieArea2.xmax/110000)+1;
   //cout<<dieArea2.w<<endl;
   dieArea2.l=int(dieArea2.ymax/110000)+1;
   //cout<<dieArea2.l<<endl;
   ////cout<<1<<endl;
   dieArea2.block_height=dieArea2.ymax/dieArea2.l;
   dieArea2.block_width=dieArea2.xmax/dieArea2.w;
   /*
   cout<<dieArea2.xmax<<endl;
    cout<<dieArea2.xmin<<endl;
    cout<<dieArea2.ymin<<endl;
    cout<<dieArea2.ymax<<endl;
   cout<<dieArea2.block_width<<" "<<dieArea2.block_height<<endl;
   char h1=cin.get();
   */
    for(auto member:FFs)
    {
    addnode(member);
    }
}
//不用删除new的部分
void clearclusters(vector<Cluster*>&clusters)
{
    clusters.clear();
}
void clearcandidates(vector<Node*>&candidates)
{
    candidates.clear();
}
void updatecandidates(vector<Cluster*>&clusters,vector<Node*>&candidates,int block_x,int block_y)
{
    clearcandidates(candidates);
    for(auto cluster:clusters)
    {
    if(cluster->members.size()==0)
    {
        continue;
    }
    /*else if(cluster->members.size()==1)
    {
        cout<<"only one"<<endl;
        cout<<cluster->members[0]->x<<" "<<cluster->members[0]->y<<endl;
        candidates.push_back(cluster->members[0]);
    }*/
    else
    {
        //还没写，找到一个合法的好地方
        //这里面要用init buffer
        //简化吧，强制在blocks1中找
        Node *p=findgoodarea(cluster,block_x,block_y);
        cout<<p->x<<" "<<p->y<<endl;
        candidates.push_back(p);
        #ifdef GENERATEBUF2BUFpatten
        appendSVGCustomNode(dieArea2.xmax,dieArea2.ymax,p,"output.svg","blue");
        #endif
        BUF1.push_back((DriverNode*)p);
    }
    }
}
//TODO
//找到了updateclusters不能收敛的原因:  if (cluster_sizes[j] < max_fanout)
//我的cluster.cpp没有这样做，是保证收敛的
void updateclusters(vector<Cluster*>& clusters, vector<Node*>& nodes) {
    // 清空现有的聚类
    cout<<"it's a new one"<<endl;
    clearclusters(clusters);

    int max_fanout = constraint.max_fanout;
    int num_nodes = nodes.size();

    // 计算需要的聚类数量，确保每个聚类的容量不超过 max_fanout
    int num_clusters = (num_nodes + max_fanout - 1) / max_fanout;

    // 初始化聚类中心，可以随机选择节点作为初始中心
    vector<Node> centers;
    unordered_set<int> chosen_indices;
    srand(static_cast<unsigned int>(time(NULL)));
    while (centers.size() < num_clusters) {
        int idx = rand() % num_nodes;
        if (chosen_indices.find(idx) == chosen_indices.end()) {
            Node center;
            center.x=nodes[idx]->x;
            center.y=nodes[idx]->y;
            cout<<"center is "<<center.x<<" "<<center.y<<endl;
            centers.push_back(center);
            chosen_indices.insert(idx);
        }
    }

    // 初始化聚类
    clusters.resize(num_clusters);
    for (int i = 0; i < num_clusters; ++i) {
        clusters[i] = new Cluster(centers[i].x,centers[i].y);
        clusters[i]->members.clear();
    }

    // 创建一个列表，用于记录每个节点的聚类分配情况
    vector<int> assignments(num_nodes, -1);

    bool changed = true;
    int maxiter=0;
    while (changed) {
        maxiter++;
        if(maxiter>100)
        break;
        changed = false;

        // 清空每个聚类的成员
        for (auto& cluster : clusters) {
            cluster->members.clear();
        }

        // 记录每个聚类的当前大小
        vector<int> cluster_sizes(num_clusters, 0);
        // 第一步：分配节点到最近的聚类中心，考虑聚类容量限制
        for (int i = 0; i < num_nodes; ++i) {
            Node* node = nodes[i];
            double min_distance = numeric_limits<double>::max();
            int closest_cluster = -1;

            // 找到距离最近且未满的聚类
            for (int j = 0; j < num_clusters; ++j) {
                if (cluster_sizes[j] < max_fanout) {
                    double distance = fabs(node->x - clusters[j]->center.x) + fabs(node->y - clusters[j]->center.y);
                    if (distance < min_distance) {
                        min_distance = distance;
                        closest_cluster = j;
                    }
                }
            }
            assert(closest_cluster != -1);

            // 如果所有聚类都已满，可能需要处理（可以添加新的聚类，或重新分配）
            if (closest_cluster == -1) {
                // 添加新的聚类
                Cluster* new_cluster = new Cluster(node->x,node->y);
                clusters.push_back(new_cluster);
                num_clusters++;
                cluster_sizes.push_back(0);
                closest_cluster = num_clusters - 1;
                changed = true;
            }

            // 分配节点
            clusters[closest_cluster]->members.push_back(node);
            cluster_sizes[closest_cluster]++;
            
            // 检查分配是否发生变化
            if (assignments[i] != closest_cluster) {
                changed = true;
                cout<<"other alloc"<<endl;
                assignments[i] = closest_cluster;
            }
       }

        // 第二步：更新聚类中心
        for (int j = 0; j < num_clusters; j++) {
            Cluster* cluster = clusters[j];
            if (cluster->members.empty()) {
                continue;
            }
            double sum_x = 0.0;
            double sum_y = 0.0;
            for (auto& node : cluster->members) {
                sum_x += node->x;
                sum_y += node->y;
            }
            double new_center_x = sum_x / cluster->members.size();
            double new_center_y = sum_y / cluster->members.size();

            // 检查聚类中心是否发生变化
            if (int(cluster->center.x) != int(new_center_x) || int(cluster->center.y) != int(new_center_y)) {
                cout<<"center changed"<<endl;
                //changed = true;
                cluster->center.x = new_center_x;
                cluster->center.y = new_center_y;
            }
        }
    }
}


void updatetopnodes(vector<Cluster*>&clusters,vector<Node*>&topnodes,int block_x,int block_y)
{
    clearcandidates(topnodes);
   //clearcandidates(candidates);
    for(auto cluster:clusters)
    {
    if(cluster->members.size()==0)
    {
        cout<<cluster->members.size()<<endl;
        continue;
    }
    /*
    else if(cluster->members.size()==1)
    {
        topnodes.push_back(cluster->members[0]);
    }*/
    else
    {
        Node *p=findgoodarea(cluster,block_x,block_y);

        p->end=1;
        topnodes.push_back(p);
        #ifdef GENERATEBUF2BUFpatten
        appendSVGCustomNode(dieArea2.xmax,dieArea2.ymax,p,"output.svg","blue");
        #endif
        BUF1.push_back((DriverNode*)p);
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
   //算了不管了 就是觉得最多再聚类这一次
    for(auto entry:blocks2)
    {
        cout<<"entered for"<<endl;
       updateclusters(entry.second->clusters,entry.second->candidates);
       cout<<"out of updateclusters"<<endl;
       cout<<entry.second->clusters.size()<<endl;
       while(blocks2[entry.first]->clusters.size()>constraint.max_fanout)
       {
        blocks2[entry.first]->level++;
        cout<<"in the while"<<endl;
        cout<<blocks2[entry.first]->clusters.size()<<endl;
            //cout<<"if you get this , maybe something wrong"<<endl;
            //char p=cin.get();
           updatecandidates(entry.second->clusters,entry.second->candidates,entry.first.first,entry.first.second);
           updateclusters(entry.second->clusters,entry.second->candidates);
       }
       updatetopnodes(entry.second->clusters,entry.second->topnodes,entry.first.first,entry.first.second);
    }
    /*
    int count0=0;
    for(auto entry:blocks2)
    {
        if(blocks2[entry.first]->topnodes.size()==0)
        {
            cout<<"hhhhhhh"<<endl;
        }
    }*/
    //cout<<count0<<endl;
    cout<<BUF1.size()<<endl;
    //char a=cin.get();
}
