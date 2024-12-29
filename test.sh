#!/bin/bash
rm -rf generated
mkdir -p generated/SVG
# 循环遍历 case1 到 case10
for i in {1..10}; do
    case_dir="cts_problems/case${i}"
    if [ -e "${case_dir}/solution.def" ];then
    rm "${case_dir}/solution.def"
    fi
done
for i in {1..3}; do
    case_dir="pre_submit/case${i}"
    if [ -e "${case_dir}/solution.def" ];then
    rm "${case_dir}/solution.def"
    fi
done
for i in {1..10}; do
    case_dir="cts_problems/case${i}"
    echo "正在执行 ./main ${case_dir}"

    # 执行程序并传入案例目录
    ./main "${case_dir}" > /dev/null 
    mv output.def "${case_dir}/solution.def"
    # 检查程序是否成功执行
    if [ $? -ne 0 ]; then
        echo "执行 case${i} 失败。"
        exit 1
    fi
    mv "output.svg" "generated/SVG/cts_problems_case${i}_result.svg"
done

for i in {1..3}; do
    case_dir="pre_submit/case${i}"
    echo "正在执行 ./main ${case_dir}"

    # 执行程序并传入案例目录
    ./main "${case_dir}" > /dev/null 
    mv output.def "${case_dir}/solution.def"
    # 检查程序是否成功执行
    if [ $? -ne 0 ]; then
        echo "执行 case${i} 失败。"
        exit 1
    fi
    mv "output.svg" "generated/SVG/pre_submit_case${i}_result.svg"
done

echo "所有案例已成功执行。"
