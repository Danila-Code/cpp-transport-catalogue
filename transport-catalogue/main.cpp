#include <iostream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace catalogue;
using namespace renderer;

int main() {
    TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(cin);
    reader.FillTransportCatalogue(catalogue);

    MapRenderer renderer(reader.GetRenderSettings());

    RequestHandler handler(catalogue, renderer);
    
    reader.ApplyStatRequests(handler, cout);
}