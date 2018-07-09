/*
 * libwebsockets-test-server - libwebsockets test implementation
 *
 * Copyright (C) 2010-2016 Andy Green <andy@warmcat.com>
 *
 * This file is made available under the Creative Commons CC0 1.0
 * Universal Public Domain Dedication.
 *
 * The person who associated a work with this deed has dedicated
 * the work to the public domain by waiving all of his or her rights
 * to the work worldwide under copyright law, including all related
 * and neighboring rights, to the extent allowed by law. You can copy,
 * modify, distribute and perform the work, even for commercial purposes,
 * all without asking permission.
 *
 * The test apps are intended to be adapted for use in your code, which
 * may be proprietary.	So unlike the library itself, they are licensed
 * Public Domain.
 */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>
#include <signal.h>
#include "server.h"
#include "../lib/libwebsockets.h"
#ifndef _WIN32
#include <syslog.h>
#include <sys/time.h>
#include <unistd.h>
#else
#include <process.h>
#endif
//#include <uv.h>
#include <chrono>
#include <sstream>

#include "gameclient.h"
#include "gamemanager.h"
#include "gameplayer.h"
#include <vector>
#include "boost/lockfree/queue.hpp"

#include <random>
#include <time.h>
#include <stdlib.h>

#include <thread>



//GameManager GAME_MANAGER;

#define LWS_INTERNAL
int close_testing;
int max_poll_elements;
int debug_level = 7;

#ifdef EXTERNAL_POLL
struct lws_pollfd *pollfds;
int *fd_lookup;
int count_pollfds;
#endif

static int versa, state;
static int times = -1;
#define MAX_ECHO_PAYLOAD 1024
//volatile int force_exit = 0;
struct lws_context *context;
struct lws_plat_file_ops fops_plat;
#define LWSWS_CONFIG_STRING_SIZE (32 * 1024)

/* http server gets files from this path */
#ifndef INSTALL_DATADIR
#define INSTALL_DATADIR "./"
#endif
#define LOCAL_RESOURCE_PATH INSTALL_DATADIR"/libwebsockets-test-server"
char *resource_path = LOCAL_RESOURCE_PATH;
#if defined(LWS_USE_POLARSSL)
#else
#if defined(LWS_USE_MBEDTLS)
#else
#if defined(LWS_OPENSSL_SUPPORT) && defined(LWS_HAVE_SSL_CTX_set1_param)
char crl_path[1024] = "";
#endif
#endif
#endif

std::chrono::steady_clock::time_point start_frame_time = std::chrono::steady_clock::now();

struct per_session_data__echo {
	//size_t rx, tx;
	//unsigned char buf[LWS_PRE + MAX_ECHO_PAYLOAD];
	//unsigned int len;
	unsigned int index;
	int final;
	int continuation;
	//int binary;
	GameClient* client;
	char message[MAX_ECHO_PAYLOAD];
	int message_len;
	size_t remaining;
};


static int
callback_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user,
	void *in, size_t len)
{
	struct per_session_data__echo *pss =
		(struct per_session_data__echo *)user;
	std::string message;
	int n;
	size_t remaining;
	char tmp_message[MAX_ECHO_PAYLOAD];

	switch (reason) {
	case LWS_CALLBACK_SERVER_WRITEABLE:
		do_tx:
		if (pss->client != nullptr) {
			/**
			 * Update heartbeat - Send position information to client every x milliseconds
			 */
			int n = 0;
			std::chrono::steady_clock::time_point current_frame_time = std::chrono::steady_clock::now();
			long duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_frame_time - start_frame_time).count();
		
			if (duration > 300) {
				start_frame_time = current_frame_time;

				std::string json_string = pss->client->generatePositionUpdate();
				std::cout << json_string << "\n";
				if (json_string.compare("") != 0) {
					n = lws_write(wsi, (unsigned char*)json_string.c_str(), json_string.size(), (lws_write_protocol)n);
					if (n < 0) {
						lwsl_err("ERROR %d writing to socket, hanging up\n", n);
						return 1;
					}
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

				pss->client->processRequest(pss->message, pss->message_len);
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
		pss->client = new GameClient();
		break;
	case LWS_CALLBACK_WSI_DESTROY:
		pss->client->leaveGame();
		delete pss->client;
		break;
	default:
		break;
	}

	return 0;
}


/* list of supported protocols and callbacks */

static struct lws_protocols protocols[] = {
        /* first protocol must always be HTTP handler */
        {
            "lws-echogen",
			callback_echo,
            sizeof(struct per_session_data__echo),
			128, /* rx buf size must be >= permessage-deflate rx size */
        },
        { NULL, NULL, 0, 0 } /* terminator */
};

void sighandler(int sig)
{
    force_exit = 1;
    lws_cancel_service(context);
}

static const struct lws_extension exts[] = {
        {
            "permessage-deflate",
            lws_extension_callback_pm_deflate,
			"permessage-deflate"
        },
        {
			"deflate-frame",
			lws_extension_callback_pm_deflate,
			"deflate_frame"
        },
        { NULL, NULL, NULL /* terminator */ }
};



static struct option options[] = {
        { "help",	no_argument,		NULL, 'h' },
        { "debug",	required_argument,	NULL, 'd' },
        { "port",	required_argument,	NULL, 'p' },
        { "ssl",	no_argument,		NULL, 's' },
        { "allow-non-ssl",	no_argument,	NULL, 'a' },
        { "interface",	required_argument,	NULL, 'i' },
        { "closetest",	no_argument,		NULL, 'c' },
        { "ssl-cert",  required_argument,	NULL, 'C' },
        { "ssl-key",  required_argument,	NULL, 'K' },
        { "ssl-ca",  required_argument,		NULL, 'A' },
#if defined(LWS_OPENSSL_SUPPORT)
{ "ssl-verify-client",	no_argument,		NULL, 'v' },
#if defined(LWS_HAVE_SSL_CTX_set1_param)
	{ "ssl-crl",  required_argument,		NULL, 'R' },
#endif
#endif
        { "libev",  no_argument,		NULL, 'e' },
#ifndef LWS_NO_DAEMONIZE
        { "daemonize",	no_argument,		NULL, 'D' },
#endif
        { "resource_path", required_argument,	NULL, 'r' },
        { NULL, 0, 0, 0 }
};

/*
void signal_cb(uv_signal_t *watcher, int signum)
{
    lwsl_err("Signal %d caught, exiting...\n", watcher->signum);
    switch (watcher->signum) {
        case SIGTERM:
        case SIGINT:
            break;
        default:
            signal(SIGABRT, SIG_DFL);
            abort();
            break;
    }
    lws_libuv_stop(context);
}
*/
/*
LWS_VISIBLE int
init_main(struct lws_context *context, struct lws_plugin_capability *c) {
	c->protocols = protocols;
	c->count_protocols = 3;
	c->extensions = NULL;
	c->count_extensions = 0;
	return 0;
}
*/

void updateGameStates(bool update_loop) {
	GameManager& manager = GameManager::getInstance();
	std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

	std::cout << "Starting update loop\n";
	while (update_loop) {
		std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();
		double time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count();

		if (time_elapsed > 300) {
			//std::cout << "Time Elapsed: " << time_elapsed << "\n";
			manager.update(time_elapsed);
			start_time = current_time;
		}
	}

	std::cout << "Exiting update loop\n";
}

int main(int argc, char **argv)
{
	bool update_loop_active = true; // @todo does this need to be atomic?
	std::thread update_loop(updateGameStates, update_loop_active);
	update_loop.detach();

	srand(time(NULL));
    struct lws_context_creation_info info;
    char interface_name[128] = "";
    unsigned int ms, oldms = 0;
    const char *iface = NULL;
    char cert_path[1024] = "";
    char key_path[1024] = "";
    char ca_path[1024] = "";
    int uid = -1, gid = -1;
    int use_ssl = 0;
    int opts = 0, cs_len = LWSWS_CONFIG_STRING_SIZE - 1;
    int n = 0;
#ifndef _WIN32
    /* LOG_PERROR is not POSIX standard, and may not be portable */
#ifdef __sun
	int syslog_options = LOG_PID;
#else
	int syslog_options = LOG_PID | LOG_PERROR;
#endif
#endif
#ifndef LWS_NO_DAEMONIZE
    int daemonize = 0;
#endif

    /*
     * take care to zero down the info struct, he contains random garbaage
     * from the stack otherwise
     */
    memset(&info, 0, sizeof info);
    info.port = 7681;

    while (n >= 0) {
        n = getopt_long(argc, argv, "eci:hsap:d:Dr:C:K:A:R:vu:g:", options, NULL);
        int f = 0;
        if (n < 0)
            f = WSAGetLastError();
            lwsl_err("Testing\n");
            continue;
        switch (n) {
            case 'e':
                opts |= LWS_SERVER_OPTION_LIBEV;
                break;
#ifndef LWS_NO_DAEMONIZE
            case 'D':
                daemonize = 1;
#if !defined(_WIN32) && !defined(__sun)
                syslog_options &= ~LOG_PERROR;
#endif
                break;
#endif
            case 'u':
                uid = atoi(optarg);
                break;
            case 'g':
                gid = atoi(optarg);
                break;
            case 'd':
                debug_level = atoi(optarg);
                break;
            case 's':
                use_ssl = 1;
                break;
            case 'a':
                opts |= LWS_SERVER_OPTION_ALLOW_NON_SSL_ON_SSL_PORT;
                break;
            case 'p':
                info.port = atoi(optarg);
                break;
            case 'i':
                strncpy(interface_name, optarg, sizeof interface_name);
                interface_name[(sizeof interface_name) - 1] = '\0';
                iface = interface_name;
                break;
            case 'c':
                close_testing = 1;
                fprintf(stderr, " Close testing mode -- closes on "
                        "client after 50 dumb increments"
                        "and suppresses lws_mirror spam\n");
                break;
            case 'r':
                resource_path = optarg;
                printf("Setting resource path to \"%s\"\n", resource_path);
                break;
            case 'C':
                strncpy(cert_path, optarg, sizeof(cert_path) - 1);
                cert_path[sizeof(cert_path) - 1] = '\0';
                break;
            case 'K':
                strncpy(key_path, optarg, sizeof(key_path) - 1);
                key_path[sizeof(key_path) - 1] = '\0';
                break;
            case 'A':
                strncpy(ca_path, optarg, sizeof(ca_path) - 1);
                ca_path[sizeof(ca_path) - 1] = '\0';
                break;
#if defined(LWS_OPENSSL_SUPPORT)
            case 'v':
			use_ssl = 1;
			opts |= LWS_SERVER_OPTION_REQUIRE_VALID_OPENSSL_CLIENT_CERT;
			break;
#if defined(LWS_USE_POLARSSL)
#else
#if defined(LWS_USE_MBEDTLS)
#else
#if defined(LWS_HAVE_SSL_CTX_set1_param)
		case 'R':
			strncpy(crl_path, optarg, sizeof(crl_path) - 1);
			crl_path[sizeof(crl_path) - 1] = '\0';
			break;
#endif
#endif
#endif
#endif
            case 'h':
                fprintf(stderr, "Usage: test-server "
                        "[--port=<p>] [--ssl] "
                        "[-d <log bitfield>] "
                        "[--resource_path <path>]\n");
                exit(1);
        }
    }

#if !defined(LWS_NO_DAEMONIZE) && !defined(WIN32)
    /*
	 * normally lock path would be /var/lock/lwsts or similar, to
	 * simplify getting started without having to take care about
	 * permissions or running as root, set to /tmp/.lwsts-lock
	 */
	if (daemonize && lws_daemonize("/tmp/.lwsts-lock")) {
		fprintf(stderr, "Failed to daemonize\n");
		return 10;
	}
#endif

    signal(SIGINT, sighandler);

#ifndef _WIN32
    /* we will only try to log things according to our debug_level */
	setlogmask(LOG_UPTO (LOG_DEBUG));
	openlog("lwsts", syslog_options, LOG_DAEMON);
#endif

    /* tell the library what debug level to emit and to send it to syslog */
    lws_set_log_level(debug_level, lwsl_emit_syslog);

    lwsl_notice("libwebsockets test server - license LGPL2.1+SLE\n");
    lwsl_notice("(C) Copyright 2010-2016 Andy Green <andy@warmcat.com>\n");

    printf("Using resource path \"%s\"\n", resource_path);
#ifdef EXTERNAL_POLL
    max_poll_elements = getdtablesize();
	pollfds = (lws_pollfd *) malloc(max_poll_elements * sizeof (struct lws_pollfd));
	fd_lookup = (int *) malloc(max_poll_elements * sizeof (int));
	if (pollfds == NULL || fd_lookup == NULL) {
		lwsl_err("Out of memory pollfds=%d\n", max_poll_elements);
		return -1;
	}
#endif

    info.iface = iface;
    info.protocols = protocols;
    info.ssl_cert_filepath = NULL;
    info.ssl_private_key_filepath = NULL;

    if (use_ssl) {
        if (strlen(resource_path) > sizeof(cert_path) - 32) {
            lwsl_err("resource path too long\n");
            return -1;
        }
        if (!cert_path[0])
            sprintf(cert_path, "%s/libwebsockets-test-server.pem",
                    resource_path);
        if (strlen(resource_path) > sizeof(key_path) - 32) {
            lwsl_err("resource path too long\n");
            return -1;
        }
        if (!key_path[0])
            sprintf(key_path, "%s/libwebsockets-test-server.key.pem",
                    resource_path);

        info.ssl_cert_filepath = cert_path;
        info.ssl_private_key_filepath = key_path;
        if (ca_path[0])
            info.ssl_ca_filepath = ca_path;
    }

    info.gid = gid;
    info.uid = uid;
    info.max_http_header_pool = 16;
    info.options = opts | LWS_SERVER_OPTION_VALIDATE_UTF8;
    info.extensions = exts;
    info.timeout_secs = 5;
    info.ssl_cipher_list = "ECDHE-ECDSA-AES256-GCM-SHA384:"
            "ECDHE-RSA-AES256-GCM-SHA384:"
            "DHE-RSA-AES256-GCM-SHA384:"
            "ECDHE-RSA-AES256-SHA384:"
            "HIGH:!aNULL:!eNULL:!EXPORT:"
            "!DES:!MD5:!PSK:!RC4:!HMAC_SHA1:"
            "!SHA1:!DHE-RSA-AES128-GCM-SHA256:"
            "!DHE-RSA-AES128-SHA256:"
            "!AES128-GCM-SHA256:"
            "!AES128-SHA256:"
            "!DHE-RSA-AES256-SHA256:"
            "!AES256-GCM-SHA384:"
            "!AES256-SHA256";
	
    if (use_ssl)
        /* redirect guys coming on http */
        info.options |= LWS_SERVER_OPTION_REDIRECT_HTTP_TO_HTTPS;

    context = lws_create_context(&info);
    if (context == NULL) {
        lwsl_err("libwebsocket init failed\n");
        return -1;
    }

	//lws_uv_sigint_cfg(context, 1, signal_cb);
	//lws_uv_initloop(context, NULL, 0);
	//lws_libuv_run(context, 0);
	
	n = 0;
	while (n >= 0 && !force_exit) {
		n = lws_service(context, 10);
	}

	lws_context_destroy(context);
	fprintf(stderr, "lwsws exited cleanly\n");

#ifndef _WIN32
    closelog();
#endif

	update_loop_active = false;
    return 0;
}
