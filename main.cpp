/*  Mark Canekeratne
    Kira Lessin
    COP 5725 Project
    Trie header File

	g++ -std=c++11 main.cpp -lmysqlcppconn	
*/

#include "connect.h"
#include "trie.h"
#include <iostream>
using namespace std;

trie t;
string field;

string menu();
vector<int> join(vector<int>, vector<int>);

int main ()
{
  connect conn;
  conn.create_global("testing");
  conn.create_mapping("testing");
  

  vector<string> test = conn.describe("testing");
  for(int i = 0; i < test.size(); i++)
  {
    if(test[i] != "record_id"){
      //creating local tables for form based search
	  conn.create_local(test[i]);
	  
      map<int, string> input = conn.insert(test[i]);
      for(auto it = input.begin(); it != input.end(); it++)
      {
        t.insert(it->second, it->first);
      }
    }
  }


  

  string search = menu();
  vector<string> keywords = conn.split(search, ' ');
  vector<int> general_local;
  vector<int> field_local; 

  for(int i = 0; i < keywords.size(); i++)
  {
    vector<string> v;
    v = t.autocomplete(keywords[i]);
    /*need to ask questions about trie structure cause doesn't support search currently*/
    for (int j = 0; j < v.size(); j++)
    {
        general_local.push_back(conn.local_search(v[j]));
    }

    
  }

  field_local = conn.field_search(field);

  vector<int> joined = join(field_local, general_local);

  conn.print_results(field, joined);

  conn.close();
  return 0;
}

vector<int> join (vector<int> v1, vector<int> v2)
{
  vector<int> ret;
  for(int i = 0; i < v1.size(); i++)
  {
    for(int j = 0; j < v2.size(); j++)
    {
      if(v1[i] == v2[j])
        ret.push_back(v1[i]);
    }
  }
  return ret;
}

string menu()
{
  char selection;
  cout << "\nSelect which field you'd like to search in: " << endl << endl;
  cout << "Enter 'a' for author search " << endl;
  cout << "Enter 't' for title search " << endl;
  cout << "Enter 'y' for year search " << endl;
  cout << "Enter 'v' for volume search " << endl;
  cout << "Enter 'j' for journal search " << endl;
  cout << "Enter 'u' for url search " << endl << endl;
  cout << "Input: ";
  cin >> selection;
  selection = tolower(selection);
  switch(selection)
  {
    case 'a':
    {
      field = "authors";
      break;
    }
    case 't':
    {
      field = "title";
      break;
    }
    case 'y': ;
    { 
      field = "year";
      break;
    }
    case 'v':
    {
      field = "volume";
      break;
    }
    case 'j':
    {
      field = "journal";
      break;
    }
    case 'u':
    {
      field = "url";
    }
    default:
      break;      
  }
  cin.ignore();
  cout << "Enter your keywords: ";
  string keyword;
  getline (cin, keyword);
  cout << endl;
  return keyword;
}