#pragma once

#include <variant>

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace router {

using TravelTime = double;
using namespace std::literals;

struct WaitingPart {
    WaitingPart(std::string_view name, TravelTime time) : stop_name{name}, travel_time{time} {
    }
    const std::string type = "Wait"s;
    std::string_view stop_name;
    TravelTime travel_time;
};

struct TransitPart {
    TransitPart(std::string_view name, TravelTime time, int count) : bus_name{name}, travel_time{time}, span_count{count} {
    }
    const std::string type = "Bus"s;
    std::string_view bus_name;
    TravelTime travel_time;
    int span_count = 0;
};

using RoutePart = std::variant<WaitingPart, TransitPart>;

struct ResultRoute {
    TravelTime total_time;
    std::vector<RoutePart> route_parts;
};

struct RoutingSettings {
    double bus_velocity;
    int bus_waiting_time;
};

class TransportRouter {
public:
    explicit TransportRouter(RoutingSettings routing_settings, const catalogue::TransportCatalogue& catalogue);
    // построить маршрут
    std::optional<ResultRoute> BuildRoute(const domain::Stop* from, const domain::Stop* to) const;

private:
    graph::DirectedWeightedGraph<TravelTime> BuildGraph(const catalogue::TransportCatalogue& catalogue);
    // добавляет в переданный в качестве аргумента граф рёбра, которые отвечают за ожидание на остановках, и заполняет vertex_id_
    void AddWaitingEdges(graph::DirectedWeightedGraph<TravelTime>& graph, const catalogue::TransportCatalogue& catalogue);

    // добавляет в переданный в качестве аргумента граф рёбра, которые отвечают за проезд на автобусе между остановками
    void AddTransitEdges(graph::DirectedWeightedGraph<TravelTime>& graph, const catalogue::TransportCatalogue& catalogue);
    
    RoutingSettings routing_settings_;
    // индекс для поиска идентификаторов вершин по указателям на остановки
    std::unordered_map<const domain::Stop*, graph::VertexId> vertexes_ids_;

    graph::DirectedWeightedGraph<TravelTime> graph_;
    graph::Router<TravelTime> router_;
};
}  // namespace router