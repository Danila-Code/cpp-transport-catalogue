#pragma once

#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <vector>
#include <unordered_set>

using namespace catalogue;

class RequestHandler {
public:
    RequestHandler(const catalogue::TransportCatalogue& db, 
                   const renderer::MapRenderer& renderer,
                   const router::TransportRouter& router) :
                                                            db_{db}, 
                                                            renderer_{renderer}, 
                                                            router_{router} {
    }

    // возвращает svg::Document с картой маршрутов
    svg::Document RenderMap() const;

    const TransportCatalogue& GetTransportCatalogue() const;

    // ищет подходящий маршрут
    std::optional<graph::Router<router::TravelTime>::RouteInfo> GetRoute(std::string_view from, std::string_view to) const;

    const graph::DirectedWeightedGraph<router::TravelTime>& GetGraph() const;

private:
    // возвращает указатели на координаты всех уникальных остановок
    const std::unordered_set<const geo::Coordinates*> GetStopsCoord(
        const std::unordered_map<std::string_view, const domain::Bus*>& routes) const;

    // возвращает вектор указателей на остановки, отсортированный по алфавиту
    // остановки, через которые не проходят автобусы в векторе отсутствуют
    const std::vector<const domain::Stop*> GetOrderedStops() const;

    const catalogue::TransportCatalogue& db_;
    const renderer::MapRenderer& renderer_;
    const router::TransportRouter& router_;
};