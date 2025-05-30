#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    using namespace catalogue;

    TransportCatalogue catalogue;
    {
        input_reader::InputReader reader;
        input_reader::FillCatalogue(catalogue, cin);
    }
    stat_reader::GetResult(catalogue, cin, cout);
}