#define DEFAULT_PORT 27001
//#define DEFAULT_ADDR "10.210.13.134"
//#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_ADDR "192.168.0.100"
#define DEFAULT_BUFLEN 255

#include <winsock2.h>
#include <windows.h>
#include <mutex>
#include <iostream>
#include <vector>
#include <thread>
#include "service.h"
#include <ctime>


vector<Counter> vecOfCounters;
int flag = 0;

using namespace std;

int numOfClients = 0;                           // number of connected clients
vector<pair<pthread_t, long>> vecOfClients;     // vector of connected clients
mutex hMutex;                                   // global mutex

//char uniqNumber[] = "Unique_number:";
//char Readings[] = "Readings:";
//char Day[] = "Day:";
//char Month[] = "Month:";
//char Year[] = "Year:";

//----------------------------------------------------------------------------------------------------------------------------------------
// A function that killing client with id = clientID
bool KillClient( int clientID ) {
    if( (vecOfClients.size() < clientID) || (clientID <= 0) ) {     // check if there is a client with id = clientID
        cout << "No client with ID = " << clientID << endl;         // message about error
        return false;
    }
    auto tmp = vecOfClients.begin() + ( clientID - 1 );             // write a client with id = clientID to the variable tmp
    shutdown( tmp->second, SD_BOTH );                               // stop sending and receiving
    closesocket( tmp->second );                                     // closing socket
    pthread_join( tmp->first, NULL );                               // block the calling thread until the thread tmp->first ends
    vecOfClients.erase( tmp );                                      // deleting client from vecOfClients
    return true;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
void UserProcess( long tmpSocket ){
    cout << "Come to user process" << endl;
    char buffer[DEFAULT_BUFLEN];
    int id = GetID( tmpSocket );
    if( id <= 0 ){
        cout << "Client " << tmpSocket << " disconnect" << endl;
        flag = 1;
        return;
    }
    cout << "User id = " << id << endl;
    char an[DEFAULT_BUFLEN];
    memset( an, 0, DEFAULT_BUFLEN );
    memmove( an, "Your_choice:", 12 );
    int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
    //int iResult = sendLine( tmpSocket, an );         // sending message to client
    if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
        cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
        flag = 1;
        return;
    }

    while( true ) {
        memset( buffer, 0, DEFAULT_BUFLEN );
        iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        //iResult = recvLine( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        if( (iResult <= 0) ) {                                      // if no bytes - error - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            flag = 1;
            break;
        }
        if( strncmp( buffer, "1", 1 ) == 0 ){
            if( setCounterToUser( id ) == 1 ){
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "Set_counter_to_user@Your_choice:", 32 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
            else{
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "No_free_counters@Your_choice:", 29 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
        }
        else if( strncmp( buffer, "2", 1 ) == 0 ){
            char buf[DEFAULT_BUFLEN];
            ShowUserData( id, buf );
            char an[DEFAULT_BUFLEN];
            memset( an, 0, DEFAULT_BUFLEN );
            memmove( an, "Shown@Your_choice:", 18 );
            int iResult = send( tmpSocket, buf, DEFAULT_BUFLEN, 0 );         // sending message to client
            if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                flag = 1;
                break;
            }
        }
        else if( strncmp( buffer, "3", 1 ) == 0 ){
            char uniqNumber[DEFAULT_BUFLEN];
            memset( uniqNumber, 0, DEFAULT_BUFLEN );
            memmove( uniqNumber, "Unique_number", 13 );
            char Readings[DEFAULT_BUFLEN];
            memset( Readings, 0, DEFAULT_BUFLEN );
            memmove( Readings, "Readings", 8 );
            int un = getInfo( tmpSocket, uniqNumber );
            int data = getInfo( tmpSocket, Readings );
            struct tm *mytime;
            time_t mmm = time( NULL );
            mytime = localtime( &mmm );
            if( setCounterReadings( id, un, *mytime, data ) ){
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "Set_counter_readings@Your_choice:", 33 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
            else{
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "Have_no_such_counter@Your_choice:", 33 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
        }
        else if( strncmp( buffer, "4", 1 ) == 0 ){
            char an[DEFAULT_BUFLEN];
            memset( an, 0, DEFAULT_BUFLEN );
            memmove( an, "Bye_bye", 7 );
            int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
            if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                flag = 1;
                break;
            }
            cout << "User with id = " << id << " ends session" << endl;
            break;
        }
        else if( strncmp( buffer, "exit", 4 ) == 0) {                                        // if error of sending - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            flag = 1;
            break;
        }
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------
//
void CompanyProcess( long tmpSocket ){
    char buffer[DEFAULT_BUFLEN];
    int id = GetID( tmpSocket );
    if( id <= 0 ){
        cout << "Client " << tmpSocket << " disconnect" << endl;
        flag = 1;
        return;
    }
    cout << "Company id = " << id << endl;
    char an[DEFAULT_BUFLEN];
    memset( an, 0, DEFAULT_BUFLEN );
    memmove( an, "Your_choice:", 12 );
    int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
    //int iResult = sendLine( tmpSocket, an );         // sending message to client
    if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
        cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
        flag = 1;
        return;
    }

    while( true ) {
        memset( buffer, 0, DEFAULT_BUFLEN );
        iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        //iResult = recvLine( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        if( (iResult <= 0) ) {                                      // if no bytes - error - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            flag = 1;
            break;
        }
        if( strncmp( buffer, "1", 1 ) == 0 ){
            char uniqNumber[DEFAULT_BUFLEN];
            memset( uniqNumber, 0, DEFAULT_BUFLEN );
            memmove( uniqNumber, "Unique_number", 13 );
            int un = getInfo( tmpSocket, uniqNumber );
            if( addCounter( id, un ) == 1 ) {
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "Added@Your_choice:", 18 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
            else{
                char an[DEFAULT_BUFLEN];
                memset( an, 0, DEFAULT_BUFLEN );
                memmove( an, "Already_have_such_counter@Your_choice:", 38 );
                int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
                if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                    cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                    flag = 1;
                    break;
                }
            }
        }
        else if( strncmp( buffer, "2", 1 ) == 0 ){
            char buf[DEFAULT_BUFLEN];
            ShowCompanyData( id, buf );
            char an[DEFAULT_BUFLEN];
            memset( an, 0, DEFAULT_BUFLEN );
            memmove( an, "Shown@Your_choice:", 18 );
            int iResult = send( tmpSocket, buf, DEFAULT_BUFLEN, 0 );         // sending message to client
            if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                flag = 1;
                break;
            }
        }
        else if( strncmp( buffer, "3", 1 ) == 0 ){
            char Day[DEFAULT_BUFLEN];
            memset( Day, 0, DEFAULT_BUFLEN );
            memmove( Day, "Day", 3 );
            char Month[DEFAULT_BUFLEN];
            memset( Month, 0, DEFAULT_BUFLEN );
            memmove( Month, "Month", 5 );
            char Year[DEFAULT_BUFLEN];
            memset( Year, 0, DEFAULT_BUFLEN );
            memmove( Year, "Year", 4 );

            int day = getInfo( tmpSocket, Day );
            int month = getInfo( tmpSocket, Month );
            int year = getInfo( tmpSocket, Year );
            char buf[DEFAULT_BUFLEN];
            ShowBadUsers( id, day, month, year, buf );
            char an[DEFAULT_BUFLEN];
            memset( an, 0, DEFAULT_BUFLEN );
            memmove( an, "Shown@Your_choice:", 18 );
            int iResult = send( tmpSocket, buf, DEFAULT_BUFLEN, 0 );         // sending message to client
            if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                flag = 1;
                break;
            }
        }
        else if( strncmp( buffer, "4", 1 ) == 0 ){
            char an[DEFAULT_BUFLEN];
            memset( an, 0, DEFAULT_BUFLEN );
            memmove( an, "Bye_bye", 7 );
            int iResult = send( tmpSocket, an, DEFAULT_BUFLEN, 0 );         // sending message to client
            if( iResult <= 0 ) {                                        // if error of sending - client disconnect - exit from while
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                flag = 1;
                break;
            }
            cout << "Company with id = " << id << " ends session" << endl;
            break;
        }
        else if( strncmp( buffer, "exit", 4 ) == 0) {                                        // if error of sending - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            flag = 1;
            break;
        }
    }
    return;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// Client process: receives messages from client and sending answers
void * ClientProcess( void * ClientSocket ) {
    int iResult = 0;                                                // used to return function results
    long tmpSocket = ( long )ClientSocket;                          // new socket of new client

    while( true ) {
        char buffer[DEFAULT_BUFLEN];                                // buffer for message from client

        // iResult = recvLine( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        iResult = readn( tmpSocket, buffer, DEFAULT_BUFLEN );       // reading message from client
        if( (iResult <= 0) ) {                                      // if no bytes - error - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            break;
        }
        else if( strncmp( buffer, "exit", 4 ) == 0 ) {                   // if message about exit - client disconnect - exit from while
            cout << "Client " << tmpSocket << " disconnect" << endl;
            break;
        }
        else if( strncmp( buffer, "user", 4 ) == 0 )
            UserProcess( tmpSocket );

        else if( strncmp( buffer, "company", 7 ) == 0 )
            CompanyProcess( tmpSocket );

        else{
            char answer[] = "user/company?";                                       // answer from server  to client
            // iResult = sendLine( tmpSocket, answer );     // sending message to client
            iResult = send( tmpSocket, answer, DEFAULT_BUFLEN, 0 );     // sending message to client
            if( iResult == SOCKET_ERROR ) {                                        // if error of sending - client disconnect - exit from while
                cout << iResult << endl << WSAGetLastError() << endl;
                cout << "Error sending answer. Client " << tmpSocket << " disconnect" << endl;
                break;
            }
        }
        if( flag == 1 ) {                   // if message about exit - client disconnect - exit from while
            //cout << "Client " << tmpSocket << " disconnect" << endl;
            break;
        }
    }
                                                                    // exit from while - closing socket
    cout << "Closing client with socket: " << tmpSocket << endl;
    shutdown( tmpSocket, SD_BOTH );                                 // stop sending and receiving
    closesocket( tmpSocket );                                       // close socket
                                                                    // finding this socket in vecOfClients
    auto tmp = vecOfClients.cbegin();
    for( ; tmp !=  vecOfClients.cend(); tmp++ ) {
        if( tmp->second == tmpSocket )
            break;
    }
    if( tmp != vecOfClients.cend() )                                // checking that we did not go away from vector
        vecOfClients.erase( tmp );                                  // deleting socket from our vector
    cout << "Finish client " << tmpSocket << endl;                  // print message about end of working of client wt socket=tmpSocket
    pthread_exit( NULL );
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// Server process: releases connection of clients
void * ServerProcess( void * ListenSocket ) {
    long ClientSocket = ( long )ListenSocket;                       // new client socket for accept function

    while( true ) {
        pthread_t ClientThread;
        ClientSocket = accept( ( SOCKET )ListenSocket, 0, 0 );      // permit an incoming connection attempt on a socket
        if( ClientSocket < 0 ) {                                    // error in accept - end of while
            break;
        }
                                                                    // create new thread for new client
        if( pthread_create( &ClientThread, 0, ClientProcess, (void *) (ClientSocket) ) == 0 ) {
                                                                    // add new client to the end of vecOfClients
            vecOfClients.push_back( pair<pthread_t, long>( ClientThread, ClientSocket ) );
            cout << "New client is " << ClientSocket << endl;
        } else {                                                    // if error - print message - end of while
            cout << "Did not create new thread for client" << endl;
        }
    }
    hMutex.lock();
    for( auto tmp: vecOfClients ) {                                 // finding our client socket in vecOfClients
        if( tmp.second == ClientSocket ) {                          // if find - close it
            shutdown(tmp.second, SD_BOTH );
            closesocket( tmp.second );
            pthread_join( tmp.first, NULL );
        }
    }
    vecOfClients.clear();
    hMutex.unlock();
    pthread_exit( NULL );
    return 0;
}

//----------------------------------------------------------------------------------------------------------------------------------------
// Creates server and release its work
int main() {
    pthread_t hThread;                          // thread for server
    WSADATA wsaData;                            // variable for WinSock initialization
    int iResult = 0;                            // used to return function results
    sockaddr_in service;                        // the socket address to be passed to bind structure, which describes socket
    SOCKET ListenSocket = INVALID_SOCKET;       // Create a the listening socket

    //----------------------
    // Initialize Winsock
    iResult = WSAStartup( MAKEWORD(2, 2), &wsaData );
    if( iResult != NO_ERROR ) {
        cout << "WSAStartup failed with error: " << iResult << endl;
        return 1;
    }

    //----------------------
    // Create a SOCKET for listening for incoming connection requests
    ListenSocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( ListenSocket == INVALID_SOCKET || ListenSocket == SOCKET_ERROR ) {
        cout << "Socket function failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    //----------------------
    // The sockaddr_in structure specifies the address family (here - TCP),
    // IP address, and port for the socket that is being bound.
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = inet_addr( DEFAULT_ADDR );
    service.sin_port = htons( DEFAULT_PORT );

    //----------------------
    // Associate a local address with a socket
    iResult = bind( ListenSocket, (SOCKADDR *) &service, sizeof (service) );
    if( iResult == SOCKET_ERROR ) {
        wcout << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket( ListenSocket );
        WSACleanup();
        return 1;
    }

    //----------------------
    // Place a socket in a state in which it is listening
    // for an incoming connection.
    iResult = listen( ListenSocket, SOMAXCONN );
    if( iResult == SOCKET_ERROR ) {
        cout << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket( ListenSocket );
        WSACleanup();
        return 1;
    }

    //----------------------
    // Creating thread for server process
    iResult = pthread_create( &hThread, NULL, ServerProcess, (void *) (ListenSocket));
    if( iResult != 0) {
        cout << "Error: can not create hThread thread" << endl;
        shutdown( ListenSocket, SD_BOTH );
        closesocket( ListenSocket );
        WSACleanup();
        return 1;
    }

    cout << "Server is started"                << endl <<
            "Waiting for client to connect..." << endl;

    cout << endl <<
                "You can: "                    << endl <<
                "0: Menu"                      << endl <<
                "1: Close client"              << endl <<
                "2: Show clients"              << endl <<
                "3: Close all"                 << endl;

//----------------------------------------------------------------------------------------------------------------------------------------
    bool working = true;
    while( working ) {
        char choice;
        cin >> choice;                                              // read choice of user
        switch( choice ) {
        //--------------------------------------------------------------------------------------------------------------------------------
        case '0':                                                   // case 0: print commands
            cout << endl <<
                "You can: "                    << endl <<
                "0: Menu"                      << endl <<
                "1: Close client"              << endl <<
                "2: Show clients"              << endl <<
                "3: Close all"                 << endl;
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        case '1':                                                   // case 1: delete client wt id, printing by user
            hMutex.lock();
            int answer;
            cout << "Enter ID of client to close:" << endl;
            cin >> answer;
            cout << "Start to kill client with ID = " << answer << "..." << endl;
            if( KillClient( answer ) )
                cout << "Success of killing client with ID = " << answer << endl;
            else
                cout << "Error removing client with ID = " << answer << endl;
            hMutex.unlock();
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        case '2':                                                   // case 2: print list of clients
            hMutex.lock();
            if( vecOfClients.size() != 0 ) {
                cout << "Number of clients: " << vecOfClients.size() << endl;
                int i = 1;
                for( auto tmp : vecOfClients )
                    cout << "Client ID = "  << i++ <<
                            ", thread = "   << tmp.first <<
                            ", socket = "   << tmp.second << endl;
            } else
                cout << "No connected clients" << endl;
            hMutex.unlock();
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        case '3':                                                   // case 3: close all
            cout << "The server is turning off..." << endl;
            working = false;
            break;
        //--------------------------------------------------------------------------------------------------------------------------------
        default:
            cout << "Can't understand your choice. Try again." << endl;
        }
    }
    //------------------------------------------------------------------------------------------------------------------------------------

    shutdown( ListenSocket, SD_BOTH );          // Disable sends or receives on a ListenSocket
    closesocket( ListenSocket );                // No longer need ListenSocket. Close it
    pthread_join( hThread, NULL );
    WSACleanup();
    pthread_exit( NULL );
    return 0;
}
