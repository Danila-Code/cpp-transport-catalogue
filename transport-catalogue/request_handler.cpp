#include "request_handler.h"

// возвращает svg::Document с картой маршрутов
svg::Document RequestHandler::RenderMap() const {
    auto routes = db_.GetAllRoutes();
    std::map<std::string_view, const domain::Bus*> ordered_routes{routes.begin(), routes.end()};

    return renderer_.Render(GetStopsCoord(routes), ordered_routes, GetOrderedStops());
}

const TransportCatalogue& RequestHandler::GetTransportCatalogue() const {
    return db_;
}

// возвращает вектор указателей на остановки, отсортированный по алфавиту
// остановки, через которые не проходят автобусы в векторе отсутствуют
const std::vector<const domain::Stop*> RequestHandler::GetOrderedStops() const {
    const auto& routes = db_.GetAllRoutes();
    
    auto comp = [](const domain::Stop* lhs, const domain::Stop* rhs) {
        return lhs->name.compare(rhs->name) < 0;
    };

    std::set<const domain::Stop*, decltype(comp)> stops;

    for(const auto& [route_name, ptr_bus] : routes) {
        for(const auto& stop : ptr_bus->stops) {
            stops.insert(stop);
        }
    }
    return {stops.begin(), stops.end()};
}

// возвращает указатели на координаты всех уникальных остановок
const std::unordered_set<const geo::Coordinates*> RequestHandler::GetStopsCoord(
    const std::unordered_map<std::string_view, const domain::Bus*>& routes) const {

    std::unordered_set<const geo::Coordinates*> res;

    for(auto& [name, ptr_bus] : routes) {        
        for(const auto& stop : ptr_bus->stops) {
            res.insert(&stop->coord);
        }     
    }

    return res;
}

// ищет подходящий маршрут
std::optional<router::ResultRoute> RequestHandler::GetRoute(std::string_view from, std::string_view to) const {
    return router_.BuildRoute(db_.GetStop(from), db_.GetStop(to));
}