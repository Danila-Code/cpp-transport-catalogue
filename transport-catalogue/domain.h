#pragma once

#include <string>
#include <vector>

#include "geo.h"

namespace domain {

using namespace geo;
    
struct Stop {
    std::string name;
    Coordinates coord;
};

struct Bus {
    std::string name;
    std::vector<const Stop*> stops;
    bool is_roundtrip;
};

struct BusStats {
    size_t stops_num;
    size_t unique_stops;
    size_t length_f;
    double curvature;
};

class StopHasher {
public:
    size_t operator()(std::pair<const Stop*, const Stop*> value) const noexcept;
private:
    std::hash<const void*> hasher;
};

}// namespace domain