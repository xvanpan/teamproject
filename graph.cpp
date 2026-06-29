#include "graph.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

bool MetroGraph::buildFromCsv(const string& fileName, const StationManager& stationManager) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "错误：无法打开文件 " << fileName << endl;
        return false;
    }

    adj_.clear();
    for (const Station& station : stationManager.getAllStations()) {
        if (station.id >= 0 && (int)adj_.size() <= station.id) {
            adj_.resize(station.id + 1);
        }
    }

    string line;
    getline(file, line);

    int edgeCount = 0;
    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;

        if (!getline(ss, token, ',')) continue;
        int from = stoi(token);

        if (!getline(ss, token, ',')) continue;
        int to = stoi(token);

        string lineName;
        if (!getline(ss, lineName, ',')) continue;

        string direction;
        if (!getline(ss, direction, ',')) continue;

        if (!getline(ss, token, ',')) continue;
        int time = stoi(token);

        if (!stationManager.hasStation(from) || !stationManager.hasStation(to)) {
            cerr << "跳过无效边 " << from << " -> " << to << endl;
            continue;
        }

        int maxId = max(from, to);
        if ((int)adj_.size() <= maxId) {
            adj_.resize(maxId + 1);
        }

        Edge edge;
        edge.from = from;
        edge.to = to;
        edge.line = lineName;
        edge.direction = direction;
        edge.timeCost = time;
        edge.type = (lineName == "换乘") ? EdgeType::Transfer : EdgeType::Normal;

        adj_[from].push_back(edge);
        edgeCount++;
    }

    cout << "成功加载 " << edgeCount << " 条边" << endl;
    cout << "顶点数：" << adj_.size() << endl;
    return true;
}

bool MetroGraph::hasStation(int stationId) const {
    return stationId >= 0 && stationId < (int)adj_.size();
}

int MetroGraph::vertexCount() const {
    return (int)adj_.size();
}

const vector<Edge>& MetroGraph::neighbors(int stationId) const {
    if (stationId < 0 || stationId >= (int)adj_.size()) {
        static const vector<Edge> empty;
        return empty;
    }
    return adj_[stationId];
}

int MetroGraph::countEdges() const {
    int total = 0;
    for (const auto& edges : adj_) {
        total += (int)edges.size();
    }
    return total;
}
