#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <queue>
#include <set>
#include<map>
#include <memory>
#include<algorithm>
#include<cassert>
#include <unordered_set>

using namespace std;

// 数据结构定义

struct Node {
    string name;
    string type; // "FF", "BUF", "CLK"
    double x;
    double y;
    int width;
    int height;
    vector<Node*> children; // 扇出节点
    Node* parent;           // 驱动节点
    double delay;           // 从时钟源到该节点的延迟
    int block_x;            // 所属块的横坐标
    int block_y;            // 所属块的纵坐标
    int cluster_id;         // 所属聚类的 ID
    bool updelay;  //已经updatedelay了吗？
    bool end;   //节省时间
};

struct Constraint {
    double net_unit_r;
    double net_unit_c;
    double max_net_rc;
    int max_fanout;
    double buffer_delay;
};

struct DieArea {
    double xmin, ymin;
    double xmax, ymax;
    double l;
    double w;
    double block_width;
    double block_height;
};

struct ComponentSize {
    string type;
    int width;
    int height;
};

struct ClusterCenter {
    double x;
    double y;
};

struct Cluster {
    ClusterCenter center;             // 聚类中心
    vector<Node*> members;            // 属于该聚类的 buffer 节点

    // 构造函数
    Cluster(double x, double y) {
        center = {x, y};
    }

    // 清空成员
    void clear() {
        members.clear();
    }
};

struct configurable{
    int K;
    double bound;  //bound初始值
    int l;
    int w;
    int deltabound;  //bound如何增加
    int deltaexceed; //exceed如何增加 
    double maxbound;
    //int hk;
    double deltaarea;
    double portion;
    double low;      //low和up由实时决定
    double up;
    double bear;
};

class Block {
    public:
    pair<int, int> coordinates;       // 块的坐标 (block_x, block_y)
    vector<Node*> nodes;              // 块内的节点
    vector<Cluster*> clusters;         // 块内的聚类
};
// 全局变量
// Derived struct for FF nodes
struct DriverNode : public Node {
    int level;
    int fanout;
    double rc;
};

//extern std::vector<Cluster*> clusters; // 用于存储聚类

extern map<pair<int, int>, Block*> blocks;

// 全局变量
extern std::vector<Node*> FFs;
extern std::vector<Node*> buffers;
extern Node* clk;
extern Constraint constraint;
extern DieArea dieArea;
extern ComponentSize ffSize;
extern ComponentSize bufSize;
extern configurable cfg;

extern double ManhattonDistance(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2);
extern double rc(double x1,double y1,double x2,double y2,double xsize1,double xsize2,double ysize1,double ysize2);
extern Node* initBuffer(double x, double y, double width, double height);
extern void getorder();
extern double rc(Node *tmp1, Node*tmp2);
extern double ManhattonDistance(Node *tmp1,Node*tmp2);
extern double delay(Node*tmp1,Node*tmp2);
#endif // COMMON_H
