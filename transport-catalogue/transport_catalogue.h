#pragma once

#include <string>
#include <string_view>
#include <deque>
#include <vector>
#include <set>
#include <unordered_map>

#include "geo.h"

namespace catalogue {
namespace detail {
struct Stop {
    std::string name;
    Coordinates coord;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
};

struct BusStats {
    size_t stops_num;
    size_t unique_stops;
    double length;
};
}// namespace detail

using namespace detail;

class TransportCatalogue {
    public:
    // добавление остановки в базу
    void AddStop(const Stop& stop);

    // добавление маршрута в базу
    void AddBus(const Bus& bus);

    // поиск остановки по имени
    const Stop* GetStop(std::string_view stop_name) const;

    // поиск маршрута по имени
    const Bus* GetBus(std::string_view bus_name) const;

    // получение информации о маршруте
    const BusStats& GetRouteInfo(const Bus* bus) const;

    // получение информации об остановке
    const std::set<std::string_view>& GetStopInfo(const Stop* stop) const;

    private:
    // расчитывает и возвращает статистику маршрута
    BusStats CalcBusStatistics(const Bus& bus) const;

    // возвращает количество уникальных остановок маршрута
    size_t GetUniqueStops(const Bus& bus) const;

    // возвращает дистанцию всего маршрута
    double GetRouteLength(const Bus& bus) const;

    std::deque<Stop> stops_;
    std::deque<Bus> buses_;

    std::unordered_map<std::string_view, const Stop*> find_stops_;
    std::unordered_map<std::string_view, const Bus*> find_buses_;

    std::unordered_map<const Bus*, BusStats> route_info_;
    std::unordered_map<const Stop*, std::set<std::string_view>> stops_info_;	
};
}// namespace catalogue