#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <set>

struct Rectangle {
    double x;      // 左下角 x 坐标
    double y;      // 左下角 y 坐标
    double width;  // 宽度（980 或 2660）
    double height; // 高度（900）
};

// 计算曼哈顿距离
double manhattan_distance(double x1, double y1, double x2, double y2) {
    return fabs(x1 - x2) + fabs(y1 - y2);
}

// 判断是否重叠超过允许的面积
bool is_overlap(const Rectangle& a, const Rectangle& b, double max_overlap_area) {
    double x_overlap = std::max(0.0, std::min(a.x + a.width, b.x + b.width) - std::max(a.x, b.x));
    double y_overlap = std::max(0.0, std::min(a.y + a.height, b.y + b.height) - std::max(a.y, b.y));
    double overlap_area = x_overlap * y_overlap;
    return overlap_area > max_overlap_area;
}

struct Position {
    double x;
    double y;
    double distance; // 到理想位置的曼哈顿距离
    bool operator<(const Position& other) const {
        return distance > other.distance; // 小根堆
    }
};

Rectangle find_best_position(double ideal_x, double ideal_y,
    const std::vector<Rectangle>& existing_rects,
    double buffer_width, double buffer_height,
    double max_overlap_area) {
    std::priority_queue<Position> pq;
    std::set<std::pair<int, int>> visited; // 记录已访问的位置
    double grid_size = buffer_width / 2.0; // 网格大小，可以根据需求调整

    pq.push({ ideal_x, ideal_y, 0.0 });
    visited.insert({ static_cast<int>(ideal_x / grid_size), static_cast<int>(ideal_y / grid_size) });

    while (!pq.empty()) {
        Position pos = pq.top();
        pq.pop();

        Rectangle buffer_rect = { pos.x, pos.y, buffer_width, buffer_height };
        bool acceptable = true;

        for (const auto& rect : existing_rects) {
            if (is_overlap(buffer_rect, rect, max_overlap_area)) {
                acceptable = false;
                break;
            }
        }

        if (acceptable) {
            return buffer_rect;
        }

        // 生成相邻位置（上下左右）
        std::vector<std::pair<double, double>> directions = {
            {pos.x + grid_size, pos.y},
            {pos.x - grid_size, pos.y},
            {pos.x, pos.y + grid_size},
            {pos.x, pos.y - grid_size}
        };

        for (const auto& dir : directions) {
            int grid_x = static_cast<int>(dir.first / grid_size);
            int grid_y = static_cast<int>(dir.second / grid_size);
            if (visited.count({ grid_x, grid_y }) == 0) {
                double distance = manhattan_distance(ideal_x, ideal_y, dir.first, dir.second);
                pq.push({ dir.first, dir.second, distance });
                visited.insert({ grid_x, grid_y });
            }
        }
    }

    // 未找到合适的位置
    return { -1, -1, buffer_width, buffer_height };
}

int main() {
    double ideal_x = 1000.0;
    double ideal_y = 1000.0;
    double buffer_width = 980.0;
    double buffer_height = 900.0;
    double max_overlap_area = buffer_width * buffer_height * 0.03; // 重叠面积阈值（3%）

    // 已有长方形列表（类型 1 和类型 2）
    std::vector<Rectangle> existing_rects = {
        {500.0, 500.0, 2660.0, 900.0},  // FF
        {2000.0, 2000.0, 980.0, 900.0}  // 现有的 BUF
    };

    // 查找最佳位置
    Rectangle buffer_rect = find_best_position(ideal_x, ideal_y, existing_rects,
        buffer_width, buffer_height,
        max_overlap_area);

    if (buffer_rect.x >= 0 && buffer_rect.y >= 0) {
        std::cout << "Buffer 放置于 (" << buffer_rect.x << ", " << buffer_rect.y << ")\n";
    }
    else {
        std::cout << "未找到合适的 Buffer 放置位置。\n";
    }

    return 0;
}