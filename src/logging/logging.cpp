#include "logging.h"

void init_logging() {
	logging::add_file_log(
		keywords::file_name = "server_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024, // 10Mib
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%",
		keywords::auto_flush = true
	);

	logging::add_console_log(std::cout,keywords::format = ">> [%TimeStamp%]: %Message%");

	logging::add_common_attributes();
}