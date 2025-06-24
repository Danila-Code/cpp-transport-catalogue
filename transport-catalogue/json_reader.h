#pragma once

#include "json.h"
#include "request_handler.h"

namespace catalogue {
namespace json_reader {

class JsonReader {
public:
    explicit JsonReader(std::istream& input);

    // заполняет каталог данными
    void FillTransportCatalogue(TransportCatalogue& catalogue) const;

    // выводит в output результаты запросов "stat_requests"
    void ApplyStatRequests(const RequestHandler& request_handler, std::ostream& output) const;
    
    // возвращает настройки для отрисовки маршрутов
    renderer::detail::RenderSettings GetRenderSettings() const;

private:
    json::Dict dict_;
};

}// namespace json_reader
}// namespace ctalogue