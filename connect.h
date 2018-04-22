/*  Mark Canekeratne
    Kira Lessin
    COP 5725 Project
    Form-based Search Set-up

*/

/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <map>
#include <iterator>
#include <algorithm>

/*
  cppconn includes to facilitate the 
  MySQL connection
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

using namespace std;


class connect
{
  public:
    /*variables*/
    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;
    sql::ResultSet *res;
    sql::ResultSet *res_1;
    string global_table;
    int local_id = 1;

    /*function prototypes*/
    connect(string db_name);
    void create_global(string table_name);
    void create_local(string table_name);
    void create_mapping(string table_name);
    void insert_mapping(int local, string global);
    int local_search(string data);
    void print_results(string field, vector<int> loc_ids);
    bool unique(vector<int> v, int i);
    vector<int> field_search(string field);
    map<int, string> insert(string table_name);
    vector<string> describe(string table_name);
    vector<string> split(const string &s, char delim);
    bool isUnique(string table, string data);
    void close();

};

connect::connect(string db_name = "test")
{
  driver = get_driver_instance();
  con = driver->connect("localhost", "root", "seedubuntu");
  stmt = con->createStatement();

  string q = "USE ";
  q += db_name;
  stmt -> execute(q);
}

void connect::create_global(string table_name)
{
  string q = "DROP TABLE IF EXISTS ";
  q += table_name;
  stmt->execute(q);

  q = "CREATE TABLE ";
  q += table_name;
  q += " (record_id INT(6) NOT NULL PRIMARY KEY, authors VARCHAR(200), title VARCHAR(200), year INT, volume INT, journal VARCHAR(50), url VARCHAR(100))";
  stmt->execute(q);

  q = "LOAD DATA LOCAL INFILE 'test1.csv' INTO TABLE ";
  q += table_name;
  q += " fields terminated by ',' lines terminated by '\n'";
  stmt -> execute(q);

  global_table = table_name;
}

void connect::create_local(string table_name)
{
  string q = "DROP TABLE IF EXISTS ";
  q += table_name;
  stmt->execute(q);

  q = "CREATE TABLE ";
  q += table_name;
  q += " (local_id INT(6) NOT NULL PRIMARY KEY, data VARCHAR(200))";
  stmt->execute(q);
}

void connect::create_mapping(string table_name)
{
  stmt->execute("DROP TABLE IF EXISTS local");
  stmt->execute("CREATE TABLE local (local_id INT PRIMARY KEY, global_id INT)");

  stmt->execute("DROP TABLE IF EXISTS global");
  stmt->execute("CREATE TABLE global (global_id INT, local_id INT)");
}

void connect::insert_mapping(int local, string global)
{
  string q = "INSERT INTO local(local_id, global_id) VALUES (";
  q += to_string(local);
  q += ", ";
  q += global;
  q += ")";
  stmt -> execute(q);

  q = "INSERT INTO global(global_id, local_id) VALUES (";
  q += global;
  q += ", ";
  q += to_string(local);
  q += ")";
  stmt -> execute(q);
            
}

vector<string> connect::describe(string table_name){
  vector<string> ret_v;

  string q = "DESCRIBE ";
  q += table_name;

  res = stmt->executeQuery(q);
  while(res->next())
  {
    ret_v.push_back(res->getString(1));
  }
  return ret_v;
}

void connect::print_results(string field, vector<int> loc_ids)
{
  vector<int> g_ids;
  vector<string> v = describe(global_table);
  for(int i = 0; i < loc_ids.size(); i++){
    string q = "SELECT global_id FROM local WHERE local_id = '";
    q += to_string(loc_ids[i]);
    q += "'";
    res = stmt->executeQuery(q);
    while(res->next())
    {
      int in = stoi(res->getString(1));
      if(unique(g_ids, in))
        g_ids.push_back(in);
    }
  }
  bool should_break;
  cout << "LOCAL RESULTS" << endl;
  for(int i = 0; i < g_ids.size(); i++)
  {
    string q = "SELECT ";
    q += field;
    q +=" FROM ";
    q += global_table;
    q += " WHERE record_id = ";
    q += to_string(g_ids[i]);

    res = stmt->executeQuery(q);
    while(res->next())
    {
        string a = res->getString(1);
        if((a.find(';') != string::npos)&&(field == "authors"))
        {
          vector<string> split1 = split(a, ';');
          for(int f = 0; f < split1.size(); f++)
          {
            vector<string> split2 = split(split1[f], ' ');
            for(int g = 0; g < split2.size(); g++)
            {
              string que = "SELECT local_id FROM ";
              que += field;
              que += " WHERE data = '";
              que += split2[g];
              que += "'";
              res_1 = stmt->executeQuery(que);
              if(res_1->next())
              {
                int tmp = stoi(res_1->getString(1));
                if (tmp == g_ids[i])
                {
                  cout << split1[f] << endl;
                  should_break = true;
                  break;
                }
              }
            }
            if(should_break)
              break;
          }

        }
        else
          cout << a << endl;
    }
  }
  cout << endl << "GLOBAL RESULTS" << endl;
  for(int i = 0; i < g_ids.size(); i++)
  {
    string q = "SELECT * FROM ";
    q += global_table;
    q += " WHERE record_id = ";
    q += to_string(g_ids[i]);

    res = stmt->executeQuery(q);
    while(res->next())
    {
      for(int i = 0; i < v.size(); i++){
        string a = v[i];
        string r = res->getString(v[i]);
        transform(a.begin(), a.end(), a.begin(), ::toupper);
        cout << a << ": ";
        cout << r << endl;
      }
      cout << endl;

    }
  }
}

bool connect::unique(vector<int> v, int k)
{
  bool u;
  for(int i = 0; i < v.size(); i++)
  {
    if(k == v[i])
      return false;
  }
  return true;
}

map<int, string> connect::insert(string table_name)
{
  map<int, string> input;

  string q = "SELECT * FROM ";
  q += global_table;
  res = stmt->executeQuery(q);
  while(res->next()){
    string globe = res->getString("record_id");
    string data = res->getString(table_name);
    if(table_name == "authors")
    {
      if(data.find(';') != string::npos)
      {
        vector<string> string_vec = split(data, ';');
        for(int i = 0; i < string_vec.size(); i++)
        {
          string temp = string_vec[i];
          if(isspace(temp[i]))
            temp.erase(0,1);
          q = "INSERT INTO ";
          q += table_name;
          q += " (local_id, data) VALUES (";
          q += to_string(local_id);
          q += ", '";
          q += temp;
          q += "')";
          stmt -> execute(q);
          input.insert(pair <int, string> (local_id, temp));
          insert_mapping(local_id, globe);
          local_id++;
        }
      }else{
        q = "INSERT INTO ";
        q += table_name;
        q += " (local_id, data) VALUES (";
        q += to_string(local_id);
        q += ", '";
        q += data;
        q += "')";
        stmt -> execute(q);
        input.insert(pair <int, string> (local_id, data));
        insert_mapping(local_id, globe);
        local_id++;

      }
    }
    if(data.find(';') != string::npos)
    {
      vector<string> string_vec = split(data, ';');
      for(int v = 0; v < string_vec.size(); v++)
      {
        vector<string> vs = split(string_vec[v], ' ');
        for(int i = 0; i < vs.size(); i++)
        {
          if(table_name == "journal"){
            if(!isUnique(table_name, data))
              string x = "ignore";
            else
            {
              q = "INSERT INTO ";
              q += table_name;
              q += " (local_id, data) VALUES (";
              q += to_string(local_id);
              q += ", '";
              q += vs[i];
              q += "')";
              stmt -> execute(q);
              input.insert(pair <int, string> (local_id, vs[i]));
              insert_mapping(local_id, globe);
              local_id++;
            }
          } else{
            q = "INSERT INTO ";
            q += table_name;
            q += " (local_id, data) VALUES (";
            q += to_string(local_id);
            q += ", '";
            q += vs[i];
            q += "')";
            stmt -> execute(q);
            input.insert(pair <int, string> (local_id, vs[i]));
            insert_mapping(local_id, globe);
            local_id++;
          }
        }
      }
    }
    else
    {

      vector<string> vs2 = split(data, ' ');
      for(int j = 0; j < vs2.size(); j++){

        if(table_name == "journal"){

          if(!isUnique(table_name, data))
            string x = "ignore";
          else
          {
            q = "INSERT INTO ";
            q += table_name;
            q += " (local_id, data) VALUES (";
            q += to_string(local_id);
            q += ", '";
            q += vs2[j];
            q += "')";
            stmt -> execute(q);
            input.insert(pair <int, string> (local_id, vs2[j]));
            insert_mapping(local_id, globe);
            local_id++;

          }
        }
        else
        {
          q = "INSERT INTO ";
          q += table_name;
          q += " (local_id, data) VALUES (";
          q += to_string(local_id);
          q += ", '";
          q += vs2[j];
          q += "')";
          stmt -> execute(q);
          input.insert(pair <int, string> (local_id, vs2[j]));
          insert_mapping(local_id, globe);
          local_id++;
        }
      }
    }

  }
  return input;
}

int connect::local_search(string data)
{
  vector<string> loop = describe(global_table);
  for(int i = 0; i < loop.size(); i++)
  {
    string q = "SELECT local_id FROM ";
    q += loop[i];
    q += " WHERE data = '";
    q += data;
    q += "'";

    res = stmt->executeQuery(q);
    if(res->next())
    {
      return stoi(res->getString(1));
    } 
    //return 0;
  }
}

vector<int> connect::field_search(string field)
{
  vector<int> temp;
  string q = "SELECT local_id FROM ";
  q += field;

  res = stmt->executeQuery(q);
  while(res->next())
  {
    string test = res->getString(1);
    int in = stoi(test);
    temp.push_back(in);
  } 
  return temp;
}



void connect::close()
{
  delete res;
  delete con;
  delete stmt;
}

vector<string> connect::split(const string &s, char delim){
  stringstream ss(s);
  string item;
  vector<string> tokens;
  while(getline(ss, item, delim)){
    if(!item.empty()){
      tokens.push_back(item);
    }
  }
  return tokens;
}

bool connect::isUnique(string table, string data){

  string q = "SELECT * FROM ";
  q += table;
  q += " WHERE data = '";
  q += data;
  q += "'";

  res = stmt->executeQuery(q);

  if(res->next())
    return false;
  
  return true;

}
