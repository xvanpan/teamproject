#pragma once
#include "common.h"
#include "graph.h"
#include "station.h"
#include <vector>

class PathFinder {
public:
    PathFinder(const MetroGraph& graph, const StationManager& stationManager);

    PathResult shortestTimePath(int startId, int endId) const;

    PathResult minTransferPath(int startId, int endId) const;

    std::vector<PathResult> kShortestTimePaths(int startId, int endId, int k = 3) const;
    std::vector<PathResult> kMinTransferPaths(int startId, int endId, int k = 3) const;

    std::vector<int> analyzeAffectedArea(const std::vector<int>& closedStationIds) const;

private:
    PathResult buildPathResult(const std::vector<int>& prev, int startId, int endId) const;
    int countTransfers(const std::vector<int>& stationIds, int startId, int endId) const;
    bool isStationAvailable(int stationId) const;

private:
    const MetroGraph& graph_;
    const StationManager& stationManager_;
};
