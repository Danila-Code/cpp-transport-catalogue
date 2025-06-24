#include <iostream>

#include <fstream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace catalogue;
using namespace renderer;

int main() {
    TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(cin);
    reader.ApplyBaseRequests(catalogue);

    MapRenderer renderer(reader.GetRenderSettings());

    RequestHandler handler(catalogue, renderer);
    
    reader.ApplyStatRequests(catalogue, handler, cout);
}