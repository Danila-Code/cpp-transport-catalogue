#include <numeric>
#include <algorithm>
#include <unordered_set>
#include <cassert>
#include "transport_catalogue.h"

namespace catalogue {

// добавление остановки в базу
void TransportCatalogue::AddStop(const Stop& stop) {
    auto new_stop = stops_.insert(stops_.end(), std::move(stop));
    find_stops_[new_stop->name] = &(*new_stop);
    stops_info_.insert({&(*new_stop), {}});
}

// Добавление расстояния между остановками
void TransportCatalogue::AddDistanceBetweenStops(std::string_view from, std::string_view to, size_t distance) {
    stops_distances_[{find_stops_[from], find_stops_[to]}] = distance;
}

// добавление маршрута в базу
void TransportCatalogue::AddBus(const Bus& bus) {
    auto new_bus = buses_.insert(buses_.end(), std::move(bus));
    find_buses_[new_bus->name] = &(*new_bus);

    // добавляем в route_info_ статистику маршрута автобуса bus
    route_info_[&(*new_bus)] = std::move(CalcBusStatistics(*new_bus));
    // добавляем в stops_info_ для каждой остановки маршрута bus номер автобуса
    std::for_each(bus.stops.begin(), bus.stops.end(), [this, new_bus](const Stop* stop) {
        stops_info_[stop].insert(new_bus->name);
    });
}

// поиск остановки по имени
const Stop* TransportCatalogue::GetStop(std::string_view stop_name) const {
    auto iter = find_stops_.find(stop_name);
    if(iter == find_stops_.end()) {
        return nullptr;
    }
    return iter->second;
}

// поиск маршрута по имени
const Bus* TransportCatalogue::GetBus(std::string_view bus_name) const {
    auto iter = find_buses_.find(bus_name);
    if(iter == find_buses_.end()) {
        return nullptr;
    }
    return iter->second;
}

// возвращает количество уникальных остановок маршрута
size_t TransportCatalogue::GetUniqueStops(const Bus& bus) const {
    std::unordered_set<const Stop*> unique_stops(bus.stops.begin(), bus.stops.end());
    return unique_stops.size();
}

// возвращает географическую длину всего маршрута 
double TransportCatalogue::GetRouteLengthG(const Bus& bus) const {
    auto calc_sum = [](double lhs, double rhs) {
        return lhs + rhs;
    };
    auto calc_distance = [](const Stop* lhs, const Stop* rhs) {
        return ComputeDistance(lhs->coord, rhs->coord);
    };
    return std::transform_reduce(bus.stops.begin(), bus.stops.end() - 1, bus.stops.begin() + 1, 0.0, 
                     calc_sum, calc_distance);
}

// возвращает фактическую дину всего маршрута
size_t TransportCatalogue::GetRouteLengthF(const Bus& bus) const {
    size_t distance = 0;
    for(size_t i = 1; i < bus.stops.size(); ++i) {
        if(auto iter = stops_distances_.find(std::pair{bus.stops[i - 1], bus.stops[i]}); iter != stops_distances_.end()) {
            distance += iter->second;
        } else if(auto iter = stops_distances_.find(std::pair{bus.stops[i], bus.stops[i - 1]}); iter != stops_distances_.end()) {
            distance += iter->second;
        }
    }
    return distance;
}

// расчитывает и возвращает статистику маршрута
BusStats TransportCatalogue::CalcBusStatistics(const Bus& bus) const {
    size_t length_f = GetRouteLengthF(bus);
    double curvature = length_f / GetRouteLengthG(bus);
    return BusStats{bus.stops.size(), GetUniqueStops(bus), length_f, curvature};
}

// получение информации о маршруте Bus X: R stops on route, U unique stops, L route length
const BusStats& TransportCatalogue::GetRouteInfo(const Bus* bus) const {
    assert(bus);
    return route_info_.at(bus);
}

// получение информации об остановке
const std::set<std::string_view>& TransportCatalogue::GetStopInfo(const Stop* stop) const {
    assert(stop);
    return stops_info_.at(stop);
}

// получение всех маршрутов
const std::unordered_map<std::string_view, const Bus*>& TransportCatalogue::GetAllRoutes() const {
    return find_buses_;
}

// получение всех остановок
const std::unordered_map<std::string_view, const Stop*> TransportCatalogue::GetAllStops() const {
    return find_stops_;
}

// получение расстояния между двумя остановками
size_t TransportCatalogue::GetDistanceBetweenStops(const Stop* from, const Stop* to) const {
    auto iter = stops_distances_.find({from, to});
    if(iter != stops_distances_.end()) {
        return iter->second;
    }
    return stops_distances_.at(std::pair(to, from));
}

}// namespace catalogue