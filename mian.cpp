#include "station.h"
#include "graph.h"
#include "pathfinder.h"
#include "menu.h"

int main() {
    StationManager stationManager;
    stationManager.loadFromCsv("data/Station.csv");

    MetroGraph graph;
    graph.buildFromCsv("data/Edge.csv", stationManager);

    PathFinder pathFinder(graph, stationManager);

    MenuSystem menu(stationManager, graph, pathFinder);
    menu.run();

    return 0;
}