#pragma once

#include <string_view>
#include <deque>
#include <set>
#include <unordered_map>
#include <map>

#include "domain.h"

namespace catalogue {

using namespace domain;

class TransportCatalogue {
public:
    // добавление остановки в базу
    void AddStop(const Stop& stop);

    void AddDistanceBetweenStops(std::string_view from, std::string_view to, size_t distance);

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
    
    // получение всех маршрутов
    const std::unordered_map<std::string_view, const Bus*>& GetAllRoutes() const;
    
    // получение всех остановок
    const std::unordered_map<std::string_view, const Stop*> GetAllStops() const;

    // получение расстояния между двумя остановками
    size_t GetDistanceBetweenStops(const Stop* from, const Stop* to) const;

private:
    // расчитывает и возвращает статистику маршрута
    BusStats CalcBusStatistics(const Bus& bus) const;

    // возвращает количество уникальных остановок маршрута
    size_t GetUniqueStops(const Bus& bus) const;

    // возвращает географическую дину всего маршрута
    double GetRouteLengthG(const Bus& bus) const;

    // возвращает фактическую дину всего маршрута
    size_t GetRouteLengthF(const Bus& bus) const;

    // хранение информации об остановках и маршрутах соответственно
    std::deque<Stop> stops_;
    std::deque<Bus> buses_;

    // индексы для поиска остановок и автобусов по их названиям соответственно
    std::unordered_map<std::string_view, const Stop*> find_stops_;
    std::unordered_map<std::string_view, const Bus*> find_buses_;

    // информация о маршрутах
    std::unordered_map<const Bus*, BusStats> route_info_;
    // информация об остановках (какие автобусы проходят через остановку)
    std::unordered_map<const Stop*, std::set<std::string_view>> stops_info_;
    // фактические расстояния между парами остановок
    std::unordered_map<std::pair<const Stop*, const Stop*>, size_t, StopHasher> stops_distances_;	
};
}// namespace catalogue