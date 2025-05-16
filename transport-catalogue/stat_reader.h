#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace catalogue {
namespace stat_reader {

void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                       std::ostream& output);

void GetResult(const TransportCatalogue& transport_catalogue, std::istream& input, std::ostream& output);
}// namespace stat_reader
}// namespace catalogue