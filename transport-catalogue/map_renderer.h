#pragma once

#include <vector>
#include <algorithm>
#include <map>
#include <unordered_set>

#include "transport_catalogue.h"
#include "svg.h"
#include "domain.h"

namespace renderer {

namespace {

inline const double EPSILON = 1e-6;
inline bool IsZero(double value) {
    return std::abs(value) < EPSILON;
}

class SphereProjector {
public:
    // points_begin и points_end задают начало и конец интервала элементов geo::Coordinates
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                    double max_width, double max_height, double padding)
        : padding_(padding) //
    {
        // Если точки поверхности сферы не заданы, вычислять нечего
        if (points_begin == points_end) {
            return;
        }

        // Находим точки с минимальной и максимальной долготой
        const auto [left_it, right_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs->lng < rhs->lng; });
        min_lon_ = (*left_it)->lng;
        const double max_lon = (*right_it)->lng;

        // Находим точки с минимальной и максимальной широтой
        const auto [bottom_it, top_it] = std::minmax_element(
            points_begin, points_end,
            [](auto lhs, auto rhs) { return lhs->lat < rhs->lat; });
        const double min_lat = (*bottom_it)->lat;
        max_lat_ = (*top_it)->lat;

        // Вычисляем коэффициент масштабирования вдоль координаты x
        std::optional<double> width_zoom;
        if (!IsZero(max_lon - min_lon_)) {
            width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
        }

        // Вычисляем коэффициент масштабирования вдоль координаты y
        std::optional<double> height_zoom;
        if (!IsZero(max_lat_ - min_lat)) {
            height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
        }

        if (width_zoom && height_zoom) {
            // Коэффициенты масштабирования по ширине и высоте ненулевые,
            // берём минимальный из них
            zoom_coeff_ = std::min(*width_zoom, *height_zoom);
        } else if (width_zoom) {
            // Коэффициент масштабирования по ширине ненулевой, используем его
            zoom_coeff_ = *width_zoom;
        } else if (height_zoom) {
            // Коэффициент масштабирования по высоте ненулевой, используем его
            zoom_coeff_ = *height_zoom;
        }
    }

    // Проецирует широту и долготу в координаты внутри SVG-изображения
    svg::Point operator()(geo::Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;
};
}  // namespace

namespace detail {
// Настройки визуализации
struct RenderSettings {
    // размеры изображения
    double width{};
    double height{};

    double padding{};  // отступ от краёв

    double line_width{};  // тощина линий для рисования маршрутов
    double stop_radius{};  // радиус окружности обозначения остановок

    int bus_label_font_size{};  // размер текста названий маршрутов
    svg::Point bus_label_offset{};  // смещения надписи названия маршрута относительно координат конечной остановки на карте

    int stop_label_font_size{};  // размер текста названий остановок
    svg::Point stop_label_offset{};  // смещения надписи названия остановки относительно её координат на карте

    svg::Color underlayer_color{};  // цвет подложки под названием маршрутов
    double underlayer_width{};  // толщина подложки под названием маршрутов

    std::vector<svg::Color> color_palette{};  // цветовая палитра
};
}  // namespace detail

class MapRenderer {
public:
    explicit MapRenderer(const detail::RenderSettings& settings);

    // Добавляет маршруты, остановки, названия в svg::Document
    svg::Document Render(const std::unordered_set<const geo::Coordinates*>& coords, 
        const std::map<std::string_view, const domain::Bus*>& buses, 
        const std::vector<const domain::Stop*>& stops) const;

private:
    // возвращает итератор на следующий цвет в векторе цветов зацикленно
    std::vector<svg::Color>::const_iterator GetNextColor(std::vector<svg::Color>::const_iterator& color_iter) const;

    // добавляет ломаные линии маршрутов
    void AddRoutes(svg::Document& doc, const std::map<std::string_view, const domain::Bus*>& buses, 
        const SphereProjector& proj) const;

    // добавляет названия маршрутов
    void AddRoutesNames(svg::Document& doc, const std::map<std::string_view, const domain::Bus*>& buses, 
        const SphereProjector& proj) const;

    // добавляет круги, обозначающие остановки   
    void AddStopsCircles(svg::Document& doc, const std::vector<const domain::Stop*>& stops, 
        const SphereProjector& proj) const;

    // добавляет названия остановок
    void AddStopsNames(svg::Document& doc, const std::vector<const domain::Stop*>& stops, 
    const SphereProjector& proj) const;

    SphereProjector InitProjector(const std::unordered_set<const geo::Coordinates*>& coords) const;

    // добавляет полилинию маршрута
    void AddRoutePolyline(svg::Document& doc, const std::vector<const domain::Stop*>& stops, 
        const svg::Color& color, const SphereProjector& proj) const;

    // добавляет текст - название
    void AddTextLabel(svg::Document& doc, std::string_view text, const svg::Point& pos, 
        const svg::Color& color, bool is_route_label) const;
        
    // добавляет круг, обозначающие остановку
    void AddStopCircle(svg::Document& doc, const svg::Point& pos) const;

    detail::RenderSettings settings_;
};

}  // namespace renderer