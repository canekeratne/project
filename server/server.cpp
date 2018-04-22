/* 
 * REST server by C++
*/


#include <algorithm>

#include <pistache/http.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include "myTrie.h"

using namespace std;
using namespace Pistache;


class ApiEndpoint {
public:
    ApiEndpoint(Address addr)
        : httpEndpoint(std::make_shared<Http::Endpoint>(addr))
    { }

    void init(size_t thr = 2) {
        auto opts = Http::Endpoint::options()
            .threads(thr)
            .flags(Tcp::Options::InstallSignalHandler);
        httpEndpoint->init(opts);
        setupRoutes();
    }

    void start() {
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() {
        httpEndpoint->shutdown();
    }

private:
    void setupRoutes() {
        using namespace Rest;

        Routes::Get(router, "/keywordsearch", Routes::bind(&ApiEndpoint::doSearch, this));
    }

    void doSearch(const Rest::Request& request, Http::ResponseWriter response) {
        Http::Uri::Query query = request.query();
	auto keyword = query.get("term");

	if (keyword.isEmpty()) 
	   return;

	trie t;
  	vector<string> v;

	t.insert("database", 1);
  	t.insert("databases", 2);
  	t.insert("lin", 3);
	t.insert("liu", 4);
	t.insert("luis", 5);
  	t.insert("vldb", 6);
  	t.insert("data", 7);

  	v = t.autocomplete(keyword.get());

	string result = "[";
	for (int i = 0; i < v.size(); i++) {
	   result += "\"";
	   result += v[i];
	   result += "\"";
	   if (i < v.size() - 1) result += ",";
	}

	result+= "]";

	response.headers().add<Http::Header::ContentType>(MIME(Application, Json));
	response.headers().add<Http::Header::AccessControlAllowOrigin>("*");
	response.send(Http::Code::Ok, result);
    }

    std::shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main(int argc, char *argv[]) {
    Port port(9082);

    int thr = 2;

    if (argc >= 2) {
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
