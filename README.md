# Advanced Databases Project


### Make sure pistache is installed: 

http://pistache.io/quickstart


### Make sure mysql connector for c++ is installed:

sudo apt-get install libmysqlcppconn-dev

### Set ip address in index.html (located in the server directory) to that of the local machine:

serviceUrl: 'http://[YOUR-IP-HERE]:9082/keywordsearch'

### Compile server.cpp (located in the server directory):

g++ -std=c++11 server.cpp -lmysqlcppconn -lpistache -lpthread -o server

### Compile search.cpp

g++ -std=c++11 search.cpp -lmysqlcppconn

### Running over different datasets

In order to run over the different database sections provided, the local file name for loading the data into the database, on line 98 of the 'connect.h' file, needs to be changed from db_50.csv to the name of the appropriate database section file.

Also, if the file you are loading from is in the same directory as sample.cpp only the file name is required. If the data is being loaded from another directory then the path name to the file must be correct in order to load properly

