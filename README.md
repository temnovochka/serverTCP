# serverTCP
Multithreaded TCP server on Windows, which implements Utility Payments Service

1. Added connection of customers in main.cpp (users, companies).
2. Added structures for storing information about counters in server.h 
    class Counter stores information about 
        belonging to the company (CounterID), 
        to the user (UserID), 
        unique number of counter (UniqNumber) 
        and all data (data); 
    class CounterData - in fact, stores information about all readings, contain fields with 
        date (readingDate) 
        and with the readings themselves (readings)).
3. Added functions for adding new counters, displaying information about them, adding readings to them etc in server.cpp.
4. New functions in main.cpp: 
    UserProcess (for users) 
    and CompanyProcess (for company) - functions, where server go, when connecting the appropriate client.
