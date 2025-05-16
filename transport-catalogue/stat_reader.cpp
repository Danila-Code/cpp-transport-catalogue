#include <iostream>
#include "stat_reader.h"

namespace catalogue {

using std::string, std::string_view, std::istream, std::ostream, std::endl;
namespace stat_reader {
namespace detail {

std::pair<string_view, string_view> ParseRequest(string_view request) { 
    size_t space_pos = request.find(' ');
    if(space_pos == request.npos) {
        return {};
    }
    
    return {request.substr(0, space_pos) ,request.substr(space_pos + 1)};
}

void PrintBusInfo(const TransportCatalogue& transport_catalogue, string_view name, ostream& output) {
    output << "Bus " << name << ": ";

    auto bus = transport_catalogue.GetBus(name);
    
    if(!bus) {
        output << "not found" << endl;
        return;
    }
    
    auto bus_stats = transport_catalogue.GetRouteInfo(bus);

    output << bus_stats.stops_num << " stops on route, " << bus_stats.unique_stops << " unique stops, "
           << bus_stats.length << " route length" << endl;
}

void PrintStopInfo(const TransportCatalogue& transport_catalogue, string_view name, ostream& output) {
    output << "Stop " << name << ": ";

    auto stop = transport_catalogue.GetStop(name);
    if(!stop) {
        output << "not found" << endl;
        return;
    }

    auto stop_info = transport_catalogue.GetStopInfo(stop);
    if(stop_info.empty()) {
        output << "no buses" << endl;
        return;
    }

    output << "buses";
    for(auto bus : stop_info) {
        output << " " << bus; 
    }
    output << std::endl;
}
} // namespace detail

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, string_view request, ostream& output) {
    using namespace detail;

    auto [request_type, name] = ParseRequest(request);
    if(name.empty()) {
        return;
    }

    if(request_type == "Bus") {
        PrintBusInfo(transport_catalogue, name, output);
        return;
    }

    if(request_type == "Stop") {
        PrintStopInfo(transport_catalogue, name, output);
        return;
    }
}

void GetResult(const TransportCatalogue& transport_catalogue, istream& input, ostream& output) {
    int request_count;
    input >> request_count;
    {
        string line;
        getline(input, line);
    }

    for (int i = 0; i < request_count; ++i) {
        string line;
        getline(input, line);
        stat_reader::ParseAndPrintStat(transport_catalogue, line, output);
    }
}

}// namespace stat_reader
}// nammespace catalogue