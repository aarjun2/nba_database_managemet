#include <stdlib.h>
#include <iostream>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

//for demonstration only. never save your password in the code!
const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "13#Paulgeorge";

int main()
{
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::ResultSet* res;

    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }

    con->setSchema("nba_db");
    int op = 0;
    while (op != 7) {
        cout << "Option 1: Administrative Access" << endl;
        cout << "Option 2: GM: Find the most efficient player on a top team" << endl;
        cout << "Option 3: Player: Find the stats under my ID" << endl;
        cout << "Option 4: Fan: Highest scoring player in their favorite team" << endl;
        cout << "Option 5: Team: Popularity, Coach and GM" << endl;
        cout << "Option 6: Fantasy Draft candidates" << endl;
        cout << "Option 7: Exit" << endl;
        cout << "Enter option: ";
        cin >> op;
        if (op == 1) {
            string u, p, s;
            cout << "Enter USERNAME" << endl;
            cin >> u;
            cout << "Enter PASSWORD" << endl;
            cin >> p;
            if (u == "wizard" && p == "bunny") {
                cout << "Enter SQL statement to execute: ";
                //cin >> s;
                try {
                    stmt = con->createStatement();
                    stmt->execute("CREATE TABLE COACH (coachId SMALLINT NOT NULL UNIQUE, coachFname VARCHAR(50) NOT NULL, coachLname VARCHAR(50) NOT NULL, experience TINYINT NOT NULL, age TINYINT NOT NULL, `win%` DECIMAL(5, 3) NOT NULL, PRIMARY KEY(coachId));");
                    cout << "CREATION SUCCESSFUL" << endl;
                }
                catch (sql::SQLException& e) {
                    cout << "Error: " << e.what() << endl;
                }
            }
        }

        if (op == 2) {
            stmt = con->createStatement();
            res = stmt->executeQuery("SELECT p.Fname, p.Lname, (s.`3P%` + s.`FG%`) AS efficiency FROM PLAYER p JOIN TEAM t ON p.tname = t.teamName JOIN STAT s ON p.playerId = s.statPlayerID WHERE t.teamName = (SELECT teamRecord FROM RECORD ORDER BY homeWins + awayWins DESC LIMIT 1) ORDER BY efficiency DESC LIMIT 1;");
            while (res->next()) {
                string Fname = res->getString("Fname");
                string Lname = res->getString("Lname");
                double val = res->getDouble("efficiency");
                cout << "First name = " << Fname << ",  Last name = " << Lname << ",  Efficiency = " << val << endl;
            }
        }
        if (op == 3) {
            int p_id = 0;
            cout << "What is your player id?";
            cin >> p_id;
            string str_id = to_string(p_id);
            string specific = "SELECT P.Fname, P.Lname, S.PPG, S.APG, S.RPG, S.SPG, S.BPG, `3P%`,`FG%` FROM PLAYER P, STAT S WHERE P.playerID = S.statPlayerID AND P.playerID =" + str_id + ";";
            stmt = con->createStatement();
            res = stmt->executeQuery(specific);
            while (res->next()) {
                string Fname = res->getString("Fname");
                string Lname = res->getString("Lname");
                double val_1 = res->getDouble("PPG");
                double val_2 = res->getDouble("APG");
                double val_3 = res->getDouble("RPG");
                double val_4 = res->getDouble("SPG");
                double val_5 = res->getDouble("BPG");
                double val_6 = res->getDouble("3P%");
                double val_7 = res->getDouble("FG%");
                cout << "First name = " << Fname << ",  Last name = " << Lname << ",  Points " << val_1 << ",  Assists " << val_2 << ",  Rebounds " << val_3 << ",  Steals " << val_4 << ",  Blocks " << val_5 << ",  3 point percentage " << val_6 << ",  Field Goal Percentage " << val_7 << endl;
            }
        }
        if (op == 4) {
            string fav_team;
            cout << " Favorite Team: ";
            cin >> fav_team;
            string specific = "SELECT P.Fname, P.Lname, S.PPG FROM PLAYER P JOIN STAT S ON P.playerID = S.statPlayerID JOIN TEAM T ON P.tname = T.teamName WHERE T.teamName = '" + fav_team + "' ORDER BY S.PPG DESC LIMIT 1; ";
            stmt = con->createStatement();
            res = stmt->executeQuery(specific);
            while (res->next()) {
                string Fname = res->getString("Fname");
                string Lname = res->getString("Lname");
                double val = res->getDouble("PPG");
                cout << "First name = " << Fname << ",  Last name = " << Lname << ",  Points " << val << endl;
            }
        }
        if (op == 5) {
            string fav_team;
            cout << " Favorite Team: ";
            cin >> fav_team;
            string specific = "SELECT a.occupancy, c.`win%`, m.experienceinRole FROM TEAM t JOIN ARENA a ON t.teamArena = a.arenaName JOIN COACH c ON t.teamCoach = c.coachId JOIN MANAGER m ON t.teamManager = m.gmId WHERE t.teamName = '" + fav_team + "';";
            stmt = con->createStatement();
            res = stmt->executeQuery(specific);
            while (res->next()) {
                int val_1 = res->getInt("occupancy");
                double val_2 = res->getDouble("win%");
                int val_3 = res->getInt("experienceinRole");
                cout << "Occupancy = " << val_1 << ", Win percentage of coach = " << val_2 << ", Experience in role of General Manager " << val_3 << endl;
            }
        }
        if (op == 6) {
            stmt = con->createStatement();
            res = stmt->executeQuery("SELECT PLAYER.Fname, PLAYER.Lname, (STAT.RPG * 5) + (STAT.APG * 3) + (STAT.PPG * 1) + (STAT.SPG * 6) + (STAT.BPG * 7) AS fantasyPoints FROM PLAYER JOIN STAT ON PLAYER.playerID = STAT.statPlayerID ORDER BY fantasyPoints DESC;");
            while (res->next()) {
                string val_1 = res->getString("Fname");
                string val_2 = res->getString("Lname");
                double val_3 = res->getInt("fantasyPoints");
                cout << "First name = " << val_1 << ", Last name = " << val_2 << "Fantasy Points = " << val_3 << endl;
            }
        }
    }
    //delete stmt;
    delete con;
    return 0;
}