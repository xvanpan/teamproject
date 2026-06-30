#include "station.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <utility>

namespace {
    bool parseUpdateStatusText(const std::string& text, StationStatus& status)
    {
        if (text == "开启") {
            status = StationStatus::Open;
            return true;
        }
        if (text == "关闭") {
            status = StationStatus::Closed;
            return true;
        }
        return false;
    }
}

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

void StationManager::rebuildIndex()
{
    idToIndex_.clear();
    for (std::size_t i = 0; i < stations_.size(); ++i) {
        idToIndex_[stations_[i].id] = (int)i;
    }
}

bool StationManager::loadFromCsv(const std::string& fileName)
{
    std::ifstream fin(fileName);
    if (!fin.is_open()) {
        std::cerr << "错误：无法打开文件 " << fileName << std::endl;
        return false;
    }

    stations_.clear();

    std::string line;
    std::getline(fin, line);

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
        station.id = std::stoi(trimCsvField(idText));
        station.name = trimCsvField(nameText);
        station.line = trimCsvField(lineText);
        station.status = parseStatusText(trimCsvField(statusText));

        stations_.push_back(station);
    }

    rebuildIndex();
    return true;
}

bool StationManager::saveToCsv(const std::string& fileName) const
{
    std::ofstream fout(fileName);
    if (!fout.is_open()) {
        std::cerr << "目标文件不存在。" << std::endl;
        return false;
    }

    fout << "id,name,line,status\n";
    for (const auto& station : stations_) {
        fout << station.id << ","
             << station.name << ","
             << station.line << ","
             << toStatusText(station.status) << std::endl;
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

bool StationManager::batchUpdateStatusFromCsv(const std::string& updateFile)
{
    std::ifstream fin(updateFile);
    if (!fin.is_open()) {
        std::cerr << "更新文件不存在。" << std::endl;
        return false;
    }

    std::string line;
    std::getline(fin, line);
    std::vector<std::pair<int, StationStatus>> updates;

    while (std::getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string idText;
        std::string statusText;
        std::string extraText;

        if (!std::getline(ss, idText, ',') || !std::getline(ss, statusText, ',') || std::getline(ss, extraText, ',')) {
            std::cerr << "格式错误，终止更新。" << std::endl;
            return false;
        }

        int id = 0;
        try {
            std::size_t pos = 0;
            std::string trimmedId = trimCsvField(idText);
            id = std::stoi(trimmedId, &pos);
            if (pos != trimmedId.size()) {
                std::cerr << "格式错误，终止更新。" << std::endl;
                return false;
            }
        }
        catch (...) {
            std::cerr << "格式错误，终止更新。" << std::endl;
            return false;
        }

        StationStatus status;
        if (!parseUpdateStatusText(trimCsvField(statusText), status)) {
            std::cerr << "非法状态值，跳过该行。（必须为“开启/关闭”）" << std::endl;
            continue;
        }

        if (!hasStation(id)) {
            std::cerr << "未匹配到对应站点，跳过该行。" << std::endl;
            continue;
        }

        updates.push_back({ id, status });
    }

    if (updates.empty()) {
        std::cerr << "未检测到有效更新记录。" << std::endl;
        return false;
    }

    for (const auto& update : updates) {
        setStationStatus(update.first, update.second);
    }

    return true;
}

const Station* StationManager::getStationById(int id) const
{
    auto it = idToIndex_.find(id);
    if (it == idToIndex_.end()) {
        return nullptr;
    }
    return &stations_[it->second];
}

Station* StationManager::getStationById(int id)
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
    } else {
        return setStationStatus(id, StationStatus::Closed);
    }
}

bool StationManager::isStationOpen(int id) const
{
    const Station* p = getStationById(id);
    if (p == nullptr)
        return false;

    return p->status == StationStatus::Open;
}

std::vector<Station> StationManager::getClosedStations() const
{
    std::vector<Station> result;
    for (const auto& a : stations_) {
        if (a.status == StationStatus::Closed)
            result.push_back(a);
    }
    return result;
}

std::vector<Station> StationManager::getStationsByLine(const std::string& line) const
{
    std::vector<Station> res;
    for (const auto& a : stations_) {
        if (a.line == line)
            res.push_back(a);
    }
    return res;
}

std::vector<int> StationManager::searchStationByName(const std::string& name) const
{
    std::vector<int> res;
    for (const auto& a : stations_) {
        if (a.name.find(name) != std::string::npos)
            res.push_back(a.id);
    }
    return res;
}

const std::vector<Station>& StationManager::getAllStations() const
{
    return stations_;
}

bool StationManager::hasStation(int id) const
{
    return idToIndex_.find(id) != idToIndex_.end();
}
