#include "cluster.h"
//l w k之类的东西都是可以动态调整的
extern double calculateDistance(const Node* node, const Cluster* cluster);
vector<Node*> removed_nodes; // 存储被移除的节点
vector<Cluster*>var_clusters;
void divideDieAreaIntoBlocks() {
    dieArea.block_width = (dieArea.xmax - dieArea.xmin) / dieArea.l;
    dieArea.block_height = (dieArea.ymax - dieArea.ymin) / dieArea.w;

    for (auto& ff : FFs) {
        ff->block_x = static_cast<int>((ff->x - dieArea.xmin) / dieArea.block_width);
        ff->block_y = static_cast<int>((ff->y - dieArea.ymin) / dieArea.block_height);

        pair<int, int> coord = {ff->block_x, ff->block_y};

        // 检查块是否已存在,如果不存在则创建新的 Block
        if (blocks.find(coord) == blocks.end()) {
            Block*b=new Block();
            b->coordinates = coord;
            blocks[coord]=b;
        }

        // 将 FF 添加到对应块的 nodes 中
        blocks[coord]->nodes.push_back(ff);
    }
}
//这里的约定是用最少的聚类数目，使得每个聚类的最大距离不超过bound
double meancluster(int K, double bound) {
    // 计算总的 FF 数量
    
    int total_ff_count = 0;
    for (const auto& entry : blocks) {
        const Block* block = entry.second;
        total_ff_count += block->nodes.size();
    }
    std::vector<double> all_max_distances; // 用于存储所有聚类的最大距离
    all_max_distances.clear();
    // 遍历每个块
    for (auto& entry : blocks) { // 使用非 const 引用以便修改 block
        pair<int, int> coord = entry.first;
        Block* block = entry.second;
        auto& block_ff = block->nodes; // 当前块内的 FF 列表
        int block_ff_count = block_ff.size(); // 当前块中的 FF 数量

        if (block_ff_count == 0) continue; // 如果块内没有 FF,跳过

        // 计算该块的聚类数量,至少为 1
        int block_k = max(1, static_cast<int>((static_cast<double>(block_ff_count) * K) / total_ff_count));
        // 随机初始化聚类中心
        vector<Node> initial_centers;
        for (int i = 0; i < block_k; ++i) {
            int random_index = rand() % block_ff_count;
            Node p;
            p.x=block_ff[random_index]->x;
            p.y=block_ff[random_index]->y;
            initial_centers.push_back(p);
        }

        // 初始化块内的聚类
        block->clusters.clear(); // 清空之前的聚类
        for (int i = 0; i < block_k; ++i) {
            Cluster*b=new Cluster(initial_centers[i].x, initial_centers[i].y);
            block->clusters.push_back(b);
        }

        bool changed = true;
        while (changed) {
            changed = false;

            // 清空每个聚类的成员
            for (auto& cluster : block->clusters) {
                cluster->clear();
            }

            // 将每个 FF 分配到最近的聚类中心（使用曼哈顿距离）
            for (auto& ff : block_ff) {
                double min_distance = numeric_limits<double>::max();
                int nearest_center = -1;

                for (int i = 0; i < block_k; ++i) {
                    double distance = fabs(ff->x - initial_centers[i].x) + fabs(ff->y - initial_centers[i].y);
                    if (distance < min_distance) {
                        min_distance = distance;
                        nearest_center = i;
                    }
                }

                // 将 FF 加入到最近的聚类中
                block->clusters[nearest_center]->members.push_back(ff);
                ff->cluster_id = nearest_center; // 设置 FF 的 cluster_id
            }

            // 更新聚类中心
            for (int i = 0; i < block_k; i++) {
                if (!block->clusters[i]->members.empty()) {
                    double sum_x = 0.0;
                    double sum_y = 0.0;

                    for (auto& node : block->clusters[i]->members) {
                        sum_x += node->x;
                        sum_y += node->y;
                    }

                    double new_x = sum_x / block->clusters[i]->members.size();
                    double new_y = sum_y / block->clusters[i]->members.size();

                    if (int(new_x) != int(initial_centers[i].x) || int(new_y) != int(initial_centers[i].y)) {
                        initial_centers[i].x = new_x;
                        block->clusters[i]->center.x=new_x;
                        initial_centers[i].y = new_y;
                        block->clusters[i]->center.y=new_y;
                        changed = true; // 如果中心改变,继续迭代
                    }
                }
            }
        }

        // 在移除之前,计算并显示每个聚类的最大距离
        //cout << "Block (" << coord.first << ", " << coord.second << ") - 聚类最大距离 (移除前):\n";
        for (int i = 0; i < block_k; ++i) {
            double max_distance = 0.0;
            for (auto& node : block->clusters[i]->members) {
                double distance = fabs(node->x - block->clusters[i]->center.x) + fabs(node->y - block->clusters[i]->center.y);
                if (distance > max_distance) {
                    max_distance = distance;
                }
            }
            all_max_distances.push_back(max_distance);
            
            //cout << "  聚类 " << i << " 最大距离: " << max_distance << "\n";
        }

        // 在聚类完成后,移除距离超过 bound 的点
        
        for (auto& cluster : block->clusters) {
            double center_x = cluster->center.x;
            double center_y = cluster->center.y;
            auto it = cluster->members.begin();
            while (it != cluster->members.end()) {
                double distance = fabs((*it)->x - center_x) + fabs((*it)->y - center_y);
                if (distance > bound) {
                    //cout<<"this could not happen"<<endl;
                    //(*it)->cluster_id = -1; // 可选：标记为未分配
                    removed_nodes.push_back(*it); // 添加到被移除的节点列表
                    it = cluster->members.erase(it); // 从聚类中移除该点
                } else {
                    ++it;
                }
            }
        }

        // 移除Cluster中元素个数超过 fanout 的点
        for (auto& cluster : block->clusters) {
            while (cluster->members.size() > constraint.max_fanout) {
                // 找到距离中心最远的节点
                auto farthest_it = cluster->members.begin();
                double max_distance = 0.0;
                for (auto it = cluster->members.begin(); it != cluster->members.end(); ++it) {
                    double distance = fabs((*it)->x - cluster->center.x) + fabs((*it)->y - cluster->center.y);
                    if (distance > max_distance) {
                        max_distance = distance;
                        farthest_it = it;
                    }
                }
                // 移除最远的节点
                //(*farthest_it)->cluster_id = -1; // 可选：标记为未分配
                removed_nodes.push_back(*farthest_it); // 添加到被移除的节点列表
                cluster->members.erase(farthest_it); // 从聚类中移除该点
            }
        }

        // 显示被移除的点的总数
        //cout << "Block (" << coord.first << ", " << coord.second << ") - 移除距离超过 " << bound << " 的点后,移除的总点数: " << removed_nodes.size() << "\n";

        // 如果需要将每个被移除的点视为一个独立的类,可以统计如下：
       // cout << "被移除的点被视为独立类,每个类包含1个点,总独立类数: " << removed_nodes.size() << "\n";
       }
       double average_max_distance = 0.0;
        if (!all_max_distances.empty()) {
        double sum = 0.0;
        for (double d : all_max_distances) {
            sum += d;
        }
        average_max_distance = sum / all_max_distances.size();
    }
    //cout<<"平均最大距离: "<<average_max_distance<<endl;
       return average_max_distance;
    }

void clearall()
{
    //有问题
    for(auto& entry : blocks) {
        for (auto& cluster : entry.second->clusters) {
            delete cluster;  // 手动释放 Cluster* 指向的内存
        }
        entry.second->clusters.clear();
    }

    // 清空 map
    
    removed_nodes.clear();
    var_clusters.clear(); //有问题，哪里来的
}

    //type=0代表*2,type=1代表+10000
    //TODO
    //这里约束了bound，实际上也间接约束了不超过max_rc
    //目前我的期望是1800000的图，50000起步
    double updatebound(double bound,int factor)
    {
        return bound+factor*cfg.deltabound;
    }
    double updateK(double K,int factor)
    {
        return K+factor*cfg.deltaexceed;
    }
    void iterartivecluster(int K,int exceed)
    {
        int factor=1;
        while(true)
        {
            bool flag=false;
            double bound=cfg.bound;//初始化
            double t=meancluster(K,bound);
            
            if(t>cfg.maxbound)
            {
                clearall();
                continue;
            }
           
        while(true)
        {
            /*
            if(K==5000)
            {
                cout<<"K==5000"<<endl;
                cout<<bound<<endl;
                //std::cin.get(); // 等待用户输入，暂停程序
            }*/
            if(bound>cfg.maxbound)
            {
             K=updateK(K,1);
                clearall();   
            break;
            }
            auto j=removed_nodes.begin();
            {
                //cout<<removed_nodes.size()<<endl;
                
                while(j!=removed_nodes.end())
                {
                    pair<int,int>c;
                    c.first=(*j)->block_x;
                    c.second=(*j)->block_y;
                    Block*block=blocks[c];
                    vector<Cluster*>&cluster=block->clusters;
                    double distance = fabs((*j)->x - cluster[(*j)->cluster_id]->center.x) + fabs((*j)->y - cluster[(*j)->cluster_id]->center.y);
                        
                    if(distance<bound)
                    {
                        if(cluster[(*j)->cluster_id]->members.size()<constraint.max_fanout)
                        {
                        cluster[(*j)->cluster_id]->members.push_back(*j);
                        j=removed_nodes.erase(j);
                        //cout<<removed_nodes.size()<<endl;
                        continue;
                        }
                        else {
                        double dist = 0;
                        Cluster* best_cluster = nullptr;
                        int current_fanout=constraint.max_fanout;
                        // 遍历所有的 block
                        for (auto& entry : blocks) {
                            double blockdistance = abs((*j)->block_x - entry.first.first) + abs((*j)->block_y - entry.first.second);
                                if (blockdistance <= 2) {
                            Block* other_block = entry.second;
                            // 判断 block 的 x 和 y 与 j 点的 x 和 y 的绝对值的最小距离
            
                            for (auto& cluster : other_block->clusters) {
                                    // 搜索找到距离小于 bound 的所有点中 fanout 最小的
                                    dist=calculateDistance(*j,cluster);
                                    if (dist < bound && cluster->members.size() < current_fanout) {
                                        current_fanout = cluster->members.size();
                                            best_cluster = cluster;
                }
            }
        }
    }

    // 如果找到合适的 cluster，则将点加入到该 cluster 中
    if (best_cluster != nullptr) {
        best_cluster->members.push_back(*j);
        j = removed_nodes.erase(j);
        continue;
    }
}
                    }
                   j++;
                }
            }
        if(removed_nodes.size()>=15*exceed)
        {
            factor=3;
            bound=updatebound(bound,factor);
            continue;
        }
        else if(removed_nodes.size()>=8*exceed)
        {
            factor=2;
            bound=updatebound(bound,factor);
            continue;
        }
        else if(removed_nodes.size()<=4*exceed)
        {
            factor=1;
            //TODO,有些removed_nodes可能是可以扔到邻近的buffer
            //TODO
            //这里需要一个函数，找到最近的buffer
            //如果找到了，就把这个点扔到buffer里面
            //如果没有找到，就继续找下一个
            
            if(removed_nodes.size()<exceed)
            {
                
                // 使用 var_clusters，与现有 clusters 比较找到最小距离
                // 若距离 < bound 并且没有超过 fanout，则加入该 cluster
                // 否则，新建一个 cluster
                // 最后将新建的 cluster 加入到 block.clusters 中

                // 假设 var_clusters 是 block.clusters 的副本

                // 遍历 removed_nodes
                for(auto it = removed_nodes.begin(); it != removed_nodes.end(); ) {
                    Node* node = *it;
                    double min_distance = std::numeric_limits<double>::max();
                    int best_cluster_idx = -1;
                    int current_fanout=constraint.max_fanout;
                    // 找到最近的 cluster
                    for(int i = 0; i < var_clusters.size(); ++i) {
                        double distance = fabs(node->x - var_clusters[i]->center.x) + fabs(node->y - var_clusters[i]->center.y);
                        if(distance < bound && var_clusters[i]->members.size() < current_fanout) {
                            current_fanout = var_clusters[i]->members.size();
                            best_cluster_idx = i;
                        }
                    }

                    if(best_cluster_idx != -1) {
                        // 加入到最佳聚类
                        var_clusters[best_cluster_idx]->members.push_back(node);
                        it = removed_nodes.erase(it);
                    }
                    else {
                        // 创建新聚类
                        pair<int,int>c;
                        c.first=node->block_x;
                        c.second=node->block_y;
                        Block*block=blocks[c];
                        vector<Cluster*>&cluster=block->clusters;
                        Cluster*new_cluster=new Cluster(node->x,node->y);
                        new_cluster->members.push_back(node);
                        var_clusters.push_back(new_cluster);
                        cluster.push_back(new_cluster);
                        it = removed_nodes.erase(it);
                    }
                }

                flag=true;
                break;
            }
        }
        bound=updatebound(bound,factor);
    }
    cout<<"Maybe it's last"<<" "<<K<<endl;
    //cout<<constraint.max_fanout<<endl;
    if(flag)
    break;
    K=updateK(K,1);
    }
}
