import re

# 初始化最短和最长路径的列表
min_distances = [float('inf'), float('inf'), float('inf')]
min_paths = ["", "", ""]
max_distances = [-float('inf'), -float('inf'), -float('inf')]
max_paths = ["", "", ""]

with open('/home/jinm/eda/solver/project/cts_problems/case5/a.txt', 'r') as file:
    for line in file:
        match = re.search(r'with distance ([\d.]+)', line)
        if match:
            distance = float(match.group(1))
            # 更新最短路径
            if distance < min_distances[0]:
                min_distances[2] = min_distances[1]
                min_paths[2] = min_paths[1]
                min_distances[1] = min_distances[0]
                min_paths[1] = min_paths[0]
                min_distances[0] = distance
                min_paths[0] = line.strip()
            elif distance < min_distances[1]:
                min_distances[2] = min_distances[1]
                min_paths[2] = min_paths[1]
                min_distances[1] = distance
                min_paths[1] = line.strip()
            elif distance < min_distances[2]:
                min_distances[2] = distance
                min_paths[2] = line.strip()
            
            # 更新最长路径
            if distance > max_distances[0]:
                max_distances[2] = max_distances[1]
                max_paths[2] = max_paths[1]
                max_distances[1] = max_distances[0]
                max_paths[1] = max_paths[0]
                max_distances[0] = distance
                max_paths[0] = line.strip()
            elif distance > max_distances[1]:
                max_distances[2] = max_distances[1]
                max_paths[2] = max_paths[1]
                max_distances[1] = distance
                max_paths[1] = line.strip()
            elif distance > max_distances[2]:
                max_distances[2] = distance
                max_paths[2] = line.strip()

# 输出结果
print(f"最短路径1: {min_paths[0]}，距离: {min_distances[0]}")
print(f"最短路径2: {min_paths[1]}，距离: {min_distances[1]}")
print(f"最短路径3: {min_paths[2]}，距离: {min_distances[2]}")
print(f"最长路径1: {max_paths[0]}，距离: {max_distances[0]}")
print(f"最长路径2: {max_paths[1]}，距离: {max_distances[1]}")
print(f"最长路径3: {max_paths[2]}，距离: {max_distances[2]}")