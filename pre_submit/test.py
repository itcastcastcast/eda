import re
from collections import defaultdict, deque

FFs=[]

# 读取约束参数
def read_constraints(file_path):
    with open(file_path, 'r') as f:
        constraints = f.read()
    net_unit_r = float(re.search(r'net_unit_r\s*=\s*([\d.e-]+)', constraints).group(1))
    net_unit_c = float(re.search(r'net_unit_c\s*=\s*([\d.e-]+)', constraints).group(1))
    buffer_delay = float(re.search(r'buffer_delay\s*=\s*([\d.e-]+)', constraints).group(1))
    print(f"约束参数读取完成: net_unit_r={net_unit_r}, net_unit_c={net_unit_c}, buffer_delay={buffer_delay}")
    return net_unit_r, net_unit_c, buffer_delay

# 解析solution.def文件，获取缓冲器位置和连接关系
def parse_solution_def(file_path):
    buffer_positions = {}
    connections = defaultdict(list)
    
    # 修改正则表达式以允许行以 '-' 开头并有可选的空格
    buf_position_pattern = re.compile(r'^-?\s*BUF(\d+)\s+BUF\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;')
    # 正则表达式匹配连接关系，例如: net_13682 ( CLK ) ( BUF13681 BUF13680 ) ;
    net_connection_pattern = re.compile(r'^-?\s*net_\d+\s*\(\s*(CLK|BUF\d+)\s*\)\s*\(([^)]+)\)\s*;')
    
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            # 解析缓冲器位置
            buf_match = buf_position_pattern.match(line)
            if buf_match:
                buf_id = f'BUF{buf_match.group(1)}'
                x = int(buf_match.group(2))
                y = int(buf_match.group(3))
                buffer_positions[buf_id] = (x, y)
                print(f"解析到缓冲器: {buf_id} 在位置 ({x}, {y})")
                continue
            
            # 解析net连接关系
            net_match = net_connection_pattern.match(line)
            if net_match:
                source = net_match.group(1)
                targets = net_match.group(2).split()
                connections[source].extend(targets)
                print(f"连接关系: {source} 连接到 {targets}")
    
    print(f"总共解析到 {len(buffer_positions)} 个缓冲器")
    return buffer_positions, connections

# 解析components.def文件，获取触发器(FF)位置
def parse_components_def(file_path):
    ff_positions = {}
    with open(file_path, 'r') as f:
        for line in f:
            line = line.strip()
            ff_match = re.match(r'^-?\s*(FF\d+)\s+FF\s*\(\s*(\d+)\s+(\d+)\s*\)\s*;', line)
            if ff_match:
                ff_id = ff_match.group(1)
                x = int(ff_match.group(2))
                y = int(ff_match.group(3))
                ff_positions[ff_id] = (x, y)
                print(f"解析到触发器: {ff_id} 在位置 ({x}, {y})")
    print(f"总共解析到 {len(ff_positions)} 个触发器")
    return ff_positions

# 曼哈顿距离计算
def manhattan_distance(p1, p2):
    return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1])

# 计算从CLK到每个FF的延迟
def calculate_delays(clk_pos, buffer_positions, connections, ff_positions, net_unit_r, net_unit_c, buffer_delay):
    delays = {}
    visited = set()

    # BFS队列，每个元素是（当前节点, 当前坐标, 累计延迟, 缓冲器数量）
    queue = deque()
    queue.append( ('CLK', clk_pos, 0.0, 0) )
    visited.add('CLK')
    print("开始BFS搜索延迟...")

    while queue:
        current_node, current_pos, current_delay, buffer_count = queue.popleft()
        print(f"正在处理节点: {current_node}, 当前延迟: {current_delay}, 缓冲器数量: {buffer_count}")

        # 连接到的下一个节点
        for next_node in connections.get(current_node, []):
            if next_node.startswith('BUF'):
                if next_node not in buffer_positions:
                    print(f"缓冲器 {next_node} 的位置未找到，跳过")
                    continue
                next_pos = buffer_positions[next_node]
                distance = manhattan_distance(current_pos, next_pos)
                segment_delay = 0.69*0.5 * net_unit_r * net_unit_c * (distance ** 2)
                total_delay = current_delay + segment_delay + buffer_delay
                print(f"从 {current_node} 到 {next_node} 的距离: {distance}, 延迟: {segment_delay:.6f}, 累计延迟: {total_delay:.6f}")
                if next_node not in visited:
                    queue.append( (next_node, next_pos, total_delay, buffer_count + 1) )
                    visited.add(next_node)
            elif next_node.startswith('FF'):
                
                if next_node not in ff_positions:
                    print(f"触发器 {next_node} 的位置未找到，跳过")
                    continue
                ff_pos = ff_positions[next_node]
                distance = manhattan_distance(current_pos, ff_pos)
                segment_delay = 0.69 *0.5* net_unit_r * net_unit_c * (distance ** 2)
                total_delay = current_delay + segment_delay
                print(f"从 {current_node} 到 {next_node} 的距离: {distance}, 延迟: {segment_delay:.6f}, 累计延迟: {total_delay:.6f}")
                if next_node not in delays or total_delay < delays[next_node]:
                    delays[next_node] = total_delay
                    print(f"更新触发器 {next_node} 的延迟为 {total_delay:.6f}")

    print("BFS搜索完成")
    return delays

def main():
    # 文件路径
    constraints_file = 'case1/constraints.txt'
    solution_def_file = 'output.def'
    components_def_file = 'output.def'  # 假设触发器位置在 components.def 中

    # 读取约束参数
    net_unit_r, net_unit_c, buffer_delay = read_constraints(constraints_file)

    # 时钟位置
    clk_pos = (0, 515580)
    print(f"时钟位置: {clk_pos}")
    ff_positions = parse_components_def(components_def_file)
    # 解析 solution.def 文件
    buffer_positions, connections = parse_solution_def(solution_def_file)
    print("stop")
    print(len(buffer_positions))
    for i in range(len(buffer_positions)):
        buf_str="BUF"+str(i+1)
        if buf_str not in connections:
            print("缓冲器"+buf_str+"未找到")
            

    # 将 CLK 作为初始节点的连接（如果 `solution.def` 中已经包含 CLK 的连接，则无需手动添加）
    # 如果需要手动添加连接，可以取消下面注释并进行调整
    # connections['CLK'] = ['BUF5760']
    # print(f"添加时钟到 BUF5760 的连接: CLK -> BUF5760")

    # 解析 components.def 文件，获取 FF 位置
    

    # 计算延迟
    delays = calculate_delays(clk_pos, buffer_positions, connections, ff_positions, net_unit_r, net_unit_c, buffer_delay)
   
    if delays:
        max_delay = max(delays.values())
        min_delay = min(delays.values())
        range_delay = max_delay - min_delay

        print(f"最大时延: {max_delay:.6f}")
        print(f"最小时延: {min_delay:.6f}")
        print(f"极差: {range_delay:.6f}")
    else:
        print("未找到有效的时延数据。")

    
if __name__ == "__main__":
    main()