#include "pathfinder.h"
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <set>
#include <string>
#include <tuple>
#include <unordered_set>

using namespace std;

namespace {
    bool isTransferEdge(const Edge& edge) {
        return edge.type == EdgeType::Transfer || edge.line == "换乘";
    }

    bool isSameStationName(const StationManager& stationManager, int leftId, int rightId) {
        const Station* left = stationManager.getStationById(leftId);
        const Station* right = stationManager.getStationById(rightId);
        return left != nullptr && right != nullptr && left->name == right->name;
    }

    bool isVirtualEndpointTransfer(const Edge& edge, int startStation, int endStation, const StationManager& stationManager) {
        bool transferAtStart = edge.from == startStation || isSameStationName(stationManager, edge.from, startStation);
        bool transferAtEnd = edge.to == endStation || isSameStationName(stationManager, edge.to, endStation);
        return isTransferEdge(edge) && (transferAtStart || transferAtEnd);
    }

    bool countsAsTransfer(const Edge& edge, int startStation, int endStation, const StationManager& stationManager) {
        return isTransferEdge(edge) && !isVirtualEndpointTransfer(edge, startStation, endStation, stationManager);
    }

    int effectiveTimeCost(const Edge& edge, int startStation, int endStation, const StationManager& stationManager) {
        if (isVirtualEndpointTransfer(edge, startStation, endStation, stationManager)) {
            return 0;
        }
        return edge.timeCost;
    }

    string makePathKey(const vector<int>& path) {
        string key;
        for (int id : path) {
            key += to_string(id);
            key += '#';
        }
        return key;
    }

    bool containsStation(const vector<int>& path, int stationId) {
        return find(path.begin(), path.end(), stationId) != path.end();
    }
}

PathFinder::PathFinder(const MetroGraph& graph, const StationManager& stationManager)
    : graph_(graph), stationManager_(stationManager) {
}

bool PathFinder::isStationAvailable(int stationId) const {
    const Station* station = stationManager_.getStationById(stationId);
    return graph_.hasStation(stationId) && station != nullptr && station->status == StationStatus::Open;
}

int PathFinder::countTransfers(const vector<int>& stationIds, int startId, int endId) const {
    int transfers = 0;//统计换成次数
    if (stationIds.empty()) return transfers;

    for (int i = 0; i + 1 < (int)stationIds.size(); ++i) {
        int from = stationIds[i];
        int to = stationIds[i + 1];

        for (const Edge& edge : graph_.neighbors(from)) {
            if (edge.to == to) {
                if (countsAsTransfer(edge, startId, endId, stationManager_)) {
                    transfers++;
                }
                break;
            }
        }
    }

    return transfers;
}

PathResult PathFinder::buildPathResult(const vector<int>& prev, int startId, int endId) const {
    PathResult result;
    result.valid = false;

    if (!isStationAvailable(startId) || !isStationAvailable(endId)) {
        return result;
    }

    vector<int> stationIds;
    if (startId == endId) {
        stationIds.push_back(startId);
    }
    else {
        if (endId < 0 || endId >= (int)prev.size() || prev[endId] == -1) {
            return result;
        }

        for (int at = endId; at != -1; at = prev[at]) {
            stationIds.push_back(at);
            if (at == startId) break;
        }

        if (stationIds.empty() || stationIds.back() != startId) {
            return result;
        }
        reverse(stationIds.begin(), stationIds.end());
    }

    result.stationIds = stationIds;
    result.stationNames.reserve(stationIds.size());
    result.lines.reserve(stationIds.size());

    int totalTime = 0;
    set<string> transferStationSet;

    for (int id : stationIds) {
        const Station* station = stationManager_.getStationById(id);
        if (station == nullptr) {
            result.valid = false;
            return result;
        }
        if (result.stationNames.empty() || result.stationNames.back() != station->name) {
            result.stationNames.push_back(station->name);
        }
    }

    for (int i = 0; i + 1 < (int)stationIds.size(); ++i) {
        int from = stationIds[i];
        int to = stationIds[i + 1];
        bool found = false;

        for (const Edge& edge : graph_.neighbors(from)) {
            if (edge.to == to) {
                totalTime += effectiveTimeCost(edge, startId, endId, stationManager_);
                if (!isTransferEdge(edge) && (result.lines.empty() || result.lines.back() != edge.line)) {
                    result.lines.push_back(edge.line);
                }
                if (countsAsTransfer(edge, startId, endId, stationManager_)) {
                    const Station* transferStation = stationManager_.getStationById(from);
                    if (transferStation != nullptr && transferStationSet.insert(transferStation->name).second) {
                        result.transferStations.push_back(transferStation->name);
                    }
                }
                found = true;
                break;
            }
        }

        if (!found) {
            return result;
        }
    }

    if (result.lines.empty() && !stationIds.empty()) {
        const Station* station = stationManager_.getStationById(stationIds.front());
        if (station != nullptr) {
            result.lines.push_back(station->line);
        }
    }

    result.totalTime = totalTime;
    result.transferCount = (int)result.transferStations.size();
    result.passedStationCount = (int)result.stationNames.size();
    result.valid = true;
    return result;
}

PathResult PathFinder::shortestTimePath(int startId, int endId) const {
    PathResult empty;//单条最短时间
    empty.valid = false;

    if (!isStationAvailable(startId) || !isStationAvailable(endId)) {
        return empty;
    }

    int n = graph_.vertexCount();
    vector<int> dist(n, INF);
    vector<int> transferDist(n, INF);
    vector<int> prev(n, -1);
    using State = tuple<int, int, int>; // time, transfers, station
    priority_queue<State, vector<State>, greater<State>> pq;

    dist[startId] = 0;
    transferDist[startId] = 0;
    pq.push(State(0, 0, startId));

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        int curDist = get<0>(current);
        int curTransfers = get<1>(current);
        int u = get<2>(current);

        if (curDist != dist[u] || curTransfers != transferDist[u]) continue;
        if (u == endId) break;

        for (const Edge& edge : graph_.neighbors(u)) {
            int v = edge.to;
            if (!isStationAvailable(v)) continue;

            int nextDist = curDist + effectiveTimeCost(edge, startId, endId, stationManager_);
            int nextTransfers = curTransfers + (countsAsTransfer(edge, startId, endId, stationManager_) ? 1 : 0);
            if (nextDist < dist[v] || (nextDist == dist[v] && nextTransfers < transferDist[v])) {
                dist[v] = nextDist;
                transferDist[v] = nextTransfers;
                prev[v] = u;
                pq.push(State(nextDist, nextTransfers, v));
            }
        }
    }

    if (dist[endId] == INF) {
        return empty;
    }
    return buildPathResult(prev, startId, endId);
}

PathResult PathFinder::minTransferPath(int startId, int endId) const {//单条最少换乘
    PathResult empty;
    empty.valid = false;

    if (!isStationAvailable(startId) || !isStationAvailable(endId)) {
        return empty;
    }

    int n = graph_.vertexCount();
    vector<int> transferDist(n, INF);
    vector<int> timeDist(n, INF);
    vector<int> prev(n, -1);

    using State = tuple<int, int, int>; // transfers, time, station
    priority_queue<State, vector<State>, greater<State>> pq;

    transferDist[startId] = 0;
    timeDist[startId] = 0;
    pq.push({0, 0, startId});

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        int curTransfers = get<0>(current);
        int curTime = get<1>(current);
        int u = get<2>(current);

        if (curTransfers != transferDist[u] || curTime != timeDist[u]) continue;
        if (u == endId) break;

        for (const Edge& edge : graph_.neighbors(u)) {
            int v = edge.to;
            if (!isStationAvailable(v)) continue;

            int nextTransfers = curTransfers + (countsAsTransfer(edge, startId, endId, stationManager_) ? 1 : 0);
            int nextTime = curTime + effectiveTimeCost(edge, startId, endId, stationManager_);

            if (nextTransfers < transferDist[v] ||
                (nextTransfers == transferDist[v] && nextTime < timeDist[v])) {
                transferDist[v] = nextTransfers;
                timeDist[v] = nextTime;
                prev[v] = u;
                pq.push({nextTransfers, nextTime, v});
            }
        }
    }

    if (transferDist[endId] == INF) {
        return empty;
    }
    return buildPathResult(prev, startId, endId);
}

vector<PathResult> PathFinder::kShortestTimePaths(int startId, int endId, int k) const {//k条最短时间
    vector<PathResult> results;
    if (k <= 0 || !isStationAvailable(startId) || !isStationAvailable(endId)) {
        return results;
    }

    struct State {
        int totalTime;
        int transfers;
        vector<int> path;
        bool operator>(const State& other) const {
            if (totalTime != other.totalTime) {
                return totalTime > other.totalTime;
            }
            return transfers > other.transfers;
        }
    };

    priority_queue<State, vector<State>, greater<State>> pq;
    vector<int> expandedCount(graph_.vertexCount(), 0);
    set<string> accepted;
    const int maxExpansions = 200000;
    int expansions = 0;

    pq.push({0, 0, {startId}});

    while (!pq.empty() && (int)results.size() < k && expansions < maxExpansions) {
        State cur = pq.top();
        pq.pop();
        expansions++;

        int u = cur.path.back();
        if (u < 0 || u >= graph_.vertexCount()) {
            continue;
        }
        if (expandedCount[u] >= k) {
            continue;
        }
        expandedCount[u]++;

        if (u == endId) {
            string key = makePathKey(cur.path);
            if (accepted.insert(key).second) {
                vector<int> prev(graph_.vertexCount(), -1);
                for (int i = 0; i + 1 < (int)cur.path.size(); ++i) {
                    prev[cur.path[i + 1]] = cur.path[i];
                }
                PathResult result = buildPathResult(prev, startId, endId);
                if (result.valid) {
                    results.push_back(result);
                }
            }
            continue;
        }

        for (const Edge& edge : graph_.neighbors(u)) {
            int v = edge.to;
            if (!isStationAvailable(v) || containsStation(cur.path, v)) continue;
            if (v >= 0 && v < graph_.vertexCount() && expandedCount[v] >= k) continue;

            State next = cur;
            next.totalTime += effectiveTimeCost(edge, startId, endId, stationManager_);
            next.transfers += countsAsTransfer(edge, startId, endId, stationManager_) ? 1 : 0;
            next.path.push_back(v);
            pq.push(next);
        }
    }

    if (results.empty()) {
        PathResult result = shortestTimePath(startId, endId);
        if (result.valid) {
            results.push_back(result);
        }
    }

    return results;
}

vector<PathResult> PathFinder::kMinTransferPaths(int startId, int endId, int k) const {//k条最短换乘
    vector<PathResult> results;
    if (k <= 0 || !isStationAvailable(startId) || !isStationAvailable(endId)) {
        return results;
    }

    struct State {
        int transfers;
        int totalTime;
        vector<int> path;
        bool operator>(const State& other) const {
            if (transfers != other.transfers) return transfers > other.transfers;
            return totalTime > other.totalTime;
        }
    };

    priority_queue<State, vector<State>, greater<State>> pq;
    vector<int> expandedCount(graph_.vertexCount(), 0);
    set<string> accepted;
    const int maxExpansions = 200000;
    int expansions = 0;

    pq.push({0, 0, {startId}});

    while (!pq.empty() && (int)results.size() < k && expansions < maxExpansions) {
        State cur = pq.top();
        pq.pop();
        expansions++;

        int u = cur.path.back();
        if (u < 0 || u >= graph_.vertexCount()) {
            continue;
        }
        if (expandedCount[u] >= k) {
            continue;
        }
        expandedCount[u]++;

        if (u == endId) {
            string key = makePathKey(cur.path);
            if (accepted.insert(key).second) {
                vector<int> prev(graph_.vertexCount(), -1);
                for (int i = 0; i + 1 < (int)cur.path.size(); ++i) {
                    prev[cur.path[i + 1]] = cur.path[i];
                }
                PathResult result = buildPathResult(prev, startId, endId);
                if (result.valid) {
                    results.push_back(result);
                }
            }
            continue;
        }

        for (const Edge& edge : graph_.neighbors(u)) {
            int v = edge.to;
            if (!isStationAvailable(v) || containsStation(cur.path, v)) continue;
            if (v >= 0 && v < graph_.vertexCount() && expandedCount[v] >= k) continue;

            State next = cur;
            next.transfers += countsAsTransfer(edge, startId, endId, stationManager_) ? 1 : 0;
            next.totalTime += effectiveTimeCost(edge, startId, endId, stationManager_);
            next.path.push_back(v);
            pq.push(next);
        }
    }

    if (results.empty()) {
        PathResult result = minTransferPath(startId, endId);
        if (result.valid) {
            results.push_back(result);
        }
    }

    return results;
}

vector<int> PathFinder::analyzeAffectedArea(const vector<int>& closedStationIds) const {//影响区域
    vector<int> affected;
    unordered_set<int> closedSet(closedStationIds.begin(), closedStationIds.end());
    unordered_set<int> affectedSet;

    for (int closedId : closedStationIds) {
        for (const Edge& edge : graph_.neighbors(closedId)) {
            int neighbor = edge.to;
            if (closedSet.count(neighbor) == 0 && affectedSet.insert(neighbor).second) {
                affected.push_back(neighbor);
            }
        }
    }

    sort(affected.begin(), affected.end());
    return affected;
}
