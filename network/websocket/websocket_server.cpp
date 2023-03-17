#include "websocket_server.h"

#include <algorithm>
#include <iostream>
#include <uwebsockets/App.h>
#include "../../Menu_Log.hpp"
#include "../../GAME_LEVEL_MANAGER.h"

struct PerSocketData {

};

const std::string_view& handleMessage(std::string_view* message) {
    std::string_view retValue = "command_unsuccessful";

    if (message) {
        if (message->compare("load_level") == 0) {
            const int level = GAME_LEVEL_MANAGER::get_level_from_name(GAME_LEVEL_MANAGER::m_instance, const_cast<char*>("Production\\TECH_RND_HZDLAB"));
            if (level != 0) {
                // Add the level we want to the level queue.
                GAME_LEVEL_MANAGER::queue_level(GAME_LEVEL_MANAGER::m_instance, level);

                // Request that the game loads the next level in the queue.
                GAME_LEVEL_MANAGER::request_next_level(GAME_LEVEL_MANAGER::m_instance, false);

                retValue = "command_successful";
            }
        }
    }

    return retValue;
}

void relaySocket() {
    new std::thread([]() {
        uWS::App().get("/*", [](auto* res, auto* /*req*/) {
            res->end("<!DOCTYPE html><html><h1>Alien: Isolation DevTools</h1><h3>APOLLO Core Interface Ready - Open a WebSocket connection on this port, to communicate.</h3></html>");
        }).ws<PerSocketData>("/commands_editor", {
            .upgrade = nullptr,
            .open = [](auto* ws) {
                logger.AddLog("[Network::WebSocket] New WS connection opened.\n");
                ws->send("server_hello", uWS::OpCode::BINARY, true);
            },
            .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
                logger.AddLog("[Network::WebSocket] Received message.\n");
                ws->send("command_received", opCode, true);
                ws->send(handleMessage(&message), opCode, true);
            },
            .drain = [](auto*/*ws*/) {
                /* Check ws->getBufferedAmount() here */
            },
            .ping = [](auto*/*ws*/, std::string_view) {
                /* Not implemented yet */
            },
            .pong = [](auto*/*ws*/, std::string_view) {
                /* Not implemented yet */
            },
            .close = [](auto* ws, int /*code*/, std::string_view /*message*/) {
                logger.AddLog("[Network::WebSocket] WS connection closed.\n");
                ws->send("server_goodbye", uWS::OpCode::BINARY, true);
                /* You may access ws->getUserData() here */
            }
            }).listen(1702, [](auto* listen_socket) {
                if (listen_socket) {
                    logger.AddLog("[Network::WebSocket] System ready, listening on port 1702.\n");
                }
            }).run();
    });
}

WebSocketServer::WebSocketServer()
{
    relaySocket();
}

WebSocketServer::~WebSocketServer()
{
}

/*
void WebSocketServer::on_message(websocketpp_server* s, websocketpp::connection_hdl hdl, websocketpp_server::message_ptr msg)
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
}*/
