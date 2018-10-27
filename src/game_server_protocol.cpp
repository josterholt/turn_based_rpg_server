#include <libwebsockets.h>
#include <string.h>
#include <chrono>
#include <sstream>

#include "../gameclient.h"
#include "../gamemanager.h"
#include "../gameplayer.h"
#include <vector>
#include "boost/lockfree/queue.hpp"

#include <random>
#include <time.h>
//#include <stdlib.h>

#include <thread>

#include "protobuf\connect.pb.h"
#include "protobuf\playerUpdate.pb.h"

#define MAX_ECHO_PAYLOAD 1024

#define RING_DEPTH 4096
struct msg {
	unsigned char *payload; /* is malloc'd */
	size_t len;
	/*
	char binary;
	char first;	
	char final;
	*/
};

#define LWS_PLUGIN_PROTOCOL_GAME_SERVER \
	{ \
		"lws-game-server", \
		callback_game_server, \
		sizeof(struct per_session_data__minimal), \
		1024, \
		0, NULL, 0 \
	}

/* one of these is created for each client connecting to us */

struct per_session_data__minimal {
	struct per_session_data__minimal *pss_list;
	struct lws *wsi;
	int last; /* the last message number we sent */
	int type;
	GameClient* client;
};

/* one of these is created for each vhost our protocol is used with */

struct per_vhost_data__minimal {
	struct lws_context *context;
	struct lws_vhost *vhost;
	const struct lws_protocols *protocol;

	struct per_session_data__minimal *pss_list; /* linked-list of live pss*/

	struct msg amsg; /* the one pending message... */
	int current; /* the current message number we are caching */
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
	struct per_session_data__minimal *pss =
		(struct per_session_data__minimal *)user;
	struct per_vhost_data__minimal *vhd = (struct per_vhost_data__minimal *)
		lws_protocol_vh_priv_get(lws_get_vhost(wsi),
			lws_get_protocol(wsi));
	const struct msg *pmsg;
	struct msg amsg;
	int n, m, flags;

	std::string message; // Custom
	char tmp_message[MAX_ECHO_PAYLOAD]; // Custom

	std::chrono::steady_clock::time_point start_frame_time = std::chrono::steady_clock::now();

	switch (reason) {
	case LWS_CALLBACK_PROTOCOL_INIT:
		vhd = (per_vhost_data__minimal *) lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi), lws_get_protocol(wsi), sizeof(struct per_vhost_data__minimal));
		vhd->context = lws_get_context(wsi);
		vhd->protocol = lws_get_protocol(wsi);
		vhd->vhost = lws_get_vhost(wsi);

	case LWS_CALLBACK_CLOSED:
		lws_ll_fwd_remove(struct per_session_data__minimal, pss_list,
			pss, vhd->pss_list);
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:
		if (!vhd->amsg.payload)
			break;

		if (pss->last == vhd->current)
			break;

		/* notice we allowed for LWS_PRE in the payload already */
		if (pss->client != nullptr) {
			/**
			* Update heartbeat - Send position information to client every x milliseconds
			*/
			int n = 0;
			std::chrono::steady_clock::time_point current_frame_time = std::chrono::steady_clock::now();
			long duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame_time - start_frame_time).count();

			if (duration >= 0) {
				start_frame_time = current_frame_time;

				//gamemessages::PositionUpdate message = pss->client->generatePositionUpdate();
				gamemessages::TestMessage message;
				message.set_test(100);
				std::string message_str = message.SerializeAsString();
				std::cout << "Sending message " << message_str << "\n";

				//const unsigned char * tmp_str = (const unsigned char *) message_str.c_str();
				//std::cout << "Foo foo foo foo 12345\n";
				//char tmp_str[] = { 't', 'e', '\0'};
				//int tmp_str_length = strlen(tmp_str);

				//n = lws_write(wsi, (unsigned char *) tmp_str, tmp_str_length + 1, (lws_write_protocol)n);
				//delete message_str;
				//std::string json_string;
				std::stringstream json_ss;
				for (int i = 0; i < LWS_PRE; i++) {
					json_ss << " ";
				}
				json_ss << message_str;
				std::string json_string = json_ss.str();

				n = lws_write(wsi, (unsigned char*)json_string.c_str() + LWS_PRE, json_string.size() - LWS_PRE, (lws_write_protocol)n);
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

		/*
		m = lws_write(wsi, vhd->amsg.payload + LWS_PRE, vhd->amsg.len,
			LWS_WRITE_TEXT);
		if (m < (int)vhd->amsg.len) {
			lwsl_err("ERROR %d writing to ws\n", m);
			return -1;
		}
		*/

		pss->last = vhd->current;
		break;

	case LWS_CALLBACK_RECEIVE:
		if (vhd->amsg.payload)
			__minimal_destroy_message(&vhd->amsg);

		vhd->amsg.len = len;
		/* notice we over-allocate by LWS_PRE */
		vhd->amsg.payload = (unsigned char *) malloc(LWS_PRE + len);
		if (!vhd->amsg.payload) {
			lwsl_user("OOM: dropping\n");
			break;
		}

		memcpy((char *)vhd->amsg.payload + LWS_PRE, in, len);
		vhd->current++;

		if (pss->type == 0) {
			std::cout << "Connecting...\n";
			char *message = new char[len];
			memcpy(message, in, len);
			gamemessages::Connect connect;
			connect.ParseFromString(message);
			delete message;

			std::cout << "Game Token: " << connect.gametoken() << "\n";
			// Right now we just add them to the same game pool. There is no join game
			std::cout << "Processing connect request\n";
			pss->client = new GameClient();
		}
		else if (pss->type == 1) {
			std::cout << "Processing position\n";
			gamemessages::PlayerUpdate player_update;
			player_update.ParseFromString((char *) vhd->amsg.payload);
			//std::cout << player_update.player().position_x() << ", " << player_update.player().position_x() << "\n";
			std::cout << "Done\n";
		}

		/*
		* let everybody know we want to write something on them
		* as soon as they are ready
		*/
		lws_start_foreach_llp(struct per_session_data__minimal **,
			ppss, vhd->pss_list) {
			lws_callback_on_writable((*ppss)->wsi);
		} lws_end_foreach_llp(ppss, pss_list);


		//lws_rx_flow_control(wsi, 1);
		break;

	case LWS_CALLBACK_CLIENT_CONFIRM_EXTENSION_SUPPORTED:
		/* reject everything else except permessage-deflate */
		if (strcmp((const char*)in, "permessage-deflate"))
			return 1;
		break;

	case LWS_CALLBACK_ESTABLISHED:
		lws_ll_fwd_insert(pss, pss_list, vhd->pss_list);
		pss->wsi = wsi;
		pss->last = vhd->current;

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
		if (pss != nullptr) {
			if (pss->client != nullptr) {
				pss->client->leaveGame();
				delete pss->client;
			}
		}
		break;
	default:
		break;
	}

	return 0;
}