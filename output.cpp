#include "output.h"
#include <iomanip> 
// 生成输出 DEF 文件

void writeDEF(const string& filename) {
    for(auto buffer:BUF1)
    {
        buffers.push_back((Node*)buffer);
    }
    for(auto buffer:BUF2)
    {
        buffers.push_back((Node*)buffer);
    }
    ofstream outfile(filename);
    // 写入头部信息
    outfile << "UNITS DISTANCE MICRONS 1000 ;\n";
    outfile << std::fixed << std::setprecision(0) << "DIEAREA ( " << dieArea.xmin << " " << dieArea.ymin << " ) ( " << dieArea.xmin << " " << dieArea.ymax << " ) ( " << dieArea.xmax << " " << dieArea.ymax << " ) ( " << dieArea.xmax << " " << dieArea.ymin << " ) ;\n";
    outfile << "FF ( " << ffSize.width << " " << ffSize.height << " ) ;\n";
    outfile << "BUF ( " << bufSize.width << " " << bufSize.height << " ) ;\n";
    outfile << "CLK ( " << clk->x << " " << clk->y << " ) ;\n";
    // 写入组件信息
    outfile << "COMPONENTS " << FFs.size() + buffers.size() << " ;\n";
    for (auto ff : FFs) {
        outfile <<"- " << ff->name << " FF ( " << ff->x << " " << ff->y << " ) ;\n";
    }
    for (auto buf : buffers) {
        outfile << "- " << buf->name << " BUF ( " << (int)buf->x << " " << (int)buf->y << " ) ;\n";
    }
    outfile << "END COMPONENTS ;\n";
    // 写入 NETS 信息
    int net_count = 1;
    unordered_map<Node*, string> net_names;
    queue<Node*> q;
    q.push(clk);
    while (!q.empty()) {
        Node* driver = q.front();
        q.pop();
        if (!driver->children.empty()) {
            // 创建网络名称
            string net_name = "net_" + to_string(net_count);
            net_names[driver] = net_name;
            ++net_count;
        }
        for (auto child : driver->children) {
            q.push(child);
        }
    }
    net_count--;
    outfile << "NETS " << net_count << " ;\n";
    q.push(clk);
    while (!q.empty()) {
        Node* driver = q.front();
        q.pop();
        if (!driver->children.empty()) {
            string net_name = net_names[driver];
            outfile << "- " << net_name << " ( " << driver->name << " ) ( ";
            for (auto child : driver->children) {
                outfile << child->name << " ";
            }
            outfile << ") ;\n";
        }
        for (auto child : driver->children) {
            q.push(child);
        }
    }
    outfile << "END NETS ;\n";
    outfile.close();
}