#define _CRT_SECURE_NO_WARNINGS

//#define N 512
#define DEFAULT_BUFLEN 512

#include <vector>
#include <winsock2.h>

#include <iostream>
#include <array>
#include <ctime>
#include "service.h"

using namespace std;
extern vector<Counter> vecOfCompanies;
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

int getInfo( long tmpSocket, char *txt ) {
    char buffer[DEFAULT_BUFLEN];

    int iResult;
    while( true ){
        iResult = send( tmpSocket, txt, DEFAULT_BUFLEN, 0 );         // sending message to client
        if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
            cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
            return -1;
        }
        memset( buffer, 0, DEFAULT_BUFLEN );

        iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        if( iResult > 0 )                                      // if no bytes - error - client disconnect - exit from while
            break;
    }

    return atoi( buffer );
}

int GetID( long tmpSocket ) {
    char buffer[DEFAULT_BUFLEN];
    char an[] = "Enter your id: ";
    int iResult;
    while( true ){
        iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
        if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
            cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
            return -1;
        }
        memset( buffer, 0, DEFAULT_BUFLEN );

        iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        if( iResult > 0 )                                      // if no bytes - error - client disconnect - exit from while
            break;
    }
    return iResult;
}

int addCounter( int id, int un ){
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->companyID == id && tmp->uniqNumber == un ){
            cout << "Already have such counter" << endl;
            return 0;
        }
    }
    Counter *newCounter;
    newCounter->companyID = id;
    newCounter->uniqNumber = un;
    newCounter->userID = 0;
    vecOfCompanies.push_back( *newCounter );
    return 1;
}

int setCounterToUser( int id ) {
    int i = 1;
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->userID == 0 )
            break;
        i++;
    }
    if( tmp != vecOfCompanies.cend() ) {
        vecOfCompanies.at( i ).setUser( id );
    } else {
        cout << "No free counters" << endl;
        return 0;
    }
    return 1;
}

int setCounterReadings( int id, int un, struct tm newTime, int newReadings ){
    int i = 1;
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->userID == id && tmp->uniqNumber == un )
            break;
        i++;
    }
    if( tmp != vecOfCompanies.cend() ) {
        CounterData *newCounterData;
        newCounterData->setData( newTime, newReadings );
        vecOfCompanies.at( i ).data.push_back( *newCounterData );
    } else {
        cout << "No such counter" << endl;
        return 0;
    }
    return 1;
}

void ShowUserData( int id ) {
    int i = 1;
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->userID == id )
            vecOfCompanies.at(i).showData();
        i++;
    }
}

void ShowCompanyData( int id ) {
    int i = 1;
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->companyID == id )
            vecOfCompanies.at(i).showData();
        i++;
    }
}

void ShowBadUsers( int id, int day, int month, int year ) {
    int i = 1;
    auto tmp = vecOfCompanies.cbegin();
    for( ; tmp !=  vecOfCompanies.cend(); tmp++ ) {
        if( tmp->companyID == id ){
            int i2 = 1;
            auto tmp2 = vecOfCompanies.at( i ).data.cbegin();
            for( ; tmp2 !=  vecOfCompanies.at( i ).data.cend(); tmp++ ) {
                if( vecOfCompanies.at( i ).data.at( i2 ).CompareDate( day, month, year ) == 1 )
                    break;
                i2++;
            }
            if( tmp2 != vecOfCompanies.at( i ).data.cend() ) {
                vecOfCompanies.at(i).showData();
            }
        }
        i++;
    }
}
