from sympy import symbols, expand, simplify, solve

# 定义符号变量
x,s0,s1,s2= symbols('x s0 s1 s2')

# 符号表达式
expr = (x+s0)**2+(x+s1)**2-s2
# 展开表达式
expanded_expr = expand(expr)
print(f"展开后的表达式: {expanded_expr}")

# 简化表达式
simplified_expr = simplify(expanded_expr)
print(f"简化后的表达式: {simplified_expr}")

# 求解方程
equation = expanded_expr
solutions = solve(equation, x)
print(f"方程 {equation} 的解: {solutions}")