
//#define USE_VCB_EVENT_COUNT


#include <vector>
using u8 = unsigned char;
using ucount = unsigned short; // type for counting the number that are on
using uptr = unsigned int; // stores indicies into the wire and component 
using ListType = std::vector<uptr>;
using atomic_ucount = ucount; // no atomics for now
struct wire;
struct component;

enum ComponentId : u8 {
    AND=0,
    NAND=1,
    OR=2,
    NOR=3,
    XOR=4,
    XNOR=5,
    LATCH=6,
    LATCH_ON=7,
};

struct component
{
    atomic_ucount num_driven = 0;
    const ucount tot_in;
    const ComponentId id;
    bool state = 0;
    const ListType output;
    
    component(ucount in, ComponentId id, ListType output) 
        : tot_in(in),id(id),output(std::move(output)) 
    {
        if(id!=LATCH_ON) return;
        id = LATCH;
        state = 1;
    }
    ucount process(std::vector<wire>&);
};

struct wire {
    atomic_ucount num_driven = 0;
    bool state=0;
    const ListType output;
    const ListType edge_output; // components like latches that operate on rising edge
    wire(ListType output, ListType edge_out) 
        : output(std::move(output)), edge_output(std::move(edge_out)) {}
    ucount process(std::vector<component>&);
};