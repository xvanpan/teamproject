#pragma once
#include <string>
#include <vector>

const int INF = 1000000000;
const int TRANSFER_TIME = 5;

enum class StationStatus {
    Open,
    Closed
};

enum class EdgeType {
    Normal,
    Transfer
};

struct Station {
    int id;
    std::string name;
    std::string line;
    StationStatus status = StationStatus::Open;
};

struct Edge {
    int from;
    int to;
    std::string line;
    std::string direction;
    int timeCost = 0;
    EdgeType type = EdgeType::Normal;
};

struct PathResult {
    std::vector<int> stationIds;
    std::vector<std::string> stationNames;
    std::vector<std::string> lines;
    std::vector<std::string> transferStations;
    int totalTime = 0;
    int transferCount = 0;
    int passedStationCount = 0;
    bool valid = false;
};