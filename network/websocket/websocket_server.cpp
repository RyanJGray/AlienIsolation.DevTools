#include "websocket_server.h"

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <iostream>

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;
using websocketpp::lib::error_code;

WebSocketServer::WebSocketServer()
{
    try {
        // Set logging settings
        this->server.set_access_channels(websocketpp::log::alevel::all);
        this->server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        // Initialize Asio
        this->server.init_asio();

        // Register our message handler
        this->server.set_message_handler(bind(&this->on_message, &this->server, ::_1, ::_2));

        // Listen on port 9002
        this->server.listen(9002);

        // Start the server accept loop
        this->server.start_accept(&this->on_end_accept);

        // Start the ASIO io_service run loop
        this->server.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "other exception" << std::endl;
    }
}

WebSocketServer::~WebSocketServer()
{
    delete this->server;
}

void WebSocketServer::on_message(server* s, websocketpp::connection_hdl hdl, server::message_ptr msg)
{
    std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload()
            << std::endl;

    // Check for a special command to instruct the server to stop listening so
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
void WebSocketServer::on_end_accept(error_code lib_ec, error_code trans_ec)
{
    std::cout << "Accept loop ended "
                << lib_ec.message() << "/" << trans_ec.message() << std::endl;
}
