#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace catalogue;
using namespace renderer;
using namespace router;

int main() {
    using namespace std::literals;

    TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(cin);
    reader.FillTransportCatalogue(catalogue);

    MapRenderer renderer(reader.GetRenderSettings());
    TransportRouter router(reader.GetRoutingSettings(), catalogue);

    RequestHandler handler(catalogue, renderer, router);
    
    reader.ApplyStatRequests(handler, cout);
}