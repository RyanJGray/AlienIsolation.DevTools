#pragma once

typedef websocketpp::server<websocketpp::config::asio> server;

class APOLLOServer {
public:
    // Define a callback to handle incoming messages.
    static void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg);
    // Define a callback to handle failures accepting connections.
    static void on_end_accept(error_code lib_ec, error_code trans_ec);
    // Initialise the websocket server.
    static bool initialise();
private:
    static server apollo_server;
}
