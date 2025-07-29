#include <iostream>

#include <fstream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace catalogue;
using namespace renderer;
using namespace router;

int main() {
    using namespace std::literals;
    std::ifstream in("final_input_2.json"s);

    TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(in);
    reader.FillTransportCatalogue(catalogue);

    MapRenderer renderer(reader.GetRenderSettings());
    TransportRouter router(reader.GetRoutingSettings(), catalogue);

    RequestHandler handler(catalogue, renderer, router);
    
    std::ofstream out("final_output_2.json");
    reader.ApplyStatRequests(handler, out);
}