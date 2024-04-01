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

static uint64_t last_team_id = 0;

class Team {
public:
    Team() : id(0), data(0) {}
    int id;
    int data;
    // std::vector<User> users;

    std::string get_data() {
        ReadLock lock(m_lock);
        return "Team " + std::to_string(id);
    }

    void set_data(int newData) {
        WriteLock lock(m_lock);
        data = newData;
    }

private:
    Lock m_lock;
};

class Database {
public:
    std::map<int, Team*> teams;

    std::string GET_team(int id) {
        return teams.at(id)->get_data();
    }

    std::vector<std::string> GET_teams() {
        std::vector<std::string> result;
        for (auto &entry : teams) {
            result.push_back(entry.second->get_data());
        }
        return result;
    }

    void POST_team() {
        WriteLock lock(globalLock);
        Team team;
        team.id = ++last_team_id;
        teams[team.id] = &team;
    }

    void DELETE_team(int id) {
        WriteLock lock(globalLock);
        teams.erase(id);
    }

    void PATCH_team(int id, int newData) {
        teams.at(id)->set_data(newData);
    }

    void init(int n_teams) {
        WriteLock lock(globalLock);
        for (int i = 0; i < n_teams; ++i) {
            Team* team = new Team();
            team->id = i;
            teams[i] = team;
            std::cout << "Team " << i << " created" << std::endl;
        }
    }

private:
    Lock globalLock;
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