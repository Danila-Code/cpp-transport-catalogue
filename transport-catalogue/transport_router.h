#pragma once

#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

namespace router {

using TravelTime = double;

struct RoutingSettings {
    double bus_velocity;
    int bus_waiting_time;
};

class TransportRouter {
public:
    explicit TransportRouter(RoutingSettings routing_settings, const catalogue::TransportCatalogue& catalogue);
    // построить маршрут
    std::optional<graph::Router<TravelTime>::RouteInfo> BuildRoute(const domain::Stop* from, const domain::Stop* to) const;

    const graph::DirectedWeightedGraph<TravelTime>& GetGraph() const;

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