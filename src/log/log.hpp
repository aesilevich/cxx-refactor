// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file log.hpp
/// Main include file for logging utilities.

#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>


using logger_t = boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>;

/// Returns reference to logger
inline logger_t & refactor_get_logger() {
    static logger_t logger;
    return logger;
}

#define REFACTOR_LOG(cat, level) \
    BOOST_LOG_SEV(refactor_get_logger(), level) \
        << ::boost::log::add_value("Category", #cat)

#define REFACTOR_LOG_TRACE(cat)         REFACTOR_LOG(cat, ::boost::log::trivial::trace)
#define REFACTOR_LOG_DEBUG(cat)         REFACTOR_LOG(cat, ::boost::log::trivial::debug)
#define REFACTOR_LOG_INFO(cat)          REFACTOR_LOG(cat, ::boost::log::trivial::info)
#define REFACTOR_LOG_WARNING(cat)       REFACTOR_LOG(cat, ::boost::log::trivial::warning)
#define REFACTOR_LOG_ERROR(cat)         REFACTOR_LOG(cat, ::boost::log::trivial::error)
#define REFACTOR_LOG_FATAL(cat)         REFACTOR_LOG(cat, ::boost::log::trivial::fatal)


#define REFACTOR_LOG_SCAT(cat, scat, level) \
    BOOST_LOG_SEV(refactor_get_logger(), level) \
        << ::boost::log::add_value("Category", #cat) \
        << ::boost::log::add_value("Subcategory", #scat)

#define REFACTOR_LOG_SCAT_TRACE(cat, scat)      REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::trace)
#define REFACTOR_LOG_SCAT_DEBUG(cat, scat)      REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::debug)
#define REFACTOR_LOG_SCAT_INFO(cat, scat)       REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::info)
#define REFACTOR_LOG_SCAT_WARNING(cat, scat)    REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::warning)
#define REFACTOR_LOG_SCAT_ERROR(cat, scat)      REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::error)
#define REFACTOR_LOG_SCAT_FATAL(cat, scat)      REFACTOR_LOG_SCAT(cat, scat, ::boost::log::trivial::fatal)
