/*  Mark Canekeratne
    Kira Lessin
    REST Server   */

#include "myConnect.h"
#include "myTrie.h"
#include <algorithm>
#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
using namespace Pistache;
using namespace std;

trie t;
string k;
myConnect conn;
bool flag = 0;
vector<int> GLOBALV;
vector<string> GLOBALS;

// uncomment to test
// int temp = 999999;

class ApiEndpoint
{
public:
    ApiEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    void init(size_t thr = 2)
    {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start()
    {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown()
    {
        httpEndpoint->shutdown();
    }

    void setupRoutes()
    {
        using namespace Rest;
        Routes::Get(router, "/keywordsearch", Routes::bind(&ApiEndpoint::doSearch, this));
    }

    void doSearch(const Rest::Request& request, Http::ResponseWriter response)
    {
        Http::Uri::Query query = request.query();
	      auto keyword = query.get("term");

        if (keyword.isEmpty())
	         return;

        if (flag == 0)
        {
          conn.create_global("testing");
          conn.create_mapping("testing");
          vector<string> test = conn.describe("testing");
          for(int i = 0; i < test.size(); i++)
          {
            vector<int> IDLIST;
            if(test[i] != "record_id")
            {
              conn.create_local(test[i]);
              map<int, string> input = conn.insert(test[i], IDLIST);
              int i = 0;
              for(auto it = input.begin(); it != input.end(); it++)
              {
                transform(it->second.begin(), it->second.end(), it->second.begin(), ::tolower);
                t.insert(it->second, IDLIST[i]);
                GLOBALV.push_back(IDLIST[i]);
                GLOBALS.push_back(it->second);
                i++;
              }
            }
          }
          flag = 1;
        }
// uncomment to test
/*     cout << keyword.get() << endl;
     vector<string> treeDump;
     treeDump = t.prefixes();
     for (int i = 0; i < treeDump.size(); i++)
      cout << treeDump[i] << t.find(keyword.get()) << endl; */

    string keyw = keyword.get();
    vector<int> keyIDs;
    vector<string> autoC = t.autocomplete(keyw);
     for(int i = 0; i < autoC.size(); i++)
     {
       for(int k = 0; k < GLOBALS.size(); k++)
       {
         if(GLOBALS[k] == autoC[i])
          {
            keyIDs.push_back(GLOBALV[i]);
          }
       }
     }

// uncomment to test
/*
     for (int i = 0; i < keyIDs.size(); i++)
     {
      if (temp != keyIDs[i])
      {
        cout << keyIDs[i] << ",";
        temp = keyIDs[i];
      }
     }
     cout << endl;
*/
	   string result = "[";
     istringstream iss(keyword.get());
	   string k;
    	while (getline(iss, k, '+'))
      {
           vector<string> v;
           v = t.autocomplete(k);
	         for (int i = 0; i < v.size(); i++)
           {
	          result += "\"";
	          result += v[i];
	          result += "\",";
           }
    	}
	    if (result.length() > 1) result.pop_back();
	    result+= "]";

	    response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
	    response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
	    response.send(Http::Code::Ok, result);
    }

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main(int argc, char *argv[])
{

    Port port(9082);
    vector<string> test;
    int thr = 2;

    if (argc >= 2)
    {
        port = std::stol(argv[1]);

        if (argc == 3)
            thr = std::stol(argv[2]);
    }

    Address addr(Ipv4::any(), port);

    ApiEndpoint api(addr);

    cout << "Listening on port 9082" << endl;

    api.init(thr);
    api.start();
    api.shutdown();
}
