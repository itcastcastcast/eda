import re
from collections import defaultdict, deque

# 读取约束参数
def read_constraints(file_path):
    with open(file_path, 'r') as f:
        constraints = f.read()
    net_unit_r = float(re.search(r'net_unit_r\s*=\s*([\d.e-]+)', constraints).group(1))
    net_unit_c = float(re.search(r'net_unit_c\s*=\s*([\d.e-]+)', constraints).group(1))
    buffer_delay = float(re.search(r'buffer_delay\s*=\s*([\d.e-]+)', constraints).group(1))
    return net_unit_r, net_unit_c, buffer_delay

# 解析solution.def文件，获取缓冲器位置和连接关系
def parse_solution_def(file_path):
    buffer_positions = {}
    connections = defaultdict(list)
    
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            # 解析缓冲器位置
            buf_match = re.match(r'BUF(\d+)\s+BUF\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if buf_match:
                buf_id = f'BUF{buf_match.group(1)}'
                x = int(buf_match.group(2))
                y = int(buf_match.group(3))
                buffer_positions[buf_id] = (x, y)
                continue
            
            # 解析net连接关系
            net_match = re.match(r'net_\d+\s*\(\s*(BUF\d+)\s*\)\s*\(([^)]+)\)\s*;', line)
            if net_match:
                source = net_match.group(1)
                targets = net_match.group(2).split()
                connections[source].extend(targets)
    
    return buffer_positions, connections

# 解析components.def文件，获取触发器(FF)位置
def parse_components_def(file_path):
    ff_positions = {}
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            ff_match = re.match(r'(FF\d+)\s+位置\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if ff_match:
                ff_id = ff_match.group(1)
                x = int(ff_match.group(2))
                y = int(ff_match.group(3))
                ff_positions[ff_id] = (x, y)
    return ff_positions

# 曼哈顿距离计算
def manhattan_distance(p1, p2):
    return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1])

# 计算从CLK到每个FF的延迟
def calculate_delays(clk_pos, buffer_positions, connections, ff_positions, net_unit_r, net_unit_c, buffer_delay):
    delays = {}
    visited = set()

    # BFS队列，每个元素是（当前节点, 累计延迟, 缓冲器数量）
    queue = deque()
    queue.append( ( 'CLK', clk_pos, 0.0, 0 ) )
    visited.add('CLK')
    
    while queue:
        current_node, current_pos, current_delay, buffer_count = queue.popleft()
        
        # 连接到的下一个节点
        for next_node in connections.get(current_node, []):
            if next_node.startswith('BUF'):
                if next_node not in buffer_positions:
                    continue
                next_pos = buffer_positions[next_node]
                distance = manhattan_distance(current_pos, next_pos)
                segment_delay = 0.5 * net_unit_r * net_unit_c * (distance ** 2)
                total_delay = current_delay + segment_delay + buffer_delay
                if next_node not in visited:
                    queue.append( ( next_node, next_pos, total_delay, buffer_count + 1 ) )
                    visited.add(next_node)
            elif next_node.startswith('FF'):
                if next_node not in ff_positions:
                    continue
                ff_pos = ff_positions[next_node]
                distance = manhattan_distance(current_pos, ff_pos)
                segment_delay = 0.5 * net_unit_r * net_unit_c * (distance ** 2)
                total_delay = current_delay + segment_delay
                if next_node not in delays or total_delay < delays[next_node]:
                    delays[next_node] = total_delay
    
    return delays

def main():
    # 文件路径
    constraints_file = 'case10/constraints.txt'
    solution_def_file = 'case10/solution.def'
    
    
    # 读取约束参数
    net_unit_r, net_unit_c, buffer_delay = read_constraints(constraints_file)
    
    # 时钟位置
    clk_pos = (0, 802177)
    
    # 解析solution.def文件
    buffer_positions, connections = parse_solution_def(solution_def_file)
    
    # 将CLK作为初始节点，假设CLK连接到BUF0或其他缓冲器
    # 如果有特定的连接，请根据实际情况调整
    # 例如，假设CLK连接到BUF0
    # connections['CLK'] = ['BUF0']
    
    # 解析components.def文件，获取FF位置
    ff_positions = parse_components_def(solution_def_file)
    
    # 计算延迟
    delays = calculate_delays(clk_pos, buffer_positions, connections, ff_positions, net_unit_r, net_unit_c, buffer_delay)
    
    if delays:
        max_delay = max(delays.values())
        min_delay = min(delays.values())
        range_delay = max_delay - min_delay
        
        print(f"最大时延: {max_delay}")
        print(f"最小时延: {min_delay}")
        print(f"极差: {range_delay}")
    else:
        print("未找到有效的时延数据。")

if __name__ == "__main__":
    main()