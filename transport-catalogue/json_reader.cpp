#include "json_reader.h"

#include <stdexcept>
#include <sstream>

using namespace std::literals;

namespace catalogue {
namespace json_reader {

namespace {

// Преобразует json::Node в svg::Color
svg::Color NodeToColor(const json::Node& node) {
    if(node.IsArray()) {
        auto& array = node.AsArray();
        if(array.size() == 3) {
            return svg::Color{svg::Rgb{static_cast<uint8_t>(array[0].AsInt()), 
                                       static_cast<uint8_t>(array[1].AsInt()), 
                                       static_cast<uint8_t>(array[2].AsInt())}};
        }
        return svg::Color{svg::Rgba{static_cast<uint8_t>(array[0].AsInt()), 
                                    static_cast<uint8_t>(array[1].AsInt()), 
                                    static_cast<uint8_t>(array[2].AsInt()), 
                                    array[3].AsDouble()}};
    }
    return svg::Color{node.AsString()};
}    

// Преобразует вектор json::Node'ов в вектор svg::Color'ов
std::vector<svg::Color> ArrayToColorVector(const json::Array& array) {
    std::vector<svg::Color> colors;
    for(const auto& node : array) {
        colors.push_back(std::move(NodeToColor(node)));
    }
    return colors;
}

//Возвращает словарь, заполненный информацией о маршруте
json::Dict GetBusInfo(const TransportCatalogue& transport_catalogue, const json::Dict& bus_request) {
    auto bus = transport_catalogue.GetBus(bus_request.at("name"s).AsString());

    if(!bus) {
        return {{"request_id"s, bus_request.at("id"s).AsInt()}, 
                {"error_message"s, "not found"s}};
    }

    auto bus_stats = transport_catalogue.GetRouteInfo(bus);
    
    int stop_count;
    
    try {
        stop_count = static_cast<int>(bus_stats.unique_stops);
    } catch(std::overflow_error& e) {
        throw std::overflow_error("Overflow: there are too many stops count, it is not possible to output in json!"s);
    }

    return {{"curvature"s, bus_stats.curvature},
            {"request_id"s, bus_request.at("id"s).AsInt()},
            {"route_length"s, static_cast<int>(bus_stats.length_f)},
            {"stop_count"s, static_cast<int>(bus_stats.stops_num)},
            {"unique_stop_count"s, stop_count}};
        // Возможно переполнение при преобразовании из size_t в int
}

//Возвращает словарь, заполненный информацией об остановке
json::Dict GetStopInfo(const TransportCatalogue& transport_catalogue, const json::Dict& stop_request) {
    json::Dict stop_info{{"request_id"s, stop_request.at("id"s).AsInt()}};
    auto stop = transport_catalogue.GetStop(stop_request.at("name"s).AsString());

    if(!stop) {
        stop_info.insert({"error_message"s, "not found"s});
        return stop_info;
    }

    auto stop_stats = transport_catalogue.GetStopInfo(stop);

    json::Array buses{};
    for(std::string_view bus : stop_stats) {
        buses.push_back(std::string(bus));
    }
    stop_info.insert({"buses"s, buses});

    return stop_info;
}

//Возвращает словарь, заполненный информацией, необходимой для отрисовки маршрутов
json::Dict GetRoutesMap(const RequestHandler& request_handler, const json::Dict& map_request) {
    using namespace std::literals;

    std::ostringstream out_str;
    request_handler.RenderMap().Render(out_str);

    return json::Dict{{"request_id"s, map_request.at("id"s).AsInt()},
                      {"map"s, out_str.str()}};
}
}  // namespace

JsonReader::JsonReader(std::istream& input) {
    dict_ = json::Load(input).GetRoot().AsMap();
}

// заполняет каталог данными
void JsonReader::FillTransportCatalogue(TransportCatalogue& catalogue) const {
    if(dict_.contains("base_requests"s)) {
        const auto& array = dict_.at("base_requests"s).AsArray();
        // Заполняем каталог остановками с координатами
        for(const auto& item : array) {
            const auto& item_info = item.AsMap();

            if(item_info.at("type"s).AsString() == "Stop"s) {
                catalogue.AddStop({item_info.at("name"s).AsString(), 
                    {item_info.at("latitude"s).AsDouble(), item_info.at("longitude"s).AsDouble()}});
            }
        }
        // Заполняем каталог расстояниями между остановок
        for(const auto& item : array) {
            const auto& item_info = item.AsMap();

            if(item_info.at("type"s).AsString() == "Stop"s) {
                for(const auto& [to, distance] : item_info.at("road_distances"s).AsMap()) {
                    catalogue.AddDistanceBetweenStops(item_info.at("name"s).AsString(), 
                        to, distance.AsInt());
                }
            }
        }
        // Заполняем каталог автобусными маршрутами
        for(const auto& item : array) {
            const auto& item_info = item.AsMap();

            if(item_info.at("type"s).AsString() == "Bus"s) {
                Bus bus{item_info.at("name"s).AsString(), {}, item_info.at("is_roundtrip"s).AsBool()};
                
                const auto& stops = item_info.at("stops"s).AsArray();
                bus.stops.reserve(bus.is_roundtrip ? stops.size() : stops.size() * 2 - 1);

                for(const auto& stop : stops) {
                    bus.stops.push_back(catalogue.GetStop(stop.AsString()));
                }

                // Разворачиваем некольцевой маршрут
                if(bus.is_roundtrip == false) {
                    std::copy(bus.stops.rbegin() + 1, bus.stops.rend(), std::back_inserter(bus.stops));
                }
                catalogue.AddBus(bus);
            }
        }

    }
}

// выводит в output результаты запросов "stat_requests"
void JsonReader::ApplyStatRequests(const RequestHandler& request_handler, std::ostream& output) const {
    json::Array root;

    if(dict_.contains("stat_requests"s)) {
        const auto& array = dict_.at("stat_requests"s).AsArray();

        for(const auto& item : array) {
            const auto& request_info = item.AsMap();

            if(request_info.at("type"s).AsString().empty()) {
                continue;
            }

            if(request_info.at("type"s).AsString() == "Bus"s) {
                root.push_back(GetBusInfo(request_handler.GetTransportCatalogue(), request_info));
                continue;
            }

            if(request_info.at("type"s).AsString() == "Stop"s) {
                root.push_back(GetStopInfo(request_handler.GetTransportCatalogue(), request_info));
                continue;
            }

            if(request_info.at("type"s).AsString() == "Map"s) {
                root.push_back(GetRoutesMap(request_handler, request_info));
            }
        }
    }
    json::Print(json::Document{root}, output);
}

// возвращает настройки для отрисовки маршрутов
renderer::detail::RenderSettings JsonReader::GetRenderSettings() const {
    const auto& render_settings = dict_.at("render_settings"s).AsMap();

    return {.width = render_settings.at("width"s).AsDouble(),
            .height = render_settings.at("height"s).AsDouble(),
            .padding = render_settings.at("padding"s).AsDouble(),
            .line_width = render_settings.at("line_width"s).AsDouble(),
            .stop_radius = render_settings.at("stop_radius"s).AsDouble(),
            .bus_label_font_size = render_settings.at("bus_label_font_size"s).AsInt(),
            .bus_label_offset{render_settings.at("bus_label_offset"s).AsArray()[0].AsDouble(),
                render_settings.at("bus_label_offset"s).AsArray()[1].AsDouble()},
            .stop_label_font_size = render_settings.at("stop_label_font_size"s).AsInt(),
            .stop_label_offset{render_settings.at("stop_label_offset"s).AsArray()[0].AsDouble(),
                render_settings.at("stop_label_offset"s).AsArray()[1].AsDouble()},
            .underlayer_color = NodeToColor(render_settings.at("underlayer_color"s)),
            .underlayer_width = render_settings.at("underlayer_width"s).AsDouble(),
            .color_palette = ArrayToColorVector(render_settings.at("color_palette"s).AsArray())};
}

}// namespace json_reader
}// namespace catalogue