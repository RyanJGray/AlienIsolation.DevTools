#pragma once

#include <thread>

/*namespace Network::WebSocket {
    namespace Commands {
        struct command {
            std::string name;
            std::vector<std::string> parameters;
        };
        //NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(command, name, parameters)
    };

    namespace Common {
        struct response {
            bool error;
        };
        //NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(response, error)
    }
};*/

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();
    // Define a callback to handle incoming messages.
    //void on_message(websocketpp_server* s, websocketpp::connection_hdl hdl, websocketpp_server::message_ptr msg);
    // Define a callback to handle failures accepting connections.
    //void on_end_accept(error_code lib_ec, error_code trans_ec);
//private:
    //std::thread* server_thread;
};
