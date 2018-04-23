#include "connect.h"
#include "trie.h"
#include <iostream>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;


trie t;
string field;

string form_menu();
char menu();

vector<int> join(vector<int>, vector<int>);

int SIZE = 6;

int main ()
{
  /* Basic database setup*/
  connect conn;
  conn.create_global("testing");
  conn.create_mapping("testing");
  

  vector<string> test = conn.describe("testing");
  for(int i = 0; i < test.size(); i++)
  {
    if(test[i] != "record_id"){
      conn.create_local(test[i]);
      map<int, string> input = conn.insert(test[i]);
      for(auto it = input.begin(); it != input.end(); it++)
      {
        t.insert(it->second, it->first);
      }
    }
  }

char search_type = menu();

  
if((search_type == 'F') || (search_type == 'f'))
{
  string search = form_menu();
  vector<string> keywords = conn.split(search, ' ');
  vector<int> general_local;
  vector<int> field_local; 

  /*starting timing search function for form-based search*/
  auto start = high_resolution_clock::now();

  for(int i = 0; i < keywords.size(); i++)
  {
    vector<string> v;
    v = t.autocomplete(keywords[i]);
    for (int j = 0; j < v.size(); j++)
    {
        general_local.push_back(conn.local_search(v[j]));
    }
  }

  field_local = conn.field_search(field);

  vector<int> joined = join(field_local, general_local);

  conn.print_results(field, joined);

  /*ending timing function for form-based search*/
  auto stop = high_resolution_clock::now();

  auto duration = duration_cast<milliseconds>(stop - start);
 
    cout << "Time taken by form-based search: " << duration.count() << " milliseconds" << endl;
}
else if((search_type == 'S')||(search_type == 's'))
{
  cin.ignore();
  cout << endl;
  cout << "Enter your keywords: ";
  string keywords;
  getline (cin, keywords);
  vector<string> sql_words;
  vector<string> key_words;

  vector<string> words = conn.split(keywords, ' ');

  vector<string> sugg = conn.sql_suggest(words);
  cout << endl << "Please Enter the Number of the SQL Search you would like to run:" << endl << endl;
  int answer;
  for( int i = 0; i < sugg.size(); i++)
    cout << i+1 << ". "<<sugg[i] << endl;

  cout << "\nEnter: ";
  cin >> answer; 

  /*starting timing search function for form-based search*/
  auto start = high_resolution_clock::now();

  conn.execSQL(answer-1, sugg);
  
  /*ending timing function for form-based search*/
  auto stop = high_resolution_clock::now();

  auto duration = duration_cast<microseconds>(stop - start);
  cout << endl <<"Time taken by SQL-based search: " << duration.count() << " microseconds" << endl;

}
else
  cout << endl << "ERROR: Invalid Input!" << endl;

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


char menu()
{
  char type;
  cout << endl << "How would you like to search?" << endl<< endl;
  cout << "Form-based Search: 'F'" << endl;
  cout << "SQL-based Search: 'S'" << endl << endl;
  cout << "Enter Search Type: ";
  cin >> type;

  return type;



}

string form_menu()
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
    {
      cout << "Error: Invalid Input!" << endl;
      exit(0);
      break;      
    }
  }
  cin.ignore();
  cout << "Enter your keywords: ";
  string keyword;
  getline (cin, keyword);
  cout << endl;
  return keyword;
}