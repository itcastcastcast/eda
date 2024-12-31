#include "preprocess.h"
#include "cluster.h"
#include "parameter.h"
#include "output.h"
#include "newcommon.h"
#include "draw.cpp"
extern void ConstructInOrder();
vector<Node*> FFs;
vector<Node*> buffers;
Node* clk = nullptr;
Constraint constraint;
DieArea dieArea;
ComponentSize ffSize;
ComponentSize bufSize;
map<pair<int, int>, Block*> blocks;
vector<NewBlock*>searchorder;
extern void divideIntoBlocks();
extern void initialbuffer1();
extern void highlevelcluster();

vector<DriverNode*>BUF1;
vector<DriverNode*>BUF2;
vector<Node*>candidates;     //candidate包含buffer和ff
//TODO 后面可能需要加一些进一步优化的，因为可能一次性聚类聚不完？case是完全可以完成的,或许可以通过cfg来确保这种情况不发生/小概率
map<pair<int,int>,NewBlock*>blocks2;
DieArea dieArea2;
//验证cluster 成功
double calculateDistance(const Node* node, const Cluster* cluster) {
    // 计算节点到聚类中心的距离
    // 这里假设 Node 和 Cluster 有适当的成员函数来获取坐标
    double dx = node->x - cluster->center.x;
    double dy = node->y - cluster->center.y;
    return fabs(dx)+fabs(dy);
}

bool areAllNodesClustered() {
    // 遍历所有的块
    double maxdistance=0;
    cout<<blocks.size()<<endl;
    for(auto node:FFs){
         bool found = false;
    for (const auto& entry : blocks) {
        const Block* block = entry.second;

        // 遍历块中的所有节点
           for (const auto& cluster : block->clusters) {
          if (std::find(cluster->members.begin(), cluster->members.end(), node) != cluster->members.end()) {
            // 遍历块中的所有聚类
                // 检查节点是否在聚类中
                found = true;
                 double dist = calculateDistance(node, cluster);
                    if (dist >= maxdistance) {
                        maxdistance=dist;
                    }
                }
            }
                 if(found==true)
                  break;
    }
     // 如果节点不在任何聚类中，返回 false
            if (!found) {
                return false;
            }
    }
    cout<<"maxdist is "<<maxdistance<<endl;
    // 所有节点都在某个聚类中，返回 true
    return true;
}

int main(int argc, char ** argv){
    srand(374);  
    string def_filename = string(argv[1])+"/problem.def";
    string constraints_filename = string(argv[1])+"/constraints.txt";
    string output_filename = "output.def";
    readConstraints(constraints_filename);
    readDEF(def_filename);
    cout<<FFs.size()<<endl;
    cout<<"attention here"<<endl;
    generateSVG(dieArea.xmax,dieArea.ymax,FFs,"output.svg");
    cout<<clk->x<<" "<<clk->y<<endl;
    /*
    for(auto ff:FFs){
        cout<<ff->name<<" "<<ff->height<<" "<<ff->width<<endl;
    }
    cout<<dieArea.xmin<<" "<<dieArea.ymin<<" "<<dieArea.xmax<<" "<<dieArea.ymax<<endl;
   */
    
    initialized();
    
    divideDieAreaIntoBlocks();
    //cout<<blocks.size()<<endl;
    /*
    for (const auto& entry : blocks) {
        std::pair<int, int> coord = entry.first;
        Block* block = entry.second;
        cout<<coord.first<<" "<<coord.second<<endl;
        for(auto ff:block->nodes){
            cout<<ff->name<<" "<<ff->block_x<<" "<<ff->block_y<<endl;
        }
    }*/
     cout<<dieArea.l<<" "<<dieArea.w<<endl;
     cout<<clk->x<<" "<<clk->y<<endl;
    // cout<<"guhdkbnuk"<<endl;
     cout<<clk->height<<clk->width<<endl;
     //cout<<FFs[106030]->name<<" "<<FFs[106030]->x<<" "<<FFs[106030]->y<<endl;
     //char p=cin.get();
     iterartivecluster(FFs.size()/65+1,cfg.deltaexceed);
    

    // 检查是否所有的 Node 都被分配到了某个 Cluster 中
    /*if (areAllNodesClustered()) {
        cout << "All nodes are clustered." << endl;
    } else {
        cout << "Some nodes are not clustered." << endl;
    }*/
    
    
    divideIntoBlocks();
    initialbuffer1();
    cout<<BUF1.size()<<endl;
    #ifdef GENERATEBUF2BUFpatten
    generateSVG(dieArea.xmax,dieArea.ymax,BUF1,"output.svg");
    #endif
    
    //char b=cin.get();
    highlevelcluster();
    //cout<<"here"<<endl;
    
    
    ConstructInOrder();
     appendSVGNode(dieArea.xmax,dieArea.ymax,BUF1,"output.svg");
     appendSVGNode(dieArea.xmax,dieArea.ymax,BUF2,"output.svg");
    writeDEF("output.def");
    
    return 0;
}
     
