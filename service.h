#ifndef SERVICE_H_INCLUDED
#define SERVICE_H_INCLUDED

#define _CRT_SECURE_NO_WARNINGS

#define N 512
#include <vector>
#include <iostream>
#include <array>
#include <ctime>

using namespace std;

struct CounterData {
private:
    struct tm readingDate;
    int readings;

public:
    void setData( struct tm newReadingDate, int newReadings ) {
        readingDate = newReadingDate;
        readings  = newReadings;
    }

    void showData() {
        cout << " date = " << readingDate.tm_mday << "." << readingDate.tm_mon << "." << readingDate.tm_year
             << ", readings = " << readings;
    }
    int CompareDate( int day, int month, int year ) {
        if( readingDate.tm_year == year && readingDate.tm_mon == month && readingDate.tm_mday == day )
            return 1;
        else
            return 0;
    }
};

class Counter{
public:
    int userID;
    int companyID;
    int uniqNumber;
    vector<CounterData> data;

    void setUser( int id ) {
        userID = id;
    }

    void showData() {
        if( data.size() != 0 ) {
            cout << "Number of readings: " << data.size() << endl;
            vector<CounterData>::const_pointer tmp;
            tmp = data.data();
            for (size_t n = data.size(); 0 < n; --n, tmp++)
                cout << " " << tmp;
            cout << endl;
        } else
                cout << "No data" << endl;
    }
};

int getInfo( long tmpSocket, char *txt );
int readn( long socket, char *recvbuf, int recvbuf_len );
int addCounter( int id, int un );
int setCounterToUser( int id );
int setCounterReadings( int id, int un, struct tm newTime, int newReadings );
void ShowUserData( int id );
void ShowCompanyData( int id );
void ShowBadUsers( int id, int day, int month, int year );
int GetID( long tmpSocket );

#endif // SERVICE_H_INCLUDED
