/* 
 * File:   Sessions.hpp
 * Author: vaulter
 *
 * Created on 13 Сентябрь 2010 г., 23:25
 */

#ifndef SESSIONS_HPP
#define	SESSIONS_HPP


/**
 * When new network user comes - server create session, game session.
 * Each session have ticket. Even if user disconnects, it can aafter while comes with this ticket
 * and if ticket still valid - proceed with old session
 */
struct Session {
    string      ticket;
    ClientPtr   client;
};

struct Sessions {

    
};
#endif	/* SESSIONS_HPP */

