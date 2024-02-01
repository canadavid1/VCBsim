#include <atomic>
#include <vector>
#include <iostream>
#include "VCBsim.h"




// struct ListType
// {
//     const std::vector<uptr> data;
//     ListType(std::vector<uptr> vec) : data(std::move(vec)) {}
//     std::vector<uptr>::const_iterator begin() {return data.cbegin();}
//     std::vector<uptr>::const_iterator end() {return data.cend();}
// };


ucount component::process(std::vector<wire>& wires) { // updates, and returns how many events.
    bool newstate;
    switch (id)
    {
    case AND:
        newstate = num_driven == tot_in && tot_in > 0;
        break;
    case NAND:
        newstate = num_driven < tot_in;
        break;
    case OR:
        newstate = num_driven > 0;
        break;
    case NOR:
        newstate = num_driven == 0;
        break;
    case XOR:
        newstate = num_driven & 1;
        break;
    case XNOR:
        newstate = !(num_driven & 1);
        break;
    case LATCH:
        newstate = state ^ (num_driven & 1);
        num_driven = 0;
        break;
    }
    if (newstate==state) return 0;
    state = newstate;
    if(newstate)
    {
        for(auto i : output) wires[i].num_driven++;
        return output.size();
    }
    else
    {
        for(auto i : output) wires[i].num_driven--;
        return output.size();
    }
}


ucount wire::process(std::vector<component>& components)
{
    bool newstate = num_driven > 0;
    if(newstate == state) return 0;
    state = newstate;
    if (newstate)
    {
        for(auto i : output) components[i].num_driven++;
        for(auto i : edge_output) components[i].num_driven++;
        return output.size()+edge_output.size();
    }
    else
    {
        for(auto i : output) components[i].num_driven--;
        #ifdef USE_VCB_EVENT_COUNT
            return output.size()+edge_output.size();
        #endif
        return output.size();
    }
}



int main()
{

    std::vector<component> components;
    std::vector<wire> wires;
    // Input, for testing purposes
    int N,M; // number of components and wires
    std::cin >> N >> M;
    // components
    components.reserve(N);
    for(int i = 0; i < N; i++)
    {
        ucount tot_in;
        int id;
        int num_out;
        std::cin >> tot_in >> id >> num_out;
        ListType output(num_out);
        for(auto& j : output) std::cin >> j;
        components.emplace_back(tot_in,(ComponentId)id,std::move(output));
    }

    wires.reserve(M);
    for(int i = 0; i < M; i++)
    {
        int num_out;
        std::cin >> num_out;
        ListType output;
        ListType edge_out;
        for(int j = 0; j < num_out; j++)
        {
            uptr k;
            std::cin >> k;
            if(components[k].id == LATCH) edge_out.push_back(k);
            else output.push_back(k);
        }
        wires.emplace_back(std::move(output),std::move(edge_out));
    }
    unsigned long long ev = 0;
    unsigned long long tick = 0;
    for(;tick < 8;)
    {
        auto len = std::max(components.size(),wires.size());
        std::cout << "            ";
        for(int i = 0; i < len; i++) std::cout << (i% 10) << " ";
        std::cout << "\n";
        std::cout << "Components: ";
        for(auto& k : components) std::cout << k.state << " ";
        std::cout << "\n";
        std::cout << "Wires:      ";
        for(auto& k : wires) std::cout << k.state << " ";
        std::cout << "\n\n";
        std::cin.ignore();
        tick++;
        for(auto& k : components) ev+=k.process(wires);
        for(auto& k : wires) ev+=k.process(components);
        std::cout << ev << "\n";
    }
    
}