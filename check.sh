#!/bin/bash
sudo chmod 777 ./cts_checker
mkdir -p generated/REPORT
# 循环遍历 case1 到 case10
for i in {1..10}; do
    case_dir="cts_problems/case${i}"
    echo "正在执行./cts_checker ${case_dir}"
    while [ ! -e "${case_dir}/solution.def" ]; do
    echo "目录 ${case_dir}/solution.def 不存在，等待中..."
    sleep 60  # 等待5秒后重试
    done

    # 执行程序并传入案例目录
    ./cts_checker "${case_dir}"

    # 检查程序是否成功执行
    if [ $? -ne 0 ]; then
        echo "执行 case${i} 失败。"
        exit 1
    fi
    mv "${case_dir}/report.txt" "generated/REPORT/cts_problems_case${i}_report.txt"
done

for i in {1..3}; do
    case_dir="pre_submit/case${i}"
    echo "正在执行./cts_checker ${case_dir}"

    while [ ! -e "${case_dir}/solution.def" ]; do
    echo "目录 ${case_dir}/solution.def 不存在，等待中..."
    sleep 60  # 等待60秒后重试
    done

    # 执行程序并传入案例目录
    ./cts_checker "${case_dir}"
    if [ $? -ne 0 ]; then
        echo "执行 case${i} 失败。"
        exit 1
    fi
    mv "${case_dir}/report.txt" "generated/REPORT/pre_submit_case${i}_report.txt"
done

echo "所有案例已成功执行。"

