/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

/* yarpmanager-console */
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/application.h>
#include <yarp/conf/filesystem.h>
#include <yarp/os/ResourceFinder.h>
#include <dirent.h>

/* yarpmanager */
#include <yarp/manager/manager.h>
#include <yarp/manager/execstate.h>
#include <yarp/manager/executable.h>
#include <yarp/os/Time.h>

/* Others */
#include <csignal>
#include <cstring>
#include <iostream> 
#include <filesystem>
#include <cstdio>
#include <ctime>
#include <sys/types.h>
#include <dirent.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include <cctype>
#include <algorithm>
#include<fstream>

#define SIGINT 2

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::manager;

/* Global variables */
Manager manager;
ExecutablePContainer modules;  
CnnContainer connections;
vector<bool> stateModules = {false};
vector<bool> stateConnections = {false};

string cyanColor = "\033[96m";
string blueColor = "\033[94m";
string yellowColor = "\033[93m";
string greenColor = "\033[92m";
string redColor = "\033[91m";
string endColor = "\033[0m";

string configFile = "../ymc2-config.ini";
bool configError = false;

/*
 * Start Console
 */ 
class Console {
    private:
        ExecutablePIterator moditr;  
        CnnIterator cnnitr;
        ResourcePIterator itrS;  
        ErrorLogger* logger;
        
        char* szAppName = nullptr;
        bool build = false;

    public: 
        void messageConsole(int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList,
                                    int resNum, string resPath, vector<string> resList, string anyRes) {
            
            cout << endl << cyanColor << "Console is running..." << endColor << endl;
            cout << endl << "Type '" << greenColor << "help" << endColor << "' to see all available commands." << endl << endl;
            
            if (configError == false)
                cout << greenColor << ">> " << endColor;

            int esc = 0;
            while (esc == 0) {
                getInput(appNum, appPath, appList, modNum, modPath, modList, resNum, resPath, resList, esc);
            }
        }
        
        /*
         * Get input
         */
        void getInput(int appNum, string appPath, vector<string> appList,
                            int modNum, string modPath, vector<string> modList, 
                                int resNum, string resPath, vector<string> resList, int& esc) {
            /* Variables */
            string in = "";
            string fileName = "";
            string instr[4] = {"", "", "", ""};
            int IDs = -1;
            bool check = false;

            const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};

            /* Build stateModules and stateConnections */
            if (build == true) {
                stateModules = {false};
                modules = manager.getExecutables();

                /* stateModules */
                for (int id = 0; id < modules.size(); id++) {
                    if (manager.running(id)) {
                        if (id == 0) 
                            stateModules[id] = true;
                        else 
                            stateModules.push_back(true);
                    }
                    else {
                        if (id == 0) 
                            stateModules[id] = false;
                        else 
                            stateModules.push_back(false);
                    }
                }

                /* stateConnections */
                for (int id = 0; id < connections.size(); id++) {
                    if (manager.connected(id)) {
                        if (id == 0) 
                            stateConnections[id] = true;
                        else 
                            stateConnections.push_back(true);
                    }
                    else {
                        if (id == 0) 
                            stateConnections[id] = false;
                        else 
                            stateConnections.push_back(false);
                    }
                }

                build = false;
            }

            /* Get instruction */
            getline(cin,in);

            /* 
             * Parse instruction 
             */
            int j = 0;
            int inNotNull = 0;
            
            /* Check if string is != null */ 
            for (int i = 0; i < in.length(); i++) {
                if (in.at(i) != ' ') {
                    inNotNull = 1;
                    break;
                }
            }

            if (inNotNull == 1) {
                for (int i = 0; j < 3 && i < in.length(); i++) {
                    if (in.at(i) == ' ') {
                        while (i < in.length() && in.at(i) == ' ')
                            i++;
                        j++;
                    }
                    if (i < in.length())
                        instr[j] += in.at(i);
                }
            }

            if (in != "")
                cout << endl;

            /*
             * Help
             */
            if (instr[0] == "help") {
                cout << greenColor << "help                     " << endColor << " : show help." << endl;
                cout << greenColor << "exit                     " << endColor << " : exit printer." << endl;
                cout << greenColor << "list mod                 " << endColor << " : list available modules." << endl;
                cout << greenColor << "list app                 " << endColor << " : list available applications." << endl;
                cout << greenColor << "list res                 " << endColor << " : list available resources." << endl;
                cout << greenColor << "add mod <filename>       " << endColor << " : add a module from its description file." << endl;
                cout << greenColor << "add app <filename>       " << endColor << " : add an application from its description file." << endl;
                cout << greenColor << "add res <filename>       " << endColor << " : add resources from a description file." << endl;
                cout << greenColor << "load app <application>   " << endColor << " : load an application to run." << endl;
                cout << greenColor << "run [IDs]                " << endColor << " : run application or a modules indicated by IDs." << endl;
                cout << greenColor << "stop [IDs]               " << endColor << " : stop running application or modules indicated by IDs." << endl;
                cout << greenColor << "kill [IDs]               " << endColor << " : kill running application or modules indicated by IDs." << endl;
                cout << greenColor << "connect [IDs]            " << endColor << " : stablish all connections or just one connection indicated by IDs." << endl;
                cout << greenColor << "disconnect [IDs]         " << endColor << " : remove all connections or just one connection indicated by IDs." << endl;
                cout << greenColor << "which                    " << endColor << " : list loaded modules, connection and resource dependencies." << endl;
                cout << greenColor << "check dep                " << endColor << " : check for all resource dependencies." << endl;
                cout << greenColor << "check state [id]         " << endColor << " : check for running state of application or a module indicated by id." << endl;
                cout << greenColor << "check con [id]           " << endColor << " : check for all connections state or just one connection indicated by id." << endl;
                cout << greenColor << "set <option> <value>     " << endColor << " : set value to an option. To see all available options type 'set --help'." << endl;
                cout << greenColor << "export <filename>        " << endColor << " : export application's graph as Graphviz dot format." << endl;
                cout << greenColor << "assign hosts             " << endColor << " : automatically assign modules to proper nodes using load balancer." << endl;
            }        

            /*
             * exit
             */
            else if (instr[0] == "exit") {
                do {
                    cout << yellowColor << "WARNING:" << endColor << " If some modules are running or/and connections are enable, these will be close. Are you sure? [y/n] ";
                    cin >> in;
                    
                    if (in == "y") {
                        manager.stop();
                        manager.kill();
                        manager.disconnect();
                        esc = 1;
                        cout << endl << cyanColor << "Bye. :)" << endColor << endl << endl;
                        return;
                    }

                    else if (in == "n") {
                        cout << endl;
                        return;
                    }

                    else 
                        cout << "Please type 'y' or 'n' to answer." << endl;
                } while (in != "y" && in != "n");
            }
            
            /*
             * list mod
             */
            else if (instr[0] == "list" && instr[1] == "mod") {                
                cout << "Modules list (" << modNum << "): " << endl;

                for (int i = 0; i < modList.size(); i++) {
                    if (modList[i] == "") 
                        continue;
                    cout << "(" << i << ") " << blueColor << modList[i] << endColor << endl;
                }  
            }
            
            /*
             * list app
             */
            else if (instr[0] == "list" && instr[1] == "app") {      
                cout << "Applications list (" << appNum << "): " << endl;
                
                for (int i = 0; i < appList.size(); i++) {
                    if (appList[i] == "") 
                        continue;
                    cout << "(" << i << ") " << blueColor << appList[i] << endColor << endl;
                }
            }
            
            /*
             * list res
             */
            else if (instr[0] == "list" && instr[1] == "res") {
                cout << "Resources list (" << resNum << "): " << endl;
                
                for (int i = 0; i < resList.size(); i++) {
                    if (resList[i] == "") 
                        continue;
                    cout << "(" << i << ") " << blueColor << resList[i] << endColor << endl;
                }
            }

            /*
             * add mod
             */
            else if (instr[0] == "add" && instr[1] == "mod" && instr[2] != "") {
                fileName = instr[2];
                check = manager.addModule((modPath+fileName).c_str());
                
                if (check == 1)
                    cout << "Module " << blueColor << fileName << endColor << " added." << endl;
                else 
                    cout << yellowColor << "FAIL:" << endColor << " Module " << blueColor << fileName << endColor << " did not add." << endl;
            }

            /*
             * add app
             */ 
            else if (instr[0] == "add" && instr[1] == "app" && instr[2] != "") {
                fileName = instr[2];
                check = manager.addApplication((appPath+fileName).c_str(), &szAppName, true);

                if (check == 1)
                    cout << "Application " << blueColor << fileName << endColor << " added." << endl;
                else 
                    cout << yellowColor << "FAIL:" << endColor << " Application " << blueColor << fileName << endColor << " did not add." << endl;
            }

            /*
             * add res
             */
            else if (instr[0] == "add" && instr[1] == "res" && instr[2] != "") {
                fileName = instr[2];
                check = manager.addResource((resPath+fileName).c_str());

                if (check == 1)
                    cout << "Resources " << blueColor << fileName << endColor << " added." << endl;
                else 
                    cout << yellowColor << "FAIL:" << endColor << " Resources " << blueColor << fileName << endColor << " did not add." << endl;
            }
            
            /*
             * load app
             */ 
            else if (instr[0] == "load" && instr[1] == "app") {
                fileName = instr[2];
                check = manager.loadApplication(szAppName);

                if (check == 1)
                    cout << "Application " << blueColor << fileName << endColor << " loaded." << endl;
                else 
                    cout << yellowColor << "FAIL:" << endColor << " Application " << blueColor << fileName << endColor << " did not load." << endl;
                
                build = true;
            }
            
            /*
             * run 
             */
            else if (instr[0] == "run") {       
                if (instr[1] != "") {
                    try {
                        IDs = stoi(instr[1]);
                        check = manager.run(IDs);

                        if (check == 1) {
                            cout << "IDs " << blueColor << IDs << endColor << " is running." << endl;
                            stateModules[IDs] = true;
                        }

                        else 
                            cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " is not running" << endl; 
                        
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " check ID number." << endl;
                    }
                }
                
                else {
                    manager.run();
                    cout << "Command launched." << endl;

                    for (int id = 0; id < stateModules.size(); id++) 
                        stateModules[id] = true;
                }
            }

            /*
             * stop
             */
            else if (instr[0] == "stop") {
                if (instr[1] != "") {
                    try {
                        IDs = stoi(instr[1]);
                        check = manager.stop(IDs);

                        if (check == 1) {
                            cout << "IDs " << blueColor << IDs << endColor << " stopped." << endl;
                            stateModules[IDs] = false;
                        }

                        else 
                            cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not stop." << endl;  
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " check ID number." << endl;
                    }
                }

                else {
                    manager.stop();
                    cout << "Command launched." << endl;

                    for (int id = 0; id < stateModules.size(); id++) 
                        stateModules[id] = false;
                }
            }

            /*
             * kill
             */
            else if (instr[0] == "kill") {
                if (instr[1] != "") {
                    try {
                        IDs = stoi(instr[1]);
                        check = manager.kill(IDs);

                        if (check == 1) {
                            cout << "IDs " << blueColor << IDs << endColor << " killed." << endl;
                            stateModules[IDs] = false;
                        }

                        else 
                            cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not kill." << endl; 
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " check ID number." << endl;
                    }
                }

                else {
                    manager.kill();
                    cout << "Command launched." << endl;

                    for (int id = 0; id < stateModules.size(); id++) 
                        stateModules[id] = false;
                }
            }

            /*
             * connect
             */
            else if (instr[0] == "connect") {
                if (instr[1] != "") {
                    try {
                        IDs = stoi(instr[1]);
                        check = manager.connect(IDs);

                        if (check == 1) {
                            cout << "IDs " << blueColor << IDs << endColor << " connected." << endl;
                            stateConnections[IDs] = true;
                        }

                        else 
                            cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not connect." << endl; 
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " check ID number." << endl;
                    }
                }

                else {
                    manager.connect();
                    cout << "Command launched." << endl;

                    for (int id = 0; id < stateConnections.size(); id++) 
                        stateConnections[id] = true;
                }
            }

            /*
             * disconnect
             */
            else if (instr[0] == "disconnect") {
                if (instr[1] != "") {
                    try {
                        IDs = stoi(instr[1]);
                        check = manager.disconnect(IDs);

                        if (check == 1) {
                            cout << "IDs " << blueColor << IDs << endColor << " disconnected." << endl;
                            stateConnections[IDs] = false;
                        }

                        else 
                            cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not disconnect." << endl; 
                        
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " ID must be a number." << endl;
                    }
                }

                else {
                    manager.disconnect();
                    cout << "Command launched." << endl;

                    for (int id = 0; id < stateConnections.size(); id++) 
                        stateConnections[id] = false;
                }
            }

            /*
             * which
             */
            else if (instr[0] == "which") {
                which();
            }

            /*
             * check dep
             */
            else if (instr[0] == "check" && instr[1] == "dep") {
                if (manager.checkDependency())
                    cout << "All of resource dependencies are satisfied." << endl;
            }
            
            
            /*
             * check state 
             */
            else if (instr[0] == "check" && instr[1] == "state") {
                modules = manager.getExecutables();
                
                if (instr[2] != "") {
                    try {
                        IDs = stoi(instr[2]);

                        if (IDs >= modules.size())
                            cout << redColor << "ERROR:" << endColor << " Module " << blueColor << IDs << endColor << " is out of range." << endl;

                        else {
                            if (manager.running(IDs)) 
                                cout << greenColor << "<RUNNING>" << endColor << endl;
                            else 
                                cout << redColor << "<STOPPED>" << endColor << endl;
                            cout << blueColor << "(" << IDs << ")" << endColor << " [" << modules[IDs]->getHost() << "]" << endl;
                        }
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " ID must be a number." << endl;
                    }
                }

                else 
                    checkStates();
            }

            /*
             * check con
             */
            else if (instr[0] == "check" && instr[1] == "con") {
                connections = manager.getConnections();

                if (instr[2] != "") {
                    try {
                        IDs = stoi(instr[2]);

                        if (IDs >= connections.size())
                            cout << redColor << "ERROR:" << endColor << " Connection " << blueColor << IDs << endColor << " is out of range." << endl;
                        
                        else {
                            if (manager.connected(IDs)) 
                                cout << greenColor << "<CONNECTED>" << endColor << endl;
                            else 
                                cout << redColor << "<DISCONNECTED>" << endColor << endl;
                            cout << "(" << IDs << ") " << blueColor << connections[IDs].from() << " - " 
                                << connections[IDs].to() << endColor << " [" << connections[IDs].carrier() << "]" << endl;
                        }
                    } catch (exception e) {
                        cout << yellowColor << "WARNING:" << endColor << " ID must be a number." << endl;
                    }
                }

                else 
                    checkConnections();
            }

            /*
             * set
             */
            else if (instr[0] == "set" && instr[1] != "") {
                if (instr[1] == "watchdog" && instr[2] != "") {
                    if (instr[2] == "yes") {
                        manager.enableWatchDog();
                        cout << "Watchdog enabled." << endl;
                    }

                    else if (instr[2] == "no") {
                        manager.disableWatchod();
                        cout << "Watchdog disabled." << endl;
                    }

                    else 
                        cout << "Please insert '" << greenColor << "yes" << endColor << "' or '" 
                            << greenColor << "no" << endColor << "' as value for this option." << endl;
                }

                else if (instr[1] == "auto_dependency" && instr[2] != "") {
                    if (instr[2] == "yes") {
                        manager.enableAutoDependency();
                        cout << "Auto dependency enabled." << endl;
                    }

                    else if (instr[2] == "no") {
                        manager.disableAutoDependency();
                        cout << "Auto dependency disabled." << endl;
                    }
                    
                    else 
                        cout << "Please insert '" << greenColor << "yes" << endColor << "' or '" 
                            << greenColor << "no" << endColor << "' as value for this option." << endl;
                }

                else if (instr[1] == "auto_connect" && instr[2] != "") {
                    if (instr[2] == "yes") {
                        manager.enableAutoConnect();
                        cout << "Auto connect enabled." << endl;
                    }

                    else if (instr[2] == "no") {
                        manager.disableAutoConnect();
                        cout << "Auto connect disabled." << endl;
                    }

                    else 
                        cout << "Please insert '" << greenColor << "yes" << endColor << "' or '" 
                            << greenColor << "no" << endColor << "' as value for this option." << endl;
                }

                else if (instr[1] == "color_theme" && instr[2] != "") {
                    if (instr[2] == "dark") {
                        cyanColor = "\033[36m";
                        blueColor = "\033[34m";
                        yellowColor = "\033[33m";
                        greenColor = "\033[32m";
                        redColor = "\033[31m";
                        cout << "Dark theme selected." << endl;
                    }

                    else if (instr[2] == "light") {
                        cyanColor = "\033[96m";
                        blueColor = "\033[94m";
                        yellowColor = "\033[93m";
                        greenColor = "\033[92m";
                        redColor = "\033[91m";
                        cout << "Light theme selected." << endl;
                    }

                    else if (instr[2] == "none") {
                        cyanColor = "\033[0m";
                        blueColor = "\033[0m";
                        yellowColor = "\033[0m";
                        greenColor = "\033[0m";
                        redColor = "\033[0m";
                        cout << "None theme selected." << endl;
                    }

                    else 
                        cout << "Please insert a valid color: '" << greenColor << "dark" << endColor 
                                << "', '" << greenColor << "light" << endColor << "' or '" << greenColor << "none" << endColor << "'." << endl;
                }

                else if (instr[1] == "--help") {
                    cout << greenColor << "--help                   " << endColor << " : show this help." << endl;
                    cout << greenColor << "whatchdog                " << endColor << " : enable watchdog with 'yes' value or disable with 'no' value." << endl;
                    cout << greenColor << "auto_dependency          " << endColor << " : enable auto dependency with 'yes' value or disable with 'no' value." << endl;
                    cout << greenColor << "auto_connect             " << endColor << " : enable auto connection with 'yes' value or disable with 'no' value." << endl;
                    cout << greenColor << "color_theme              " << endColor << " : set 'dark', 'light' or 'none' color theme." << endl;
                }

                else 
                    cout << "Please select a valid option. Digit '" << greenColor << "set --help" << endColor << "' for more details." << endl;
            }

            /*
             * export
             */
            else if (instr[0] == "export" && instr[1] != "") {
                check = manager.exportDependencyGraph(instr[1].c_str());

                if (check == 1)
                    cout << "Graph exported." << endl;
                else 
                    cout << yellowColor << "FAIL:" << endColor << " Cannot export graph to " << instr[1] << endl;
            }

            /*
             * assign host
             */
            else if (instr[0] == "assign" && instr[1] == "hosts") {
                manager.loadBalance();
            }

            else 
                if (inNotNull == 1)
                    cout << "Instruction not valid, type '" << greenColor << "help" << endColor << "' for more details." << endl;
    
            reportErrors();
            if (in != "")
                cout << endl;
            cout << greenColor << ">> " << endColor;
        }

        /*
         * which
         */
        void which() {
            modules = manager.getExecutables();
            connections  = manager.getConnections();

            cout << "Application: " << endl;
            cout << blueColor << manager.getApplicationName() << endColor << endl;

            cout << endl << "Modules: " << endl;
            int id = 0;
            for (moditr = modules.begin(); moditr < modules.end(); moditr++) {
                cout << "(" << id++ << ") " << blueColor << (*moditr)->getCommand() << endColor;
                cout << " [" << (*moditr)->getHost() << "] [" << (*moditr)->getParam() << "]";
                cout << " [" << (*moditr)->getEnv() << "]" << endl;
            }

            cout << endl << "Connections: " << endl;
            id = 0;
            for (cnnitr = connections.begin(); cnnitr < connections.end(); cnnitr++) {
                cout << "(" << id++ << ") ";
                cout << blueColor << (*cnnitr).from() << " - " << (*cnnitr).to() << endColor;
                    cout<<" [" << (*cnnitr).carrier() << "]";
                cout << endl;
            }

            cout << endl << "Resources:" << endl;
            id = 0;
            for (itrS = manager.getResources().begin(); itrS != manager.getResources().end(); itrS++) {
                cout << "(" << id++ << ") ";
                cout << blueColor << (*itrS)->getName() << endColor << " [" << (*itrS)->getTypeName() << "]" << endl;
            }
        }

        /*
         * Check executables state
         */
        void checkStates() {
            modules = manager.getExecutables();
            unsigned int id = 0;
            bool bShouldRun = false;

            for (moditr = modules.begin(); moditr < modules.end(); moditr++) {
                if (manager.running(id)) {
                    bShouldRun = true;
                    cout << greenColor << "<RUNNING> " << endColor;
                }

                else
                    cout << redColor << "<STOPPED> " << endColor;

                cout << "(" << id << ") " << blueColor << (*moditr)->getCommand() << endColor;
                cout << " [" << (*moditr)->getHost() << "]" << endl;
                id++;
            }
        }
        
        /*
         * Check connections
         */ 
        void checkConnections() {
            connections = manager.getConnections();
            int id = 0;

            for (cnnitr = connections.begin(); cnnitr < connections.end(); cnnitr++) {
                if (manager.connected(id))
                    cout << greenColor << "<CONNECTED> " << endColor;
                else
                    cout << redColor << "<DISCONNECTED> " << endColor;

                cout << "(" << id << ") " << blueColor << (*cnnitr).from() << " - " << (*cnnitr).to();
                cout << endColor << " [" << (*cnnitr).carrier() << "]" << endl;
                id++;
            }
        }

        /*
         * Get errors
         */
        void reportErrors() {
            logger  = ErrorLogger::Instance();

            if(logger->errorCount() || logger->warningCount()) {
                const char* msg;

                while ((msg = logger->getLastError()))
                    cout << redColor << "ERROR: " << endColor << msg << endl;

                while ((msg = logger->getLastWarning()))
                    cout << yellowColor << "WARNING: " << endColor << msg << endl;
            }
        }

        /*
         * Get Time
         */
        string getTime() {
            time_t t = time(0);
            tm* temp = localtime(&t);
            string hour = to_string(temp->tm_hour);
            string min = to_string(temp->tm_min);
            string sec = to_string(temp->tm_sec);

            if (hour.length() == 1) hour = "0" + hour;
            if (min.length() == 1) min = "0" + min;
            if (sec.length() == 1) sec = "0" + sec;

            return hour + ":" + min + ":" + sec;
        }
};

/*
 * Initialization
 */
class Init {
    private: 
        DIR* dir = NULL;
        struct dirent* pDir;
        string line = "";

    public:
        /*
         * Applications path
         */
        string getApplicationsPath() {
            string appPath = "";

            /* If there is the config file */
            if (fopen(configFile.c_str(), "r")) {
                ifstream fin(configFile);

                if (fin.good()) {
                    while (!fin.eof()) {
                        getline(fin, line);

                        if (line.find("appPath") != string::npos) {
                            for (int i = 0; i < line.length(); i++) {
                                if (line.at(i) == char(34)) {
                                    i++;

                                    while (line.at(i) != char(34)) {
                                        appPath += line.at(i);
                                        i++;
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    /* Clearing white spaces */
                    string temp = "";

                    for (int i = 0; i < appPath.size(); i++) {
                        if (appPath.at(i) != ' ')
                            temp += appPath.at(i);
                    }

                    appPath = temp;

                    /* Check empty path */
                    if (appPath == "") 
                        dir = NULL;            

                    else {
                        if (appPath[appPath.size()-1] != '/')
                            appPath.append("/");
                        if (appPath[0] != '/')
                            appPath = "/" + appPath;
                        
                        dir  = opendir(appPath.c_str());
                        if (dir == NULL) {
                            cout << redColor << "ERROR:" << endColor << " path of applications in the config file did not find." << endl;
                            configError = true;
                        }
                    }
                }
            }
           
            /* If there is NOT the config file */
            if (!fopen(configFile.c_str(), "r") || dir == NULL) {
                while (true) {
                    cout << "Please supply the path to manager applications: ";
                    cin >> appPath;

                    if (appPath[appPath.size()-1] != '/')
                        appPath.append("/");
                    if (appPath[0] != '/')
                        appPath = "/" + appPath;

                    dir  = opendir(appPath.c_str());
                    if (dir == NULL) {
                        cout << redColor << "ERROR:" << endColor << " directory '" << blueColor << appPath << endColor << "' not found." << endl;
                        continue;
                    }
                    else break;
                }
                cout << endl;
            }

            return appPath;
        }
        
        /*
        * Applications list
        */
        vector<string> getApplicationsList(int& appNum) {
            vector<string> appList = {""};

            while ((pDir = readdir(dir)) != NULL) {
                if (string(pDir->d_name).find(".xml") == string::npos)
                    continue; 
                if (appList[0] == "")
                    appList[0] = string(pDir->d_name);
                else 
                    appList.push_back(string(pDir->d_name));

                appNum++;
            } 

            return appList;
        }

        /* 
         * Modules path 
         */
        string getModulesPath() {
            string modPath = "";
            
            /* If there is the config file */
            if (fopen(configFile.c_str(), "r")) {
                ifstream fin(configFile);

                if (fin.good()) {
                    while (!fin.eof()) {
                        getline(fin, line);

                        if (line.find("modPath") != string::npos) {
                            for (int i = 0; i < line.length(); i++) {
                                if (line.at(i) == char(34)) {
                                    i++;

                                    while (line.at(i) != char(34)) {
                                        modPath += line.at(i);
                                        i++;
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    /* Clearing white spaces */
                    string temp = "";

                    for (int i = 0; i < modPath.size(); i++) {
                        if (modPath.at(i) != ' ')
                            temp += modPath.at(i);
                    }

                    modPath = temp;
             
                    /* Check empty path */
                    if (modPath == "") 
                        dir = NULL;

                    else {
                        if (modPath[modPath.size()-1] != '/')
                            modPath.append("/");
                        if (modPath[0] != '/')
                            modPath = "/" + modPath;
                        
                        dir  = opendir(modPath.c_str());
                        if (dir == NULL) {
                            cout << redColor << "ERROR:" << endColor << " path of modules in the config file did not find." << endl;
                            configError = true;
                        }
                    }
                }
            }
            
            /* If there is NOT the config file */
            if (!fopen(configFile.c_str(), "r") || dir == NULL) {
                while (true) {
                    cout << "Please supply the path to manager modules: ";
                    cin >> modPath;

                    if (modPath[modPath.size()-1] != '/')
                        modPath.append("/");
                    if (modPath[0] != '/')
                        modPath = "/" + modPath;

                    dir  = opendir(modPath.c_str());
                    if (dir == NULL) {
                        cout << redColor << "ERROR:" << endColor << " directory '" << blueColor << modPath << endColor << "' not found." << endl;
                        continue;
                    }
                    else break;
                }
                cout << endl;
            }

            return modPath;
        }

        /*
        * Modules list
        */
        vector<string> getModulesList(int& modNum) {
            vector<string> modList = {""};

            while ((pDir = readdir(dir)) != NULL) {
                if (string(pDir->d_name).find(".xml") == string::npos)
                    continue; 
                if (modList[0] == "")
                    modList[0] = string(pDir->d_name);
                else 
                    modList.push_back(string(pDir->d_name));
                modNum++;
            }      

            return modList;
        }

        /* 
         * Resources path 
         */
        string getResourcesPath() {
            string resPath = "";

            /* If there is the config file */
            if (fopen(configFile.c_str(), "r")) {
                ifstream fin(configFile);

                if (fin.good()) {
                    while (!fin.eof()) {
                        getline(fin, line);
                        
                        if (line.find("resPath") != string::npos) {
                            for (int i = 0; i < line.length(); i++) {
                                if (line.at(i) == char(34)) {
                                    i++;

                                    while (line.at(i) != char(34)) {
                                        resPath += line.at(i);
                                        i++;
                                    }
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    /* Clearing white spaces */
                    string temp = "";

                    for (int i = 0; i < resPath.size(); i++) {
                        if (resPath.at(i) != ' ')
                            temp += resPath.at(i);
                    }

                    resPath = temp;
                    
                    /* Check empty path */
                    if (resPath == "") 
                        dir = NULL;
                    
                    else {
                        if (resPath[resPath.size()-1] != '/')
                            resPath.append("/");
                        if (resPath[0] != '/')
                            resPath = "/" + resPath;
                        
                        dir  = opendir(resPath.c_str());
                        if (dir == NULL) {
                            cout << redColor << "ERROR:" << endColor << " path of resources in the config file did not find." << endl;
                            configError = true;
                        }
                    }
                }
            }
            
            /* If there is NOT the config file */
            if (!fopen(configFile.c_str(), "r") || dir == NULL) {
                while (true) {
                    cout << "Please supply the path to manager resources: ";
                    cin >> resPath;

                    if (resPath[resPath.size()-1] != '/')
                        resPath.append("/");
                    if (resPath[0] != '/')
                        resPath = "/" + resPath;

                    dir  = opendir(resPath.c_str());
                    if (dir == NULL) {
                        cout << redColor << "ERROR:" << endColor << " directory '" << blueColor << resPath << endColor << "' not found." << endl;
                        continue;
                    }
                    else break;
                }
                cout << endl;
            }

            return resPath;
        }

        /*
        * Resources list
        */
        vector<string> getResourcesList(int& resNum) {
            vector<string> resList = {""};

            while ((pDir = readdir(dir)) != NULL) {
                if (string(pDir->d_name).find(".xml") == string::npos)
                    continue; 
                if (resList[0] == "")
                    resList[0] = string(pDir->d_name);
                else 
                    resList.push_back(string(pDir->d_name));
                resNum++;
            }    

            return resList;
        }
};

/*
 * Check status            
 */
void checkStatus() {
    string line = "";
    string isANumber = "";

    Console obj;

    bool isValid = false;
    int sleepTimer = 5000;

    /* Check configuration file to set sleepTimer */
    if (fopen(configFile.c_str(), "r")) {
        ifstream fin(configFile);

        if (fin.good()) {
            while (!fin.eof()) {
                getline(fin, line);

                if (line.find("sleepTimer") != string::npos) {
                    for (int i = 0; i < line.length(); i++) {
                        if (line.at(i) == char(34)) {
                            i++;

                            while (line.at(i) != char(34)) {
                                isANumber += line.at(i);
                                i++;
                            }
                            break;
                        }
                    }
                    break;
                }
            }

            /* Clearing white spaces */
            string temp = "";

            for (int i = 0; i < isANumber.size(); i++) {
                if (isANumber.at(i) != ' ')
                    temp += isANumber.at(i);
            }

            isANumber = temp;
                    
            /* Check empty path */
            if (isANumber == "") 
                isValid = false;
            
            try {
                sleepTimer = stoi(isANumber);
                isValid = true;
            } catch (exception e) {
                isValid = false;
            }
        }
    }
            
    /* If there is NOT the config file */
    if (isValid == false || sleepTimer < 0) 
        sleepTimer = 5000;
    
    while (true) {
        this_thread::sleep_for(chrono::milliseconds(sleepTimer)); // default: 5 seconds
        modules = manager.getExecutables();
        connections = manager.getConnections();
        bool bShouldRun = false;
        bool bShoulConnect = false;

        /* Check Modules */
        for (int id = 0; id < modules.size(); id++) {
            if (manager.running(id)) 
                bShouldRun = true;
            else
                bShouldRun = false;

            if (stateModules[id] != bShouldRun) {
                if (bShouldRun == false) {
                    cout << endl << redColor << "ERROR: " << endColor << "Module " << id 
                        << " of Application " << blueColor + string(manager.getApplicationName()) << endColor 
                        << " terminated unexpectedly at: " << blueColor << obj.getTime() << endColor << endl;

                    stateModules[id] = false;
                }
            }          
        }

        /* Check Connections */
        for (int id = 0; id < connections.size(); id++) {
            if (manager.connected(id)) 
                bShoulConnect = true;
            else
                bShoulConnect = false;

            if (stateConnections[id] != bShoulConnect) {
                if (bShoulConnect == false) {
                    cout << endl << redColor << "ERROR: " << endColor << "Connection " << id 
                        << " of Application " << blueColor + string(manager.getApplicationName()) << endColor 
                        << " disconnected at: " << blueColor << obj.getTime() << endColor << endl;
                    
                    stateConnections[id] = false;
                }
            }          
        }
    }
}

/* 
 * Catch CTRL+C 
 */ 
void signal_callback_handler(int sig) {
   cout << endl << "Type '" << greenColor << "exit" << endColor << "' to quit. " << cyanColor << ":(" << endColor << endl;
}

/*
 * Start program
 */
int main(int argc, char* argv[]) {
    
    Init objInit;
    Console objConsole;

    /* Start message */
    cout << cyanColor << endl;

    cout << "__   __                                           ____" << endl;
    cout << char(92) << " " << char(92) << " / / __ ___   __ _ _ __   __ _  __ _  ___ _ __/ _  " << char(92) << endl; 
    cout << " " << char(92) << " V / '_ ` _ " << char(92) << " / _` | '_ " << char(92) << " / _` |/ _` |/ _ " << char(92) << " '__|/ | |" << endl;
    cout << "  | || | | | | | (_| | | | | (_| | (_| |  __/ |    / /_" << endl;
    cout << "  |_||_| |_| |_|" << char(92) << "__,_|_| |_|" << char(92) << "__,_|" << char(92) << "__, |" << char(92) << "___|_|   |____|" << endl;
    cout << "                                  |___/" << endl;

    cout << endl;

    cout << " ___________________________" << endl;
    cout << "|                           |" << endl;     
    cout << "|  Author: Simone Contorno  |" << endl;
    cout << "|___________________________|" << endl; 

    cout << endColor << endl << endl;

    /* Read Applications path */ 
    string appPath = objInit.getApplicationsPath();

    /* Create Applications list */
    int appNum = 0;
    vector<string> appList = objInit.getApplicationsList(appNum);
     
    /* Read Modules path */
    string modPath = objInit.getModulesPath();
    
    /* Create Modules list */
    int modNum = 0;
    vector<string> modList = objInit.getModulesList(modNum);

    string anyRes = "";
    string resPath = "";
    vector<string> resList = {""};
    int resNum = 0;

    /* Check config file to anyRes */
    if (fopen(configFile.c_str(), "r")) {
        ifstream fin(configFile);
        string line = "";

        if (fin.good()) {
            while (!fin.eof()) {
                getline(fin, line);
             
                if (line.find("anyRes") != string::npos) {
                    for (int i = 0; i < line.length(); i++) {
                        if (line.at(i) == char(34)) {
                            i++;

                            while (line.at(i) != char(34)) {
                                anyRes += line.at(i);
                                i++;
                            }
                            break;
                        }
                    }
                    break;
                }
            }

            /* Clearing white spaces */
            string temp = "";

            for (int i = 0; i < anyRes.size(); i++) {
                if (anyRes.at(i) != ' ')
                    temp += anyRes.at(i);
            }

            anyRes = temp;
        }
    }
    
    do {
        if (anyRes != "y" && anyRes != "n") {
            configError = true;
            cout << "Do you have any resources? [y/n] ";
            cin >> anyRes;
        }

        if (anyRes == "y") {
            /* Reading Resources path */
            resPath = objInit.getResourcesPath();
            
            /* Creating Resources list */
            resNum = 0;
            resList = objInit.getResourcesList(resNum);
        }

        else if (anyRes != "y" && anyRes != "n")
            cout << "Please insert '" << greenColor << "y" << endColor 
                << "' or '" << greenColor << "n" << endColor << "'." << endl;
    } while (anyRes != "y" && anyRes != "n");

    /* Register signal and signal handler */
    signal(SIGINT, signal_callback_handler);

    /* Start thread */
    thread t(checkStatus);
    t.detach();

    /* Call Console */
    objConsole.messageConsole(appNum, appPath, appList, modNum, modPath, modList, resNum, resPath, resList, anyRes);

    return 0;
}