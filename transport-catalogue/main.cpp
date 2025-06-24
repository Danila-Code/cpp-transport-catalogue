#include <iostream>

#include <fstream>

#include "json_reader.h"
#include "request_handler.h"

using namespace std;
using namespace catalogue;
using namespace renderer;

int main() {
    ifstream file("s10_final_opentest_3.json"s);

    TransportCatalogue catalogue;
    
    json_reader::JsonReader reader(file);
    reader.FillTransportCatalogue(catalogue);
    MapRenderer renderer(reader.GetRenderSettings());


    RequestHandler handler(catalogue, renderer);

    ofstream output("s10_final_opentest_3_my_answer.json"s);
    reader.ApplyStatRequests(handler, output);
}