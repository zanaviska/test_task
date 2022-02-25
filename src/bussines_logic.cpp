#include <iostream>
#include <map>

#include "json.hpp"
#include "sm_receiver.h"
#include "sm_sender.h"

int convert_symbol_to_idx(const std::string &symbol)
{
    if (symbol == "AAPL") return 0;
    if (symbol == "IBM") return 1;
    return 2;
}

int convert_company_to_idx(const std::string &symbol)
{
    if (symbol == "ABC Inc") return 1;
    if (symbol == "XYZ Inc") return 2;
    return 0;
}

int table[2][3] = {{100, 90, 95}, {120, 110, 112}};

std::string bussiness_logic(const std::string &request)
{
    auto json = nlohmann::json::parse(request);
    int symbol_idx = convert_symbol_to_idx(json["symbol"]);
    int company_idx = convert_company_to_idx(json["company"]);
    int ordered = json["numOrdered"];

    if (symbol_idx == 2) return "ERROR NOT KNOWN";

    return std::to_string(ordered * table[symbol_idx][company_idx]);
}

int main(int argc, char *argv[])
{

    std::cerr << "This is BO\n";

    sm_sender sender(SHM_KEY_1);
    sm_receiver receiver(SHM_KEY_2);

    try
    {
        while (true)
        {
            std::string request = receiver.receive();
            std::cout << request << '\n';

            sender.send(bussiness_logic(request));
            std::cout << '\n';
        }
    }
    catch (const std::exception &e)
    {
        sender.close();
        std::cerr << e.what();
    }

    return 0;
}