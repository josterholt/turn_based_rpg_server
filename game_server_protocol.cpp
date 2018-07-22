#include <libwebsockets.h>
#include <string.h>
#include <chrono>
#include <sstream>

#include "gameclient.h"
#include "gamemanager.h"
#include "gameplayer.h"
#include <vector>
#include "boost/lockfree/queue.hpp"

#include <random>
#include <time.h>
//#include <stdlib.h>

#include <thread>

#include "src\protobuf\connect.pb.h"
#include "src\protobuf\playerUpdate.pb.h"

#define MAX_ECHO_PAYLOAD 1024

#define RING_DEPTH 4096
struct msg {
	void *payload; /* is malloc'd */
	size_t len;
	char binary;
	char first;
	char final;
};

#define LWS_PLUGIN_PROTOCOL_GAME_SERVER \
	{ \
		"lws-game-server", \
		callback_game_server, \
		sizeof(struct per_session_data__game_server), \
		1024, \
		0, NULL, 0 \
	}

struct per_session_data__game_server {
	struct lws_ring *ring;
	uint32_t msglen;
	uint32_t tail;
	uint8_t completed : 1;
	uint8_t flow_controlled : 1;
	int type;

	unsigned int index;
	int final;
	int continuation;

	GameClient* client = nullptr;
	char message[MAX_ECHO_PAYLOAD];
	int message_len;
	size_t remaining;
};

struct vhd_minimal_server_echo {
	struct lws_context *context;
	struct lws_vhost *vhost;

	int *interrupted;
	int *options;
};

static void
__minimal_destroy_message(void *_msg)
{
	struct msg *msg = reinterpret_cast<struct msg*>(_msg);

	free(msg->payload);
	msg->payload = NULL;
	msg->len = 0;
}
#include <assert.h>


static int
callback_game_server(struct lws *wsi, enum lws_callback_reasons reason, void *user,
	void *in, size_t len)
{
	struct per_session_data__game_server *pss =
		(struct per_session_data__game_server *)user;
	struct vhd_minimal_server_echo *vhd = (struct vhd_minimal_server_echo *)
		lws_protocol_vh_priv_get(lws_get_vhost(wsi),
			lws_get_protocol(wsi));
	const struct msg *pmsg;
	struct msg amsg;
	int n, m, flags;

	std::string message; // Custom
	char tmp_message[MAX_ECHO_PAYLOAD]; // Custom

	std::chrono::steady_clock::time_point start_frame_time = std::chrono::steady_clock::now();

	switch (reason) {
	case LWS_CALLBACK_SERVER_WRITEABLE:
		lwsl_user("LWS_CALLBACK_SERVER_WRITEABLE\n");

		if (pss->client != nullptr) {
			/**
			* Update heartbeat - Send position information to client every x milliseconds
			*/
			int n = 0;
			std::chrono::steady_clock::time_point current_frame_time = std::chrono::steady_clock::now();
			long duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame_time - start_frame_time).count();

			if (duration > 300) {
				start_frame_time = current_frame_time;

				//gamemessages::PositionUpdate message = pss->client->generatePositionUpdate();
				gamemessages::TestMessage message;
				message.set_test(100);

				std::string *message_str = new std::string();
				message.SerializeToString(message_str);

				gamemessages::TestMessage message2;
				message2.ParseFromString(*message_str);
				std::cout << "Affirm " << message2.test() << "\n";

				//std::string message_str = message.SerializePartialAsString();
				std::cout << "Writing to browser " << message_str->c_str() << " size: " << message_str->size() << "\n";

				n = lws_write(wsi, (unsigned char*)message_str->c_str(), message_str->size(), LWS_WRITE_BINARY);
				delete message_str;

				//n = lws_write(wsi, (unsigned char*)json_string.c_str(), json_string.size(), (lws_write_protocol)n);
				if (n < 0) {
					lwsl_err("ERROR %d writing to socket, hanging up\n", n);
					return 1;
				}
			}
			else {
				/**
				* This is a new client, run initialization
				* @todo Clean up routineQueue if it is unused
				*/
				int routine;

				try {
					if (pss->client->routineQueue.pop(routine)) {
						if (routine == GameClient::routines::INIT) {
							std::string json_string = pss->client->generateInit();
							std::cout << json_string << "\n";
							if (json_string.compare("") != 0) {
								n = lws_write(wsi, (unsigned char*)json_string.c_str(), json_string.size(), (lws_write_protocol)n);
								if (n < 0) {
									lwsl_err("ERROR %d writing to socket, hanging up\n", n);
									return 1;
								}
							}

						}
					}
				}
				catch (const std::exception& e) {
					std::cout << e.what();
				}
				catch (...) {
					// Don't want the server to crash due to a ill-formed request
					std::cout << "An error occurred while process request\n";
					std::cout << message << "\n";
				}
			}
		}

		lws_rx_flow_control(wsi, 1);
		lws_callback_on_writable(wsi);
		break;

	case LWS_CALLBACK_RECEIVE:
		pss->final = lws_is_final_fragment(wsi);

		std::cout << "Echo received\n";

		memcpy(&tmp_message, in, len);
		//std::cout << "Data: " << tmp_message << "\n";
		memset(&tmp_message, '\0', sizeof(char)*(MAX_ECHO_PAYLOAD));

		//std::cout << "Remaining: " << pss->remaining << "\n";

		//std::cout << "Pre-Append state: " << pss->message << "\n";
		if (!pss->remaining) {
			memcpy(&pss->message[pss->message_len], in, len);
			pss->message_len += len;
		}
		else {
			memcpy(&pss->message[pss->message_len], in, pss->remaining);
			pss->message_len += pss->remaining;
			pss->remaining = 0;
		}
		//std::cout << "Post-Append state: " << pss->message << "\n";
		//std::cout << "Message Len " << pss->message_len << " / " << MAX_ECHO_PAYLOAD << "\n";

		pss->remaining = lws_remaining_packet_payload(wsi);

		if (!pss->remaining && pss->final) {
			/*
			* Process payload when it has all been received
			* Clear out payload buffer for next payload
			*/
			//std::cout << "Final state: " << pss->message << "\n";
			if (pss->type == 0) {
				std::cout << "Connecting...\n";
				gamemessages::Connect connect;
				connect.ParseFromString(pss->message);
				// Right now we just add them to the same game pool. There is no join game
				std::cout << "Processing connect request\n";
				pss->client = new GameClient();
			}
			else if (pss->type == 1) {
				std::cout << "Processing position\n";
				gamemessages::PlayerUpdate player_update;
				player_update.ParseFromString(pss->message);
				//std::cout << player_update.player().position_x() << ", " << player_update.player().position_x() << "\n";
				std::cout << "Done\n";
			}
			/*
			else {
			std::cout << "Running standard process request (something went wrong)\n";
			pss->client->processRequest(pss->message, pss->message_len);
			}
			*/
			memset(&pss->message, '\0', sizeof(char)*(MAX_ECHO_PAYLOAD));
			pss->message_len = 0;

		}

		//lws_rx_flow_control(wsi, 1);
		break;
	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		/* reject everything else except permessage-deflate */
		if (strcmp((const char*)in, "permessage-deflate"))
			return 1;
		break;
	case LWS_CALLBACK_ESTABLISHED:
		lws_set_extension_option(wsi, "permessage-deflate",
			"rx_buf_size", "12");

		char buf[32];
		if (lws_hdr_copy(wsi, buf, sizeof(buf), WSI_TOKEN_GET_URI) > 0) {
			if (!strcmp(buf, "/connect")) {
				std::cout << "connect\n";
				pss->type = 0;
			}
			else if (!strcmp(buf, "/playerUpdate")) {
				std::cout << "playerUpdate\n";
				pss->type = 1;
			}
			else {
				std::cout << "Not special URL\n";
			}
		}
		else {
			std::cout << "No URI\n";
		}

		break;
	case LWS_CALLBACK_WSI_DESTROY:
		/*
		if (pss != nullptr) {
			if (pss->client != nullptr) {
				pss->client->leaveGame();
				delete pss->client;
			}
		}
		*/
		break;
	default:
		break;
	}

	return 0;
}