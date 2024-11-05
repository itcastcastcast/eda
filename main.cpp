#include "preprocess.h"
#include "cluster.h"
#include "parameter.h"


vector<Node*> FFs;
vector<Node*> buffers;
Node* clk = nullptr;
Constraint constraint;
DieArea dieArea;
ComponentSize ffSize;
ComponentSize bufSize;
map<pair<int, int>, Block*> blocks;
vector<Block*>searchorder;



int main(){
    string def_filename = "cts_problems/case5/problem.def";
    string constraints_filename = "cts_problems/case5/constraints.txt";
    //string output_filename = "output.def";
    readConstraints(constraints_filename);
    readDEF(def_filename);
    cout<<"attention here"<<endl;
    cout<<clk->x<<" "<<clk->y<<endl;
    /*
    for(auto ff:FFs){
        cout<<ff->name<<" "<<ff->height<<" "<<ff->width<<endl;
    }
    cout<<dieArea.xmin<<" "<<dieArea.ymin<<" "<<dieArea.xmax<<" "<<dieArea.ymax<<endl;
   */
  
    initialized();
    
    divideDieAreaIntoBlocks();
    
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
     cout<<"guhdkbnuk"<<endl;
     iterartivecluster(2000,cfg.deltaexceed);
     //meancluster(5000,80000);
     
     //getorder();
}