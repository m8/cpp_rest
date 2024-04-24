#include "crow_all.h"
#include <cstdio>
#include <iostream>
#include <shared_mutex>
#include <map>
#include <vector>
#include <string>
#include <mutex>
#include <rocksdb/db.h>

#ifndef TEAM_CNT
#define TEAM_CNT 100000
#endif

typedef std::mutex Lock;
typedef std::lock_guard<Lock> WriteGuard;
typedef std::shared_lock<Lock> ReadGuard;


class Database {
private:
  long last_index = 0;

public:
    rocksdb::DB* db;

    Database() {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status = rocksdb::DB::Open(options, "temp_db", &db);
        if (!status.ok()) std::cerr << status.ToString() << std::endl;
        std::cout << "Database initizaled ..." << std::endl;
    }

    ~Database() {
        delete db;
    }

    std::string GET_team(int id) {
        std::string value;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), std::to_string(id), &value);
        if (s.ok()) {
          return "{ id: " + std::to_string(id) + ", title: " + value + "}";
        }
        return "Team not found";
    }

    std::vector<std::string> GET_teams(){
      std::vector<std::string> values;
      for (int k = 0; k < this->last_index; k++){
       values[k] = GET_team(k); 
      }
      return values;
    }    
    
    // Now removed... Serialize added in this one
    void POST_team(int id) {
        std::string key = std::to_string(id);
        std::string value = "Team ___";
        db->Put(rocksdb::WriteOptions(), key, value);
    }

    void DELETE_team(int id) {
        db->Delete(rocksdb::WriteOptions(), std::to_string(id));
    }

    void PATCH_team(int id, std::string newData) {
        std::string value;
        rocksdb::Status s = db->Get(rocksdb::ReadOptions(), std::to_string(id), &value);
        if (s.ok()) {
            db->Put(rocksdb::WriteOptions(), std::to_string(id), newData);
        } 
    }

    // Now initialize some teams
    void init(int n_teams) {
        for (int i = 0; i < n_teams; ++i) {
            this->POST_team(i);
            std::cout << "Team " << i << " created" << std::endl;
        }
    }
};



int main(int argc, char* argv[])
{
  int n_workers = std::thread::hardware_concurrency();

  if (argc > 1)
  {
    n_workers = std::stoi(argv[1]);
  }

  Database* db = new Database();

  /* Start REST api*/
  crow::SimpleApp app;
  app.loglevel(crow::LogLevel::Warning);

  CROW_ROUTE(app, "/")([]() { return "Hello world"; });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
    auto x = crow::json::load(req.body);
    if (!x)
    {
      return crow::response(400);
    }
    int team_id = x["teamId"].i();
    
    db->POST_team(team_id);

    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::GET)([&](const crow::request& req) {
    std::string result = "";
    for (auto team : db->GET_teams())
    {
      result += team + "\n";
    }
    return crow::response(200, result);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::DELETE)([&](const crow::request& req, int id) {
    db->DELETE_team(id);
    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::GET)([&](const crow::request& req, int id) {
    auto team = db->GET_team(id);
    return crow::response(200, team);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req, int id) {
    //db.PATCH_team(id);
    return crow::response(200);
  });

  app.port(18080).concurrency(n_workers).run();
}
