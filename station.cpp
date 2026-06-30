#include"station.h"
#include<fstream>
#include<sstream>


StationStatus StationManager::parseStatusText(const std::string& text) const
{
    if (text == "开启")
        return StationStatus::Open;
    else
        return StationStatus::Closed;
}
std::string StationManager::toStatusText(StationStatus status) const
{
    if (status == StationStatus::Open)
        return "开启";
    else
        return "关闭";
}
void StationManager::rebuildIndex() {
    idToIndex_.clear();
    for (int i = 0; i < stations_.size(); ++i) {
        idToIndex_[stations_[i].id] = i;
    }
}
bool StationManager::loadFromCsv(const std::string& fileName) {
    std::ifstream fin(fileName);
    if (!fin.is_open()) {
        return false;
    }

    stations_.clear();

    std::string line;
    std::getline(fin, line); // 跳过表头

    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string idText;
        std::string nameText;
        std::string lineText;
        std::string statusText;

        std::getline(ss, idText, ',');
        std::getline(ss, nameText, ',');
        std::getline(ss, lineText, ',');
        std::getline(ss, statusText, ',');

        Station station;
        station.id = std::stoi(idText);
        station.name = nameText;
        station.line = lineText;
        station.status = parseStatusText(statusText);

        stations_.push_back(station);
    }

    rebuildIndex();
    return true;
}
Station* StationManager::getStationById(int id) {
    auto it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        return nullptr;
    }
    return &stations_[it->second];
}
const Station* StationManager::getStationById(int id) const
{
    auto it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        return nullptr;
    }
    return &stations_[it->second];
}
bool StationManager::setStationStatus(int id, StationStatus status)
{
    Station* p = getStationById(id);
    if (p == nullptr)
        return false;
    p->status = status;
    return true;

}
bool StationManager::setStationOpen(int id, bool isOpen)
{
    if (isOpen) {
        return setStationStatus(id, StationStatus::Open);
    }
    else
        return setStationStatus(id, StationStatus::Open);
}
bool StationManager::isStationOpen(int id) const
{
    const Station* p = getStationById(id);
    if (p == nullptr)
        return false;
    if (p->status == StationStatus::Open)
        return true;
    else
        return false;
}
const std::vector<Station>& StationManager::getAllStations() const
{
    return stations_;
}
std::vector<Station> StationManager::getClosedStations() const
{
    std::vector<Station> result;
    for (& const auto a : stations_) {
        if (a.status == StationStatus::Closed)
            result.push_back(a);
    }
    return result;
}
std::vector<Station> StationManager::getStationsByLine(const std::string& line) const
{
    std::vector<Station> res;&
    for (& const auto a : stations_) {
        if (a.line == line)
            res.push_back(a);
    }
    return res;
}

std::vector<int> StationManager::searchStationByName(const std::string& name) const
{
    std::vector<int> res;
    for (& const auto a : stations_) {
        if (a.name == name)
            res.push_back(a.id);
    }
    return res;
}
bool StationManager::saveToCsv(const std::string& fileName) const
{
    std::ofstream fout(fileName);
    if (!fout.is_open()) {
        return false;
    }

    fout << "id,name,line,status\n";
    for (const auto& station : stations_) {
        fout << station.id << ","
            << station.name << ","
            << station.line << ","
            << toStatusText(station.status) << endl;
    }

    return true;
}
bool StationManager::batchUpdateStatusFromCsv(const std::string& updateFile)
{
    std::ifstream fin(updateFile);
    if (!fin.is_open()) {
        return false;
    }

    std::string line;
    std::getline(fin, line); // 跳过表头

    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string idText;
        std::string statusText;

        std::getline(ss, idText, ',');
        std::getline(ss, statusText, ',');

        int id = std::stoi(idText);
        StationStatus status = parseStatusText(statusText);
        setStationStatus(id, status);
    }

    return true;
}
bool StationManager::resetFromInitCsv(const std::string& initFile, const std::string& targetFile)
{
    if (!loadFromCsv(initFile)) {
        return false;
    }

    return saveToCsv(targetFile);
}
bool StationManager::hasStation(int id) const
{
    return idToIndex_.find(id) != idToIndex_.end();
}