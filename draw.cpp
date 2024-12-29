#include "common.h"
#include "newcommon.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib> // 用于rand()
#include <ctime>   // 用于time()
void generateSVG(double L, double W, const std::vector<Node*>& nodes, const std::string& filename) {
    // 定义SVG画布的尺寸（像素）
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;

    // 计算缩放因子，将布局尺寸缩放到画布尺寸
    double scaleX = canvasWidth / L;
    double scaleY = canvasHeight / W;
    double scale = std::min(scaleX, scaleY); // 保持比例

    // 计算偏移，确保布局在画布中居中
    double offsetX = (canvasWidth - L * scale) / 2;
    double offsetY = (canvasHeight - W * scale) / 2;

    // SVG文件开始
    std::ofstream svgFile(filename);
    if (!svgFile.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 写入SVG头部
    svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    svgFile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ";
    svgFile << "width=\"" << canvasWidth << "px\" height=\"" << canvasHeight << "px\" ";
    svgFile << "viewBox=\"0 0 " << canvasWidth << " " << canvasHeight << "\">\n";

    // 背景设为黑色
    svgFile << "<rect width=\"100%\" height=\"100%\" fill=\"black\" />\n";

    // 绘制节点
    for (const auto& node : nodes) {
        // 转换为SVG坐标系（y向下）
        double rectX = node->x * scale + offsetX;
        double rectY = (W - node->y - node->height) * scale + offsetY; // 将y从左下角翻转到左上角

        // 设置颜色
        std::string color = (node->type == "FF") ? "green" : "blue";

        // 绘制矩形
        svgFile << "<rect x=\"" << rectX << "\" y=\"" << rectY << "\" ";
        svgFile << "width=\"" << node->width * scale << "\" height=\"" << node->height * scale << "\" ";
        svgFile << "fill=\"" << color << "\" />\n";
    }

    // SVG文件结束
    svgFile << "</svg>";
    svgFile.close();

    std::cout << "SVG文件已生成: " << filename << std::endl;
}
void generateSVG(double L, double W, const std::vector<DriverNode*>& nodes, const std::string& filename) {
    // 定义SVG画布的尺寸（像素）
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;

    // 计算缩放因子，将布局尺寸缩放到画布尺寸
    double scaleX = canvasWidth / L;
    double scaleY = canvasHeight / W;
    double scale = std::min(scaleX, scaleY); // 保持比例

    // 计算偏移，确保布局在画布中居中
    double offsetX = (canvasWidth - L * scale) / 2;
    double offsetY = (canvasHeight - W * scale) / 2;

    // SVG文件开始
    std::ofstream svgFile(filename);
    if (!svgFile.is_open()) {
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 写入SVG头部
    svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n";
    svgFile << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" ";
    svgFile << "width=\"" << canvasWidth << "px\" height=\"" << canvasHeight << "px\" ";
    svgFile << "viewBox=\"0 0 " << canvasWidth << " " << canvasHeight << "\">\n";

    // 背景设为黑色
    svgFile << "<rect width=\"100%\" height=\"100%\" fill=\"black\" />\n";

    // 绘制节点
    for (const auto& node : nodes) {
        // 转换为SVG坐标系（y向下）
        double rectX = node->x * scale + offsetX;
        double rectY = (W - node->y - node->height) * scale + offsetY; // 将y从左下角翻转到左上角

        // 设置颜色
        std::string color = (node->type == "FF") ? "blue" : "red";

        // 绘制矩形
        svgFile << "<rect x=\"" << rectX << "\" y=\"" << rectY << "\" ";
        svgFile << "width=\"" << node->width * scale << "\" height=\"" << node->height * scale << "\" ";
        svgFile << "fill=\"" << color << "\" />\n";
    }

    // SVG文件结束
    svgFile << "</svg>";
    svgFile.close();

    std::cout << "SVG文件已生成: " << filename << std::endl;
}
void appendSVGNode(double L, double W, vector<DriverNode*> nodes, const std::string& filename) {
    // 定义SVG画布的尺寸（像素）
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;

    // 计算缩放因子，将布局尺寸缩放到画布尺寸
    double scaleX = canvasWidth / L;
    double scaleY = canvasHeight / W;
    double scale = std::min(scaleX, scaleY); // 保持比例

    // 计算偏移，确保布局在画布中居中
    double offsetX = (canvasWidth - L * scale) / 2;
    double offsetY = (canvasHeight - W * scale) / 2;

    // 打开SVG文件读取内容
    std::ifstream inFile(filename);
    if(!inFile.is_open()){
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 读取整个文件内容
    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();

    // 查找 </svg> 标签位置
    size_t pos = content.find("</svg>");
    if(pos == std::string::npos){
        std::cerr << "无效的SVG文件: 缺少 </svg> 标签。" << std::endl;
        return;
    }

    // 转换为SVG坐标系（y向下）
    for(auto node:nodes)
    {
    double rectX = node->x * scale + offsetX;
    double rectY = (W - node->y - node->height) * scale + offsetY; // 将y从左下角翻转到左上角

    // 设置颜色
    std::string color = (node->type == "FF") ? "green" : "blue";

    // 创建新的 <rect> 元素
    std::string newRect = "<rect x=\"" + std::to_string(rectX) + "\" y=\"" + 
                          std::to_string(rectY) + "\" width=\"" + 
                          std::to_string(node->width * scale) + "\" height=\"" + 
                          std::to_string(node->height * scale) + "\" fill=\"" + 
                          color + "\" />\n";

    // 在 </svg> 之前插入新的 <rect>
    content.insert(pos, newRect);
    }
    // 写回文件
    std::ofstream outFile(filename, std::ios::trunc);
    if(!outFile.is_open()){
        std::cerr << "无法写入文件: " << filename << std::endl;
        return;
    }

    outFile << content;
    outFile.close();

    std::cout << "已添加新节点到SVG文件: " << filename << std::endl;

    
    
}

// 新增函数：添加自定义颜色的 Node*
void appendSVGCustomNode(double L, double W, Node* node, const std::string& filename,string color) {
    // 定义SVG画布的尺寸（像素）
    const int canvasWidth = 1000;
    const int canvasHeight = 1000;

    // 计算缩放因子，将布局尺寸缩放到画布尺寸
    double scaleX = canvasWidth / L;
    double scaleY = canvasHeight / W;
    double scale = std::min(scaleX, scaleY); // 保持比例

    // 计算偏移，确保布局在画布中居中
    double offsetX = (canvasWidth - L * scale) / 2;
    double offsetY = (canvasHeight - W * scale) / 2;

    // 打开SVG文件读取内容
    std::ifstream inFile(filename);
    if(!inFile.is_open()){
        std::cerr << "无法打开文件: " << filename << std::endl;
        return;
    }

    // 读取整个文件内容
    std::string content((std::istreambuf_iterator<char>(inFile)),
                         std::istreambuf_iterator<char>());
    inFile.close();

    // 查找 </svg> 标签位置
    size_t pos = content.find("</svg>");
    if(pos == std::string::npos){
        std::cerr << "无效的SVG文件: 缺少 </svg> 标签。" << std::endl;
        return;
    }

    // 转换为SVG坐标系（y向下）
    double rectX = node->x * scale + offsetX;
    double rectY = (W - node->y - node->height) * scale + offsetY; // 将y从左下角翻转到左上角

    // 使用节点的颜色，如果未指定则根据类型设置默认颜色
    

    // 创建新的 <rect> 元素
    std::string newRect = "<rect x=\"" + std::to_string(rectX) + "\" y=\"" + 
                          std::to_string(rectY) + "\" width=\"" + 
                          std::to_string(node->width * scale) + "\" height=\"" + 
                          std::to_string(node->height * scale) + "\" fill=\"" + 
                          color + "\" />\n";

    // 在 </svg> 之前插入新的 <rect>
    content.insert(pos, newRect);

    // 写回文件
    std::ofstream outFile(filename, std::ios::trunc);
    if(!outFile.is_open()){
        std::cerr << "无法写入文件: " << filename << std::endl;
        return;
    }

    outFile << content;
    outFile.close();

    std::cout << "已添加自定义节点到SVG文件: " << filename << std::endl;
}