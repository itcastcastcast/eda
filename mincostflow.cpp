#include<bits/stdc++.h>
using namespace std;
typedef pair<int, int>P;//first保存最短距离，second保存顶点的编号
const int maxn = 1200;
const int INF = 0x3f3f3f3f;
struct Edge
{
    int to, cap, cost, rev;//终点，容量（指残量网络中的），费用，反向边编号
    Edge(int t, int c, int cc, int r) :to(t), cap(c), cost(cc), rev(r) {}
};
class MCMF
{
public:
    int V;//顶点数
    vector<Edge>G[maxn];//图的邻接表
    int h[maxn];//上一次顶点的最短路s到v的最短路径
    int dist[maxn];//最短距离
    int prevv[maxn];//最短路中的父结点
    int preve[maxn];//最短路中的父边
    void init(int n)
    {
        for (int i = 0; i < n; ++i)    G[i].clear();
        V = n;
    }
    void addEdge(int from, int to, int cap, int cost)
    {
        G[from].push_back(Edge(to, cap, cost, G[to].size()));
        G[to].push_back(Edge(from, 0, -cost, G[from].size() - 1));
    }
    int min_cost_flow(int s, int t, int f)//返满足流f的最小费用 不能满足返回-1
    {
        int res = 0;
        fill(h, h + V, 0);
        while (f > 0)//f>0时还需要继续增广
        {
            priority_queue<P, vector<P>, greater<P> >q;
            fill(dist, dist + V, INF);//距离初始化为INF
            dist[s] = 0;
            q.push(P(0, s));
            while (!q.empty())
            {
                P p = q.top();
                q.pop();
                int v = p.second;
                if (dist[v] < p.first)    continue;//p.first是v入队列时候的值，dist[v]是目前的值，如果目前的更优，扔掉旧值
                for (int i = 0; i < G[v].size(); i++)
                {
                    Edge& e = G[v][i];
                    if (e.cap > 0 && dist[e.to] > dist[v] + e.cost + h[v] - h[e.to])//松弛操作
                    {
                        dist[e.to] = dist[v] + e.cost + h[v] - h[e.to];
                        prevv[e.to] = v;//更新父结点
                        preve[e.to] = i;//更新父边编号
                        q.push(P(dist[e.to], e.to));
                    }
                }
            }
            if (dist[t] == INF)//如果dist[t]还是初始时候的INF，那么说明s-t不连通，不能再增广了
                return -1;
            for (int j = 0; j < V; j++)//更新h
                h[j] = dist[j];
            int d = f;
            int sum = 0;
            for (int x = t; x != s; x = prevv[x]) {
                d = min(d, G[prevv[x]][preve[x]].cap);//从t出发沿着最短路返回s找可改进量
                sum += G[prevv[x]][preve[x]].cost;
            }
            f -= d;
            res += d * sum;//h[t]表示最短距离的同时，也代表了这条最短路上的费用之和，乘以流量d即可得到本次增广所需的费用
            for (int x = t; x != s; x = prevv[x])
            {
                Edge& e = G[prevv[x]][preve[x]];
                e.cap -= d;//修改残量值
                G[x][e.rev].cap += d;
            }
        }
        return res;
    }
};
/*
int main(){
    // Define number of nodes
    int numA = 1000;
    int numB = 20;
    int S = 0;
    int T = numA + numB +1;
    int totalNodes = T +1; // Nodes are 0 to T

    // Initialize MCMF
    MCMF mcmf;
    mcmf.init(totalNodes);

    // Generate random positions for A and B
    vector<pair<double, double>> A(numA);
    vector<pair<double, double>> B(numB);
    srand(time(0));
    for(int i = 0; i < numA; ++i){
        A[i].first = rand() % 10000;
        A[i].second = rand() % 10000;
    }
    for(int j = 0; j < numB; ++j){
        B[j].first = rand() % 10000;
        B[j].second = rand() % 10000;
    }

    // Connect S to all A
    for(int i = 0; i < numA; ++i){
        mcmf.addEdge(S, i +1, 1, 0);
    }

    // Connect each A to all B with cost as Euclidean distance
    for(int i = 0; i < numA; ++i){
        for(int j = 0; j < numB; ++j){
            double dx = A[i].first - B[j].first;
            double dy = A[i].second - B[j].second;
            double distance = sqrt(dx*dx + dy*dy);
            mcmf.addEdge(i +1, numA +1 +j, 1, static_cast<int>(distance));
        }
    }

    // Connect all B to T
    for(int j = 0; j < numB; ++j){
        mcmf.addEdge(numA +1 +j, T, 65, 0);
    }

    // Compute minimum cost flow
    int flow = numA;
    int cost = mcmf.min_cost_flow(S, T, flow);
    if(cost != -1){
        cout << "Minimum cost flow: " << cost << endl;
    }
    else{
        cout << "Cannot satisfy the flow requirement." << endl;
    }

    return 0;
}*/