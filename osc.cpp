#include "emscripten.h"
#include <emscripten/bind.h>
// #include <src/lerp.cpp>

#include <include/oscpp/client.hpp>

using namespace emscripten;

EMSCRIPTEN_KEEPALIVE

    // // Construct a packet
    // OSCPP::Client::Packet packet(buffer, size);
    // packet
    //     // Open a bundle with a timetag
    //     .openBundle(1234ULL)
    //         // Add a message with two arguments and an array with 6 elements;
    //         // for efficiency this needs to be known in advance.
    //         .openMessage("/s_new", 2 + OSCPP::Tags::array(6))
    //             // Write the arguments
    //             .string("sinesweep")
    //             .int32(2)
    //             .openArray()
    //                 .string("start-freq")
    //                 .float32(330.0f)
    //                 .string("end-freq")
    //                 .float32(990.0f)
    //                 .string("amp")
    //                 .float32(0.4f)
    //             .closeArray()
    //         // Every `open` needs a corresponding `close`
    //         .closeMessage()
    //         // Add another message with one argument
    //         .openMessage("/n_free", 1)
    //             .int32(1)
    //         .closeMessage()
    //         // And nother one
    //         .openMessage("/n_set", 3)
    //             .int32(1)
    //             .string("wobble")
    //             // Numeric arguments are converted automatically
    //             // (see below)
    //             .int32(31)
    //         .closeMessage()
    //     .closeBundle();
    // return packet.size();

EMSCRIPTEN_BINDINGS(my_module) {
    class_<Packet>("Packet")
        .constructor<int, int>()
        .function("openBundle", &Packet::openBundle)
        // .property("x", &MyClass::getX, &MyClass::setX)
        // .class_function("getStringFromInstance", &MyClass::getStringFromInstance)
        ;
}

// quick_example.cpp
// #include <emscripten/bind.h>

// float lerp(float a, float b, float t) {
//     return (1 - t) * a + t * b;
// }

// EMSCRIPTEN_BINDINGS(my_module) {
//     function("lerp", &lerp);
// }
