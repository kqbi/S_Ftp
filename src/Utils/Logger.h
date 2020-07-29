//
// Created by kqbi on 2020/6/1.
//

#ifndef IDAS_LOGGER_H
#define IDAS_LOGGER_H

#include <stdexcept>
#include <string>
#include <iostream>
#include <fstream>
#include <boost/log/common.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/attributes/named_scope.hpp>

//namespace iDAS {
    class Logger {
    public:
        typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;
        enum LoggerType {
            console = 0,
            file,
        };


        Logger() {}

        ~Logger() {}

        static Logger &Instance();

        bool Init(std::string fileName, int type, int level, int maxFileSize, int maxBackupIndex);

        boost::log::sources::severity_logger<boost::log::trivial::severity_level> _logger;
    };

#define S_LOG_TRACE(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::trace) << logEvent;

#define S_LOG_DEBUG(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::debug) << logEvent;

#define S_LOG_INFO(logEvent)   BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::info) << logEvent;

#define S_LOG_WARN(logEvent)   BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::warning) << logEvent;

#define S_LOG_ERROR(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::error) << logEvent;

#define S_LOG_FATAL(logEvent)  BOOST_LOG_FUNCTION(); BOOST_LOG_SEV(Logger::Instance()._logger, boost::log::trivial::fatal) << logEvent;

//}

#endif //IDAS_LOGGER_H
