#define _CRT_SECURE_NO_WARNINGS

#define DEFAULT_BUFLEN 255

#include <vector>
#include <winsock2.h>

#include <iostream>
#include <array>
#include <ctime>
#include "service.h"

using namespace std;
extern vector<Counter> vecOfCounters;
int NumOfUsers = 0;

//----------------------------------------------------------------------------------------------------------------------------------------
// A function that guarantees reading specified number of bytes
int readn( long socket, char *recvbuf, int recvbuf_len ){
    int iResult = 0;
    memset( recvbuf, 0, recvbuf_len );                              // set recvbuf[i] = 0
    int numOfRecivedBytes = 0;                                      // variable to count received bytes

    while( numOfRecivedBytes < recvbuf_len ) {                      // while number of read bytes is less than specified number
                                                                    // receive data from a connected socket
        iResult = recv( socket, recvbuf + numOfRecivedBytes, recvbuf_len - numOfRecivedBytes, 0 );
        if( iResult == INVALID_SOCKET || iResult == SOCKET_ERROR )  // if error of receiving - return -1
            return -1;
        numOfRecivedBytes += iResult;                               // add the number of reading bytes
    }
    return numOfRecivedBytes;                                       // success - return number of receiving bytes
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
int getInfo( long tmpSocket, char *txt ) {
    char buffer[DEFAULT_BUFLEN];

    int iResult = send( tmpSocket, txt, DEFAULT_BUFLEN, 0 );         // sending message to client
    //iResult = sendLine( tmpSocket, txt );         // sending message to client
    if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
        cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
        return -1;
    }
    memset( buffer, 0, DEFAULT_BUFLEN );

    iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
    //iResult = recvLine( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
    if( (iResult <= 0) ) {                                      // if no bytes - error - client disconnect - exit from while
        cout << "Client " << tmpSocket << " disconnect" << endl;
        return -1;
    }
    return atoi( buffer );
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
int GetID( long tmpSocket ) {
    char buffer[DEFAULT_BUFLEN];
    char an[DEFAULT_BUFLEN];
    memset( an, 0, DEFAULT_BUFLEN );
    memmove( an, "id:", 3 );
    int iResult = send( (SOCKET)tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
    //iResult = sendLine( tmpSocket, an );         // sending message to client
    if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
        cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
        return -1;
    }
    memset( buffer, 0, DEFAULT_BUFLEN );

    iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
    //iResult = recvLine( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
    if( iResult <= 0 )                                      // if no bytes - error - client disconnect - exit from while
        return -1;

    return atoi(buffer);
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
int addCounter( int id, int un ) {
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->companyID == id && tmp->uniqNumber == un ){
            cout << "Already have such counter" << endl;
            return 0;
        }
    }
    Counter newCounter;
    newCounter.companyID = id;
    newCounter.uniqNumber = un;
    newCounter.userID = 0;
    vecOfCounters.push_back( newCounter );
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
int setCounterToUser( int id ) {
    int i = 1;
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->userID == 0 )
            break;
        i++;
    }
    if( tmp != vecOfCounters.cend() ) {
        vecOfCounters.at( i ).setUser( id );
    } else {
        cout << "No free counters" << endl;
        return 0;
    }
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
int setCounterReadings( int id, int un, struct tm newTime, int newReadings ){
    int i = 1;
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->userID == id && tmp->uniqNumber == un )
            break;
        i++;
    }
    if( tmp != vecOfCounters.cend() ) {
        CounterData newCounterData;
        newCounterData.setData( newTime, newReadings );
        vecOfCounters.at( i ).data.push_back( newCounterData );
    } else {
        cout << "No such counter" << endl;
        return 0;
    }
    return 1;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
void ShowUserData( int id, char *buffer ) {
    int i = 1;
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->userID == id )
            vecOfCounters.at(i).showData( buffer );
        i++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
void ShowCompanyData( int id, char *buffer ) {
    int i = 1;
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->companyID == id )
            vecOfCounters.at(i).showData( buffer );
        i++;
    }
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
void ShowBadUsers( int id, int day, int month, int year, char *buffer ) {
    int i = 1;
    auto tmp = vecOfCounters.cbegin();
    for( ; tmp !=  vecOfCounters.cend(); tmp++ ) {
        if( tmp->companyID == id ){
            int i2 = 1;
            auto tmp2 = vecOfCounters.at( i ).data.cbegin();
            for( ; tmp2 !=  vecOfCounters.at( i ).data.cend(); tmp++ ) {
                if( vecOfCounters.at( i ).data.at( i2 ).CompareDate( day, month, year ) == 1 )
                    break;
                i2++;
            }
            if( tmp2 != vecOfCounters.at( i ).data.cend() ) {
                vecOfCounters.at(i).showData( buffer );
            }
        }
        i++;
    }
}
