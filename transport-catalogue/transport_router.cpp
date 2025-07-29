#include "transport_router.h"

#include <iostream>

using namespace graph;

namespace router {

TransportRouter::TransportRouter(RoutingSettings routing_settings,
                                          const catalogue::TransportCatalogue& catalogue)
        : routing_settings_{routing_settings},
          graph_{BuildGraph(catalogue)},
          router_{graph_} {
}

// добавляет в переданный в качестве аргумента граф рёбра, которые отвечают за ожидание на остановках, и заполняет vertex_id_
void TransportRouter::AddWaitingEdges(DirectedWeightedGraph<TravelTime>& graph, const catalogue::TransportCatalogue& catalogue) {
    VertexId id = 0;
    for(const auto [name, ptr_stop] : catalogue.GetAllStops()) {
        vertexes_ids_[ptr_stop] = id;
        graph.AddEdge(Edge<TravelTime>{.route_id = name,
                                       .from = id,
                                       .to = ++id,
                                       .weight = static_cast<TravelTime>(routing_settings_.bus_waiting_time),
                                       .span_count = 0});
        ++id;
    }
}

// добавляет в переданный в качестве аргумента граф рёбра, которые отвечают за проезд на автобусе между остановками
void TransportRouter::AddTransitEdges(DirectedWeightedGraph<TravelTime>& graph, const catalogue::TransportCatalogue& catalogue) {
    const auto& buses = catalogue.GetAllRoutes();
    for(const auto& [bus_name, ptr_bus] : buses) {
        const auto& stops = ptr_bus->stops;
        size_t stops_count = stops.size();
        for(int i = 0; i < stops_count; ++i) {
            int dist = 0;
            int reverse_dist = 0;

            for(int j = i + 1; j < stops_count; ++j) {

                dist += catalogue.GetDistanceBetweenStops(stops[j - 1], stops[j]);
                reverse_dist += catalogue.GetDistanceBetweenStops(stops[j], stops[j-1]);

                TravelTime travel_time = dist / routing_settings_.bus_velocity;
                // ребра для прямого пути из вершины остановки i в j
                graph.AddEdge(Edge<TravelTime>{.route_id = bus_name,
                                               .from = vertexes_ids_[stops[i]] + 1,
                                               .to = vertexes_ids_[stops[j]],
                                               .weight = travel_time,
                                               .span_count = j - i});
                if(!ptr_bus->is_roundtrip) {
                    travel_time = reverse_dist / routing_settings_.bus_velocity;
                    // ребра для обратного пути из вершины остановки j в i
                    graph.AddEdge(Edge<TravelTime>{.route_id = bus_name,
                                                  .from = vertexes_ids_[stops[j]] + 1,
                                                  .to = vertexes_ids_[stops[i]],
                                                  .weight = travel_time,
                                                  .span_count = j - i});
                }
            }
        }
    }
}

graph::DirectedWeightedGraph<TravelTime> TransportRouter::BuildGraph(const catalogue::TransportCatalogue& catalogue) {
    // конструируем граф, на каждую остановку по две вершины: первая для ожидания, вторая - для начала пути
    DirectedWeightedGraph<TravelTime> graph(catalogue.GetAllStops().size() * 2);
    AddWaitingEdges(graph, catalogue);
    AddTransitEdges(graph, catalogue);
    return graph;
}

std::optional<Router<TravelTime>::RouteInfo> TransportRouter::BuildRoute(const domain::Stop* from, const domain::Stop* to) const {
    return router_.BuildRoute(vertexes_ids_.at(from), vertexes_ids_.at(to));
}

const DirectedWeightedGraph<TravelTime>& TransportRouter::GetGraph() const {
    return graph_;
}

}  // namespace router