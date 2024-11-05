#include "func.h"
// 计算曼哈顿距离
double manhattanDistance(double x1, double y1, double x2, double y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

// 检查 RC 约束
bool checkRCConstraint(Node* driver, const vector<Node*>& sinks) {
    double total_rc = 0.0;
    for (auto sink : sinks) {
        double dist = manhattanDistance(driver->x, driver->y, sink->x, sink->y);
        total_rc += 0.5 * pow(dist, 2) * constraint.net_unit_r * constraint.net_unit_c;
    }
    return total_rc <= constraint.max_net_rc;
}

// 检查扇出约束
bool checkFanoutConstraint(const vector<Node*>& sinks) {
    return sinks.size() <= constraint.max_fanout;
}

// 检查缓冲器放置是否与已有组件重叠
bool checkOverlap(Node* buffer, const vector<Node*>& components) {
    double buf_left = buffer->x - buffer->width / 2.0;
    double buf_right = buffer->x + buffer->width / 2.0;
    double buf_bottom = buffer->y - buffer->height / 2.0;
    double buf_top = buffer->y + buffer->height / 2.0;
    for (auto comp : components) {
        double comp_left = comp->x - comp->width / 2.0;
        double comp_right = comp->x + comp->width / 2.0;
        double comp_bottom = comp->y - comp->height / 2.0;
        double comp_top = comp->y + comp->height / 2.0;
        // 检查重叠
        if (buf_left < comp_right && buf_right > comp_left &&
            buf_bottom < comp_top && buf_top > comp_bottom) {
            return true;
        }
    }
    return false;
}

void calculateDelay(Node* node) {
    if (node->parent) {
        double dist = manhattanDistance(node->x, node->y, node->parent->x, node->parent->y);
        double segment_delay = 0.69 * 0.5 * pow(dist, 2) * constraint.net_unit_r * constraint.net_unit_c;
        node->delay = node->parent->delay + segment_delay + constraint.buffer_delay;
    }
    for (auto child : node->children) {
        calculateDelay(child);
    }
}