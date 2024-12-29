import os
import re
import matplotlib.pyplot as plt
import numpy as np

# 更新后的正则表达式，包含 Overlap Inst Number 和 Area
patterns = {
    'clock_skew': r"Clock Global Skew:\s*([\d\.]+)\s*ps",
    'clock_latency': r"Average clock latency:\s*([\d\.]+)\s*ps",
    'buffer_count': r"Buffer Count:\s*(\d+)",
    'overlap_inst_number': r"Overlap Inst Number:\s*(\d+)",
}

# 函数：从文件中提取数据
def extract_data_from_file(file_path):
    data = {}
    
    with open(file_path, 'r') as file:
        content = file.read()

        # 使用正则表达式从文件内容中提取信息
        for key, pattern in patterns.items():
            match = re.search(pattern, content)
            if match:
                data[key] = match.group(1)
    
    return data

# 函数：读取目录中的所有文件并提取数据
def read_files_from_directory(directory_path):
    data_list = []
    filenames = []
    
    # 获取文件夹中的所有文件
    for filename in os.listdir(directory_path):
        file_path = os.path.join(directory_path, filename)
        
        # 仅处理文件，不处理子目录
        if os.path.isfile(file_path):
            data = extract_data_from_file(file_path)
            if data:  # 确保数据不为空
                data_list.append(data)
                filenames.append(filename)  # 收集文件名
    
    return data_list, filenames

# 函数：可视化数据
def visualize_data(data_list, filenames):
    # 提取数据
    clock_skews = [float(data.get('clock_skew', 0)) for data in data_list]
    clock_latencies = [float(data.get('clock_latency', 0)) for data in data_list]
    buffer_counts = [int(data.get('buffer_count', 0)) for data in data_list]

    # 绘制每个指标的单独图表，并增大图形尺寸
    metrics = {
        "Clock Global Skew (ps)": clock_skews,
        "Average Clock Latency (ps)": clock_latencies,
        "Buffer Count": buffer_counts,
    }

    for metric_name, metric_data in metrics.items():
        plt.figure(figsize=(10, 6))  # 增大图形尺寸
        plt.bar(filenames, metric_data, color='skyblue')

        plt.title(f"{metric_name}")
        plt.xlabel("File Name")
        plt.ylabel(f"{metric_name}")  # 在纵坐标上显示单位

        # 设置纵坐标为更详细的刻度
        plt.tick_params(axis='x', rotation=90)  # 旋转X轴标签（90度），确保竖直显示
        plt.xticks(np.arange(len(filenames)), filenames, rotation=90)

        # 动态设置y轴刻度
        plt.ylim(min(metric_data) - 5, max(metric_data) + 5)  # 留出一定空白
        plt.grid(True, axis='y', linestyle='--', alpha=0.7)  # 添加y轴网格线

        plt.tight_layout()
        plt.show()

# 主函数
def main():
    directory_path = './generated/REPORT'  # 文件夹路径
    data_list, filenames = read_files_from_directory(directory_path)
    
    # 如果没有数据，则打印警告并退出
    if not data_list:
        print("没有数据可供绘制。请检查目录中的文件内容。")
        return
    
    visualize_data(data_list, filenames)

# 执行主函数
if __name__ == "__main__":
    main()
