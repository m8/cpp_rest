#include "crow_all.h"
#include <cstdio>
#include <iostream>
#include <shared_mutex>
#include <map>
#include <vector>
#include <string>
#include <mutex>

typedef std::shared_mutex Lock;
typedef std::unique_lock<Lock> WriteLock;
typedef std::shared_lock<Lock> ReadLock;

#ifndef TEAM_CNT
#define TEAM_CNT 20000
#endif

static uint64_t last_team_id = 0;

class Team {
public:
    Team() : id(0), data(0) {}
    int id;
    int data;

    std::string get_data() {
        ReadLock lock(private_lock);
        return "Team " + std::to_string(id);
    }

    void set_data(int newData) {
        WriteLock lock(private_lock);
        data = newData;
    }
private:
    Lock private_lock;
};

class Database {
public:
    std::map<int, Team*> global_team_list;

    std::string GET_team(int id) {
        return global_team_list.at(id)->get_data();
    }

    std::vector<std::string> GET_teams() {
        std::vector<std::string> result;
        for (auto &entry : global_team_list) {
            result.push_back(entry.second->get_data());
        }
        return result;
    }

    void POST_team() {
        WriteLock lock(global_lock);
        Team team;
        team.id = ++last_team_id;
        global_team_list[team.id] = &team;
    }

    void DELETE_team(int id) {
        WriteLock lock(global_lock);
        global_team_list.erase(id);
    }

    void PATCH_team(int id, int newData) {
        global_team_list.at(id)->set_data(newData);
    }

    // Now initialize some teams
    void init(int n_teams) {
        WriteLock lock(global_lock);
        for (int i = 0; i < n_teams; ++i) {
            Team* team = new Team();
            team->id = i;

            // Set the teams now
            global_team_list[i] = team;
        }
    }

private:
    Lock global_lock;
};


int main(int argc, char* argv[])
{
  int n_workers = std::thread::hardware_concurrency();

  if (argc > 1)
  {
    n_workers = std::stoi(argv[1]);
  }

  /* Init simple database */
  Database db;
  db.init(TEAM_CNT);

  /* Start REST api*/
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")([]() { return "Hello world"; });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
    db.POST_team();
    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::GET)([&](const crow::request& req) {
    std::string result = "";
    for (auto team : db.GET_teams())
    {
      result += team + "\n";
    }
    return crow::response(200, result);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::DELETE)([&](const crow::request& req, int id) {
    db.DELETE_team(id);
    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::GET)([&](const crow::request& req, int id) {
    auto team = db.GET_team(id);
    return crow::response(200, team);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req, int id) {
    //db.PATCH_team(id);
    return crow::response(200);
  });

  app.port(18080).concurrency(n_workers).run();
}
