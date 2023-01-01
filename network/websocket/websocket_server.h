#pragma once

typedef websocketpp::server<websocketpp::config::asio> websocketpp_server;

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();
    // Define a callback to handle incoming messages.
    void on_message(websocketpp_server* s, websocketpp::connection_hdl hdl, message_ptr msg);
    // Define a callback to handle failures accepting connections.
    void on_end_accept(error_code lib_ec, error_code trans_ec);
private:
    websocketpp_server server;
}
