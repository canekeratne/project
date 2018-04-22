# Advanced Databases Project


### Make sure pistache is installed: 

http://pistache.io/quickstart


### Make sure mysql connector for c++ is installed:

sudo apt-get install libmysqlcppconn-dev


### Set ip address in *index.html* to that of the local machine:

serviceUrl: 'http://*YOUR-IP-HERE*:9082/keywordsearch'

### Compile server.cpp:

g++ server.cpp -lpistache -lpthread -std=c++11 -o server


