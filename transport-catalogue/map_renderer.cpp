#include "map_renderer.h"

using namespace catalogue;

namespace renderer {

// возвращает итератор на следующий цвет в векторе цветов зацикленно
std::vector<svg::Color>::const_iterator MapRenderer::GetNextColor(std::vector<svg::Color>::const_iterator& color_iter) const {
    auto old_iter = color_iter;
    ++color_iter;
    if(color_iter == settings_.color_palette.end()) {
        color_iter = settings_.color_palette.begin();
    }
    return old_iter;
}

MapRenderer::MapRenderer(const detail::RenderSettings& settings) : settings_{settings} {
}

// добавляет ломаные линии маршрутов
void MapRenderer::AddRoutePolyline(svg::Document& doc, const std::vector<const domain::Stop*>& stops,
    const svg::Color& color, const SphereProjector& proj) const {
    
    svg::Polyline route;

    route.SetStrokeColor(color);
    route.SetFillColor(svg::NoneColor);
    route.SetStrokeWidth(settings_.line_width);
    route.SetStrokeLineCap(svg::StrokeLineCap::ROUND);
    route.SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
    
    for(const auto& stop : stops) {
        route.AddPoint(proj(stop->coord));
    }

    doc.Add(route);
}

// добавляет полилинию маршрута
void MapRenderer::AddRoutes(svg::Document& doc, const std::map<std::string_view, const domain::Bus*>& routes, 
    const SphereProjector& proj) const {

    /*auto GetNextColor = [this]( auto& color_iter) {
        auto old_iter = color_iter;
        ++color_iter;
        if(color_iter == settings_.color_palette.end()) {
            color_iter = settings_.color_palette.begin();
        }
        return old_iter;
    };*/

    auto iter_color = settings_.color_palette.begin();
    for(const auto& [name, ptr_bus] : routes) {
        AddRoutePolyline(doc, ptr_bus->stops, *GetNextColor(iter_color), proj);
    }

}

// добавляет названия маршрутов
void MapRenderer::AddRoutesNames(svg::Document& doc, const std::map<std::string_view,
    const domain::Bus*>& buses, const SphereProjector& proj) const {

    /*auto GetNextColor = [this]( auto& color_iter) {
        auto old_iter = color_iter;
        ++color_iter;
        if(color_iter == settings_.color_palette.end()) {
            color_iter = settings_.color_palette.begin();
        }
        return old_iter;
    };*/    

    auto color_iter = settings_.color_palette.begin();

    for(const auto& [name, ptr_bus] : buses) {

        if(ptr_bus->stops.empty()) {
            continue;
        } 

        svg::Point pos = proj(ptr_bus->stops[0]->coord);
        auto color_iter2 = GetNextColor(color_iter);
        AddTextLabel(doc, name, pos, *color_iter2, true);

        // совпадают ли крайние остановки некольцевого маршрута или нет
        bool is_edge_stops_eq = *ptr_bus->stops.begin() == *(ptr_bus->stops.begin() + ptr_bus->stops.size() / 2);
        
        if(ptr_bus->is_roundtrip == false && ptr_bus->stops.size() > 1 && !is_edge_stops_eq) {
            pos = proj((*(ptr_bus->stops.begin() + ptr_bus->stops.size() / 2))->coord);
            AddTextLabel(doc, name, pos, *color_iter2, true);
        }
    }
}

// добавляет текст - название
void MapRenderer::AddTextLabel(svg::Document& doc, std::string_view text, const svg::Point& pos, 
    const svg::Color& color, bool is_route_label) const {
    using namespace std::literals;

    svg::Text base_text;
    base_text.SetData(std::string(text))
             .SetPosition(pos)
             .SetFontFamily("Verdana"s);
    
    if(is_route_label) {
        base_text.SetOffset(settings_.bus_label_offset)
                 .SetFontSize(settings_.bus_label_font_size)
                 .SetFontWeight("bold"s);
    } else {
        base_text.SetOffset(settings_.stop_label_offset)
                 .SetFontSize(settings_.stop_label_font_size);
    }

    // подложка под основной текст
    doc.Add(svg::Text{base_text}.SetFillColor(settings_.underlayer_color)
                               .SetStrokeColor(settings_.underlayer_color)
                               .SetStrokeWidth(settings_.underlayer_width)
                               .SetStrokeLineCap(svg::StrokeLineCap::ROUND)
                               .SetStrokeLineJoin(svg::StrokeLineJoin::ROUND));
    // основной текст названия
    doc.Add(svg::Text{base_text}.SetFillColor(color));
}

// добавляет круги, обозначающие остановки
void MapRenderer::AddStopsCircles(svg::Document& doc, const std::vector<const domain::Stop*>& stops, 
    const SphereProjector& proj) const {
    for(const auto& stop : stops) {
        AddStopCircle(doc, proj(stop->coord));
    }
}

// добавляет круг, обозначающие остановку
void MapRenderer::AddStopCircle(svg::Document& doc, const svg::Point& pos) const {
    using namespace std::literals;

    doc.Add(svg::Circle()
                         .SetCenter(pos)
                         .SetRadius(settings_.stop_radius)
                         .SetFillColor("white"s));
}

// добавляет названия остановок
void MapRenderer::AddStopsNames(svg::Document& doc, const std::vector<const domain::Stop*>& stops, 
    const SphereProjector& proj) const {
    using namespace std::literals;

    for(const auto& stop : stops) {
        AddTextLabel(doc, stop->name, proj(stop->coord), svg::Color{"black"s}, false);
    }
}

// Добавляет маршруты, остановки, названия в svg::Document
svg::Document MapRenderer::Render(const std::unordered_set<const geo::Coordinates*>& coords, 
                                  const std::map<std::string_view, const domain::Bus*>& buses, 
                                  const std::vector<const domain::Stop*>& stops) const {
    
    SphereProjector proj = InitProjector(coords);

    svg::Document doc;
    // добавляем ломаные линии маршрутов
    AddRoutes(doc, buses, proj);
    // добавляем названия маршрутов
    AddRoutesNames(doc, buses, proj);
    // добавляем круги, обозначающие остановки
    AddStopsCircles(doc, stops, proj);
    // добавляем названия остановок
    AddStopsNames(doc, stops, proj);

    return doc;
}

SphereProjector MapRenderer::InitProjector(const std::unordered_set<const geo::Coordinates*>& coords) const {   
    return SphereProjector{coords.begin(), coords.end(), 
        settings_.width, settings_.height, settings_.padding};
}

}  // namespace renderer