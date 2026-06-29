#pragma once
#include "common.h"
#include "station.h"
#include <string>
#include <vector>

class MetroGraph {
public:
    // Station id rule:
    // id = line code + station sequence, for example 111 is line 1 station 11.
    // Transfer stations are stored as separate Station records on each line
    // and connected by Edge records whose line is "换乘" and timeCost is 5.
    bool buildFromCsv(const std::string& fileName, const StationManager& stationManager);

    bool hasStation(int stationId) const;
    int vertexCount() const;
    const std::vector<Edge>& neighbors(int stationId) const;

private:
    std::vector<std::vector<Edge>> adj_;
    int countEdges() const;
};
