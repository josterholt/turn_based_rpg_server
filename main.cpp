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
#include <libwebsockets.h>
#include <string.h>
#include <signal.h>
#include "src/game_server_protocol.cpp";
#include <vector>


struct lws_context *context;

static struct lws_protocols protocols[] = {
	LWS_PLUGIN_PROTOCOL_GAME_SERVER,
{ NULL, NULL, 0, 0 } /* terminator */
};

static int interrupted, port = 7681, options;

/* pass pointers to shared vars to the protocol */

static const struct lws_protocol_vhost_options pvo_options = {
	NULL,
	NULL,
	"options",		/* pvo name */
	(const char *)&options	/* pvo value */
};

static const struct lws_protocol_vhost_options pvo_interrupted = {
	&pvo_options,
	NULL,
	"interrupted",		/* pvo name */
	(const char *)&interrupted	/* pvo value */
};

static const struct lws_protocol_vhost_options pvo = {
	NULL,				/* "next" pvo linked-list */
	&pvo_interrupted,		/* "child" pvo linked-list */
	"lws-game-server",	/* protocol name we belong to on this vhost */
	""				/* ignored */
};
static const struct lws_extension extensions[] = {
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate"
		"; client_no_context_takeover"
	"; client_max_window_bits"
	},
{ NULL, NULL, NULL /* terminator */ }
};

void sigint_handler(int sig)
{
	interrupted = 1;
}

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
			lws_callback_on_writable_all_protocol(context, protocols);
			
			start_time = current_time;
		}
	}

	std::cout << "Exiting update loop\n";
}

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;



extern "C"
{
#include "lua535/include/lua.h";
#include "lua535/include/lauxlib.h";
#include "lua535/include/lualib.h";
}

#define SOL_ALL_SAFETIES_ON  1
#include <sol/sol.hpp>

#ifdef _WIN32
#pragma comment(lib, "lua535/lua53.dll")
#endif

bool CheckLua(lua_State *L, int r)
{
	if (r != LUA_OK)
	{
		std::string errormsg = lua_tostring(L, -1);
		std::cout << errormsg << std::endl;
		return false;
	}
	return true;
}

std::string lua_get_string(lua_State *l_state, std::string key) {
	std::string return_value;

	lua_pushstring(l_state, key.c_str());
	lua_gettable(l_state, -2);
	return_value = lua_tostring(l_state, -1);
	lua_pop(l_state, 1);
	return return_value;

}

void l_pushtablestring(lua_State* L, char* key, char* value) {
	lua_pushstring(L, key);
	lua_pushstring(L, value);
	lua_settable(L, -3);
}

struct Cat {
	std::string color;

	Cat(std::string c) {
		color = c;
	}
};


int main(int argc, const char **argv)
{

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::package);
	lua.script("print('bark bark bark!')");
	std::cout << std::endl;
	
	std::vector<Cat> cats;
	cats.push_back(Cat{ "Orange" });
	
	//sol::usertype<std::vector<Cat>> = lua.new_usertype<std::vector<Cat>>("std::vector<Cat>", std::vector<Cat>);
	
	lua.new_usertype<Cat>("Cat", "color", &Cat::color);
	lua.set("cats", &cats);


	lua.script("print('testing')");
	lua.script("print(cats[1].color)");

	std::string cmd = "a = 7 + 11";

	lua_State *L = luaL_newstate();
	luaL_openlibs(L);

	lua_newtable(L);
	l_pushtablestring(L, "fname", "john");
	lua_setglobal(L, "test_table");


	lua_getglobal(L, "test_table");
	if (lua_istable(L, -1)) {
		std::cout << lua_get_string(L, "fname") << "\n";
	}

	std::string test_str;
	if(CheckLua(L, luaL_dofile(L, "test.lua")))
	{
		lua_getglobal(L, "player");
		if (lua_istable(L, -1))
		{
			//std::cout << typeid(test_str).name() << "\n";
			//std::cout << lua_get_string(L, "title") << "\n";

		}
	}

	/*
	if (r == LUA_OK)
	{
		lua_getglobal(L, "a");
		if (lua_isnumber(L, -1))
		{
			float a_in_cpp = (float)lua_tonumber(L, -1);
			std::cout << a_in_cpp << std::endl;
		}
	}
	else
	{
		std::string error_msg = lua_tostring(L, -1);
		std::cout << error_msg << std::endl;
	}
	*/

	// Initialize logging
	logging::add_file_log(
		keywords::file_name = "server_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024, // 10MiB
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%",
		keywords::auto_flush = true
	);
	
	/*
	logging::core::get()->set_filter(
		logging::trivial::severity >= logging::trivial::info
	);
	*/
	logging::add_common_attributes();

	using namespace logging::trivial;
	src::severity_logger<severity_level> lg;
	BOOST_LOG_SEV(lg, trace) << "A trace severity message";

	// Setup server
	bool update_loop_active = true; // @todo does this need to be atomic?
	std::thread update_loop(updateGameStates, update_loop_active);
	update_loop.detach();
	srand(time(NULL));

	struct lws_context_creation_info info;
	const char *p;
	int n = 0, logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE
		/* for LLL_ verbosity above NOTICE to be built into lws,
		* lws must have been configured and built with
		* -DCMAKE_BUILD_TYPE=DEBUG instead of =RELEASE */
		/* | LLL_INFO */ /* | LLL_PARSER */ /* | LLL_HEADER */
											/* | LLL_EXT */ /* | LLL_CLIENT */ /* | LLL_LATENCY */
		/* | LLL_DEBUG */;

	signal(SIGINT, sigint_handler);

	if ((p = lws_cmdline_option(argc, argv, "-d")))
		logs = atoi(p);

	lws_set_log_level(logs, NULL);
	lwsl_user("LWS minimal ws client echo + permessage-deflate + multifragment bulk message\n");
	lwsl_user("   lws-minimal-ws-client-echo [-n (no exts)] [-p port] [-o (once)]\n");


	if ((p = lws_cmdline_option(argc, argv, "-p")))
		port = atoi(p);

	if (lws_cmdline_option(argc, argv, "-o"))
		options |= 1;

	memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = port;
	info.protocols = protocols;
	info.vhost_name = "localhost";

	//info.ssl_cert_filepath = "C:\\Users\\shrod\\OneDrive\\Documents\\ssl_certs\\gameserver.crt";
	//info.ssl_private_key_filepath = "C:\\Users\\shrod\\OneDrive\\Documents\\ssl_certs\\gameserver.key";
	info.ssl_cert_filepath = "C:\\Users\\Justin\\Documents\\ssl_certs\\gameserver.crt";
	info.ssl_private_key_filepath = "C:\\Users\\Justin\\Documents\\ssl_certs\\gameserver.key";


	info.pvo = &pvo;
	if (!lws_cmdline_option(argc, argv, "-n"))
		info.extensions = extensions;
	info.pt_serv_buf_size = 32 * 1024;
	info.options = LWS_SERVER_OPTION_VALIDATE_UTF8 | LWS_SERVER_OPTION_DISABLE_IPV6;

	context = lws_create_context(&info);
	if (!context) {
		lwsl_err("lws init failed\n");
		return 1;
	}

	while (n >= 0 && !interrupted)
		n = lws_service(context, 1000);

	lws_context_destroy(context);

	lwsl_user("Completed %s\n", interrupted == 2 ? "OK" : "failed");

	return interrupted != 2;
}

