#ifndef _LOGGING_H_
#define _LOGGING_H_

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
using namespace logging::trivial;

void init_logging() {
	logging::add_file_log(
		keywords::file_name = "server_%N.log",
		keywords::rotation_size = 10 * 1024 * 1024, // 10Mib
		keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		keywords::format = "[%TimeStamp%]: %Message%",
		keywords::auto_flush = true
	);

	logging::add_common_attributes();
}
#endif