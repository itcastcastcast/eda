from sympy import symbols, expand, simplify, solve

# 定义符号变量
p,s1,s2,s3,s4,n = symbols('p s1 s2 s3 s4 n')

# 符号表达式
expr = n*p**2*s2+(1-n*p)**2*s2+s1+n*s4-s3
# 展开表达式
expanded_expr = expand(expr)
print(f"展开后的表达式: {expanded_expr}")

# 简化表达式
simplified_expr = simplify(expanded_expr)
print(f"简化后的表达式: {simplified_expr}")

# 求解方程
equation = expanded_expr
solutions = solve(equation, p)
print(f"方程 {equation} 的解: {solutions}")