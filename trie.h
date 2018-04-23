/*  Mark Canekeratne
    Kira Lessin
    COP 5725 Project
    Trie header File
*/

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iostream>

using namespace std;

class trie
{
    public:
        /* variables */
        map<char, trie> children;
        string value;
        bool wordEnd;
        vector<int> IDs;

        /* function prototypes */
        trie(const string &val = "");
        void add(char);
        string find(const string &);
        void insert(const string &, const int &);
        vector<string> prefixes();
        vector<string> autocomplete(const string &);
};

/* constructor */
trie::trie(const string &val)
{
    value = val;
    wordEnd = false;
}

/* add character to trie */
void trie::add(char c)
{
    if (value == "")
        children[c] = trie(string(1, c));
    else
        children[c] = trie(value + c);
}

/* find string */
string trie::find(const string &word)
{
    trie * node = this;
    for (int i = 0; i < word.length(); i++)
    {
        const char c = word[i];
        if (node->children.find(c) == node->children.end())
            return "";
        else
            node = &node->children[c];
    }
    return node->value;
}

/* insert string */
void trie::insert(const string &word, const int &ID)
{
    trie * node = this;
    bool duplicate = false;

    for (int i = 0; i < word.length(); i++)
    {
        const char c = word[i];
        if (node->children.find(c) == node->children.end())
            node->add(c);
        node = &node->children[c];
    }
    node->wordEnd = true;

    for (int i = 0; i < node->IDs.size(); i++)
      if (node->IDs[i] == ID)
        duplicate = true;

    if (!duplicate)
      node->IDs.push_back(ID);
}

/* find all prefixes */
vector<string> trie::prefixes() {
    vector<string> results;
    if (wordEnd)
        results.push_back(value);

    if (children.size())
    {
        map<char, trie>::iterator iter;
        vector<string>::iterator node;
        for(iter = children.begin(); iter != children.end(); iter++)
        {
            vector<string> nodes = iter->second.prefixes();
            results.insert(results.end(), nodes.begin(), nodes.end());
        }
    }
    return results;
}

/* autocomplete prefix */
vector<string> trie::autocomplete(const string &prefix) {
    trie * node = this;
    vector<string> results;
    for (int i = 0; i < prefix.length(); i++)
    {
        const char c = prefix[i];
        if (node->children.find(c) == node->children.end())
            return results;
        else
            node = &node->children[c];
    }
    return node->prefixes();
}
