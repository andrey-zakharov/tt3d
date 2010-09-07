/* 
 * File:   Logger.hpp
 * Author: vaulter
 *
 * Created on 5 РЎРµРЅС‚СЏР±СЂСЊ 2010 Рі., 23:41
 */

#ifndef LOGGER_HPP
#define	LOGGER_HPP
#include <iostream>

/// layer for another logger facility (now glog)
#include <glog/logging.h>


class Logger {
public:
    Logger( const char* name ) {
        google::InitGoogleLogging( name );
    }
};


/**
 GLOG_logtostderr=1 GLOG_v=2 ./test
 */
/// #define LOG( level ) std::clog
/// #define VLOG( level ) std::clog


#endif	/* LOGGER_HPP */

