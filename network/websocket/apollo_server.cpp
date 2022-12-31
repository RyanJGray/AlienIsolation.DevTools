#include "apollo_server.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::error_code;

// Pull out the type of messages sent by our config
typedef server::message_ptr message_ptr;

void APOLLOServer::on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload()
            << std::endl;

    // check for a special command to instruct the server to stop listening so
    // it can be cleanly exited.
    if (msg->get_payload() == "stop-listening") {
        s->stop_listening();
        return;
    }

    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (websocketpp::exception const & e) {
        std::cout << "Echo failed because: "
                << "(" << e.what() << ")" << std::endl;
    }
}

// Define a callback to handle failures accepting connections
void APOLLOServer::on_end_accept(error_code lib_ec, error_code trans_ec) {
    std::cout << "Accept loop ended "
                << lib_ec.message() << "/" << trans_ec.message() << std::endl;
}

bool APOLLOServer::initialise() {
    try {
        // Set logging settings
        this->apollo_server.set_access_channels(websocketpp::log::alevel::all);
        this->apollo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        this->apollo_server.init_asio();

        // Register our message handler
        this->apollo_server.set_message_handler(bind(&this->on_message, &this->apollo_server, ::_1, ::_2));

        // Listen on port 9002
        this->apollo_server.listen(9002);

        // Start the server accept loop
        this->apollo_server.start_accept(&this->on_end_accept);

        // Start the ASIO io_service run loop
        this->apollo_server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}
