import math
from collections import defaultdict, deque
import re

# Parse constraints.txt
constraints = {}
with open('/home/jinm/eda/solver/project/cts_problems/case5/constraints.txt', 'r') as f:
    for line in f:
        if '=' in line:
            key, value = line.strip().split('=')
            constraints[key.strip()] = float(value.strip())

r = constraints.get('net_unit_r')
c = constraints.get('net_unit_c')

# Parse solution.def
nodes = {}
edges = defaultdict(list)
clk_position = None

with open('/home/jinm/eda/solver/project/cts_problems/case5/solution.def', 'r') as f:
    for line in f:
        line = line.strip()
        if line.startswith('NETS'):
            continue  # Skip NETS declaration
        if line.startswith('- net_'):
            match = re.match(r'-\s+net_\d+\s*\(\s*(BUF\d+|CLK)\s*\)\s*\(\s*([^)]+)\s*\)\s*;', line)
            if match:
                src = match.group(1)
                targets = match.group(2).split()
                for tgt in targets:
                    edges[src].append(tgt)
        elif line.startswith('CLK'):
            match = re.match(r'CLK\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if match:
                clk_position = ('CLK', int(match.group(1)), int(match.group(2)))
                nodes['CLK'] = clk_position[1:]
        elif line.startswith('- FF'):
            match = re.match(r'-\s+(FF\d+)\s+FF\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if match:
                ff_id = match.group(1)
                x, y = int(match.group(2)), int(match.group(3))
                nodes[ff_id] = (x, y)
        elif line.startswith('- BUF'):
            match = re.match(r'-\s+(BUF\d+)\s+BUF\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if match:
                buf_id = match.group(1)
                x, y = int(match.group(2)), int(match.group(3))
                nodes[buf_id] = (x, y)

# Calculate distance
def calculate_distance(x1, y1, x2, y2, tgt):
    if r is None or c is None:
        return 0
    base_distance = 0.69 * 0.5 * r * c * (abs(x1 - x2) + abs(y1 - y2))**2
    if tgt.startswith('BUF'):
        base_distance += 25.69
    return base_distance

# Build graph with distances
graph = defaultdict(list)
for src, targets in edges.items():
    for tgt in targets:
        if src in nodes and tgt in nodes:
            distance = calculate_distance(nodes[src][0], nodes[src][1], nodes[tgt][0], nodes[tgt][1],tgt)
            graph[src].append((tgt, distance))

# Debugging: Print nodes and graph
print("Nodes:")
for node, coords in nodes.items():
    print(f"{node}: {coords}")

print("\nGraph Connections with Distances:")
for src, targets in graph.items():
    for tgt, dist in targets:
        print(f"{src} -> {tgt} : {dist}")

# Graph traversal for shortest and longest paths
def bfs_shortest_paths(graph, start):
    distances = {start: 0}
    paths = {start: [start]}
    queue = deque([start])
    while queue:
        current = queue.popleft()
        for neighbor, dist in graph[current]:
            if neighbor not in distances or distances[neighbor] > distances[current] + dist:
                distances[neighbor] = distances[current] + dist
                paths[neighbor] = paths[current] + [neighbor]
                queue.append(neighbor)
    return distances, paths

def dfs_longest_path(graph, start):
    visited = set()
    max_distance = 0
    max_path = []

    def dfs(current, distance, path):
        nonlocal max_distance, max_path
        visited.add(current)
        if current.startswith('FF') and distance > max_distance:
            max_distance = distance
            max_path = path.copy()
        for neighbor, dist in graph[current]:
            if neighbor not in visited:
                dfs(neighbor, distance + dist, path + [neighbor])
        visited.remove(current)

    dfs(start, 0, [start])
    return max_distance, max_path



# Find all FF nodes
ff_nodes = [node for node in nodes if node.startswith('FF')]

# Shortest paths
short_distances, short_paths = bfs_shortest_paths(graph, 'CLK')


# Longest path
max_distance, max_path = dfs_longest_path(graph, 'CLK')

# Output shortest paths
print("\nShortest Paths:")
for ff in ff_nodes:
    short_path = short_paths.get(ff, 'No path')
    short_distance = short_distances.get(ff, 'N/A')
    
    if isinstance(short_path, list):
        short_path_str = ' -> '.join(short_path)
    else:
        short_path_str = short_path
    
    print(f"Shortest path to {ff}: {short_path_str} with distance {short_distance}")

# Output longest path

print("\nlongest Path:")
if max_path:
    max_path_str = ' -> '.join(max_path)
    print(f"Longest path: {max_path_str} with distance {max_distance}")
else:
    print("No longest path found.")