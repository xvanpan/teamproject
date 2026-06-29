#pragma once
#include "station.h"
#include "graph.h"
#include "pathfinder.h"

class MenuSystem {
public:
    MenuSystem(StationManager& stationManager, MetroGraph& graph, PathFinder& pathFinder);
    void run();

private:
    void showMainMenu() const;
    void handlePathMenu();
    void handleStationMenu();
    void handleLineMenu();

    void queryShortestTimePath();
    void queryKShortestTimePaths();
    void queryMinTransferPath();
    void queryKMinTransferPaths();

    void batchUpdateStationStatus();
    void manualUpdateStationStatus();
    void resetStationStatus();
    void showClosedStations();
    void showLineInfo();

    void printPathResult(const PathResult& result) const;

private:
    StationManager& stationManager_;
    MetroGraph& graph_;
    PathFinder& pathFinder_;
};