#include "preprocess.h"


// 读取约束文件
void readConstraints(const string& filename) {
    ifstream infile(filename);
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string key, eq;
        double value;
        iss >> key >> eq >> value;
        if (key == "net_unit_r") constraint.net_unit_r = value;
        else if (key == "net_unit_c") constraint.net_unit_c = value;
        else if (key == "max_net_rc") constraint.max_net_rc = value;
        else if (key == "max_fanout") constraint.max_fanout = static_cast<int>(value);
        else if (key == "buffer_delay") constraint.buffer_delay = value;
        //cout<<value<<endl;
    }
    infile.close();
}

void readDEF(const string& filename) {
    ifstream infile(filename);
    string line;
    int mode = 0; // 0: header, 1: components, 2: end components
    int compCount = 0;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        istringstream iss(line);
        string token;
        iss >> token;
        if (token == "UNITS") {
            // 跳过
        }
        else if (token == "DIEAREA") {
            // 读取芯片区域
            string temp;
            //      (         0                  0          )       (       0       1000    )       (         1000              1000        
            iss >> temp >> dieArea.xmin >> dieArea.ymin >> temp >> temp >> temp >> temp >> temp >> temp >> dieArea.xmax >> dieArea.ymax;
        }
        else if (token == "FF") {
            // 读取 FF 尺寸
            string temp;
            iss >> temp >> ffSize.width >> ffSize.height;
        }
        else if (token == "BUF") {
            // 读取 BUF 尺寸
            string temp;
            iss >> temp >> bufSize.width >> bufSize.height;
        }
        else if (token == "CLK") {
            // 读取时钟源位置
            double x, y;
            string temp;
            iss >> temp >> x >> y;
            DriverNode* clock=new DriverNode();
            clock->block_x = -1;
            clock->block_y = -1;
            clock->cluster_id = -1;
            clock->delay = 0.0;
            clock->x = x;
            clock->y = y;
            clock->width = 0;
            clock->height = 0;
            clock->name = "CLK";
            clock->type = "CLK";
            clock->parent = nullptr;
            clock->rc = 0.0;
            clock->fanout = 0;
            clock->children = {};
            clock->updelay=1;  //确实已经update delay了
            clock->end=0;
            clk=(Node*)clock;
        }
        else if (token == "COMPONENTS") {
            // 开始读取组件
            mode = 1;
            iss >> compCount;
        }
        else if (token == "END" && mode == 1) {
            // 结束组件读取
            mode = 2;
        }
        else if (mode == 1) {
            // 读取组件信息
            if (token == "-") {
                string temp;
                string name, type;
                double x, y;
                iss >> name >> type;
                getline(iss, line);
                istringstream posStream(line);
                posStream >> temp >> x >> y;
                if (type == "FF") {
                    FFs.push_back(new Node{ name, type, x, y, ffSize.width, ffSize.height, {}, nullptr, 0.0 ,0 ,0 ,0 ,0,1});
                }
            }
        }
        // 忽略其他内容
    }
    infile.close();
}