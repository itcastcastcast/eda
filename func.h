#include "common.h"
double manhattanDistance(double x1, double y1, double x2, double y2);

bool checkRCConstraint(Node* driver, const std::vector<Node*>& sinks);

bool checkFanoutConstraint(const std::vector<Node*>& sinks);

bool checkOverlap(Node* buffer, const std::vector<Node*>& components);

void calculateDelay(Node* node);