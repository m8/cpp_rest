#include "crow_all.h"

#include <cstdio>
#include <iostream>
#include <shared_mutex>

Lock sharedLock;

typedef std::shared_mutex Lock;
typedef std::unique_lock<Lock> WriteLock; // C++ 11
typedef std::shared_lock<Lock> ReadLock; // C++ 14

static uint64_t last_team_id = 0;

class Team
{
public:
  Team() : id(0) {}

  int id;
  int data;
  // std::vector<User> users;
  std::string get_name()
  {
    return "Team " + std::to_string(id);
  }
};

class Database
{
public:
  std::map<int, Team> teams;

  Team GET_team(int id)
  {
    ReadLock lock(sharedLock);
    return teams[id];
  }

  std::vector<Team> GET_teams()
  {
    ReadLock lock(sharedLock);
    std::vector<Team> result;
    for (auto it = teams.begin(); it != teams.end(); it++)
    {
      result.push_back(it->second);
    }
    return result;
  }

  void POST_team()
  {
    WriteLock lock(sharedLock);
    Team team;
    team.id = last_team_id++;
    teams[team.id] = team;
  }

  void DELETE_team(int id)
  {
    WriteLock lock(sharedLock);
    teams.erase(id);
  }

  void PATCH_team(int id)
  {
    WriteLock lock(sharedLock);
    teams[id].data = rand();
  }

  void init(int n_teams)
  {
    for (int i = 0; i < n_teams; i++)
    {
      Team team;
      team.id = i;
      this->teams[i] = team;
    }
  }
};

int main(int argc, char* argv[])
{
  int n_workers = std::thread::hardware_concurrency();
  int n_teams = 100;

  if (argc > 1)
  {
    n_workers = std::stoi(argv[1]);
  }

  /* Init simple database */
  Database db;
  db.init(n_teams);

  /* Start REST api*/
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([]() { return "Hello world"; });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::POST)([&](const crow::request& req) {
    db.POST_team();
    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams").methods(crow::HTTPMethod::GET)([&](const crow::request& req) {
    std::string result = "";
    for (auto team : db.GET_teams())
    {
      result += team.get_name() + "\n";
    }
    return crow::response(200, result);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::DELETE)([&](const crow::request& req, int id) {
    db.DELETE_team(id);
    return crow::response(200);
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::GET)([&](const crow::request& req, int id) {
    return crow::response(200, db.GET_team(id).get_name());
  });

  CROW_ROUTE(app, "/teams/<int>").methods(crow::HTTPMethod::PATCH)([&](const crow::request& req, int id) {
    Team team;
    team.id = id;
    db.PATCH_team(id);
    return crow::response(200);
  });

  app.port(18081).concurrency(n_workers).run();
}