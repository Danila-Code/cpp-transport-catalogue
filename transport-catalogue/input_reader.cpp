#include "input_reader.h"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <iostream>
#include <cmath>

namespace catalogue {
namespace input_reader {
namespace detail {
/**
 * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
 */
Coordinates ParseCoordinates(std::string_view str) {
    static const double nan = std::nan("");

    auto not_space = str.find_first_not_of(' ');
    auto comma = str.find(',');

    if (comma == str.npos) {
        return {nan, nan};
    }

    auto not_space2 = str.find_first_not_of(' ', comma + 1);
    auto comma2 = str.find(',', not_space2 + 1);

    double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
    double lng = std::stod(std::string(str.substr(not_space2, comma2 - not_space2)));

    return {lat, lng};
}

/**
 * Удаляет пробелы в начале и конце строки
 */
std::string_view Trim(std::string_view string) {
    const auto start = string.find_first_not_of(' ');
    if (start == string.npos) {
        return {};
    }
    return string.substr(start, string.find_last_not_of(' ') + 1 - start);
}

/**
 * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
 */
std::vector<std::string_view> Split(std::string_view string, char delim) {
    std::vector<std::string_view> result;

    size_t pos = 0;
    while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
        auto delim_pos = string.find(delim, pos);
        if (delim_pos == string.npos) {
            delim_pos = string.size();
        }
        if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
            result.push_back(substr);
        }
        pos = delim_pos + 1;
    }

    return result;
}
/**
 * Парсит строку вида " 3900m to Marushkino" и возвращает пару (расстояние, название остановки)
 */
std::pair<std::string_view, size_t> ParseDistance(std::string_view str) {
    size_t m_pos = str.find('m');

    if(m_pos == str.npos) {
        return {};
    }

    size_t t_pos = str.find('t', m_pos+2);
    if(t_pos == str.npos) {
        return {};
    }

    size_t distance = stoi(std::string(Trim(str.substr(0, m_pos))));
    return {Trim(str.substr(t_pos + 3)), distance};
}
/**
 * Парсит описание остановки и возвращает вектора расстояний до остановок
 */
std::unordered_map<std::string_view, size_t> ParseStopDescription(std::string_view str) {
    size_t comma_pos = str.find(',');
    if(comma_pos == str.npos) {
        return {};
    }
    comma_pos = str.find(',', comma_pos + 1);
    if(comma_pos == str.npos) {
        return {};
    }

    std::vector<std::string_view> stop_distances = Split(str.substr(comma_pos + 1), ',');
    if(stop_distances.empty()) {
        return {};
    }

    std::unordered_map<std::string_view, size_t> results;
    results.reserve(stop_distances.size());
    for(auto& distance : stop_distances) {
        results.insert(ParseDistance(distance));
    }
    return results;
}

/**
 * Парсит маршрут.
 * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
 * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
 */
std::vector<std::string_view> ParseRoute(std::string_view route) {
    if (route.find('>') != route.npos) {
        return Split(route, '>');
    }

    auto stops = Split(route, '-');
    std::vector<std::string_view> results(stops.begin(), stops.end());
    results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

    return results;
}

CommandDescription ParseCommandDescription(std::string_view line) {
    auto colon_pos = line.find(':');
    if (colon_pos == line.npos) {
        return {};
    }

    auto space_pos = line.find(' ');
    if (space_pos >= colon_pos) {
        return {};
    }

    auto not_space = line.find_first_not_of(' ', space_pos);
    if (not_space >= colon_pos) {
        return {};
    }

    return {std::string(line.substr(0, space_pos)),
            std::string(line.substr(not_space, colon_pos - not_space)),
            std::string(line.substr(colon_pos + 1))};
}
}// namespace detail

using namespace detail;

void InputReader::ParseLine(std::string_view line) {
    auto command_description = ParseCommandDescription(line);
    if (command_description) {
        commands_.push_back(std::move(command_description));
    }
}

void InputReader::ApplyCommands([[maybe_unused]] TransportCatalogue& catalogue){
    auto first_bus = std::partition(commands_.begin(), commands_.end(), [](CommandDescription command) {
        return command.command == "Stop";
    });

    std::vector<CommandDescription> stops_commands(std::make_move_iterator(commands_.begin()), std::make_move_iterator(first_bus));
    std::vector<CommandDescription> buses_commands(std::make_move_iterator(first_bus), std::make_move_iterator(commands_.end()));

    for(auto& command : stops_commands) {
            catalogue.AddStop({command.id, ParseCoordinates(command.description)});
    }

    for(auto& command : stops_commands) {
            auto description = ParseStopDescription(command.description);
            if(description.empty()) {
                continue;
            }
            for(auto& [to, distance] : description) {
                catalogue.AddDistanceBetweenStops(command.id, to, distance);
            }
    }

    for(auto& command : buses_commands) {
        Bus bus{command.id, {}};
        auto route = ParseRoute(command.description);
        bus.stops.reserve(route.size());
        std::for_each(route.begin(), route.end(), [&](std::string_view stop) {
            bus.stops.push_back(catalogue.GetStop(stop));
        });
        catalogue.AddBus(bus);
    }
}

// Чтение апросов и заполнение каталога
void FillCatalogue(TransportCatalogue& catalogue, std::istream& input) {
    int request_count ;
    input >> request_count;
    {
        std::string line;
        getline(input, line);
    }

    input_reader::InputReader reader;

    for(int i = 0; i < request_count; ++i) {
        std::string line;
        getline(input, line);
        reader.ParseLine(line);
    }
    reader.ApplyCommands(catalogue);
}
}// namespace input_reader
}// namespace catalogue