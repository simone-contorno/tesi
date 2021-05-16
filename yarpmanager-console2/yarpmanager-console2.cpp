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

/* Ports */
#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

/* yarpmanager */
#include <yarp/manager/broker.h>
#include <yarp/manager/manager.h>
#include <yarp/manager/yarpbroker.h>
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

#define SIGINT 2

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcServer;

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::manager;

using yarp::os::BufferedPort;
/*
 * Start Console
 */ 
class Console {
    private:
        yarp::os::Network yarp;
        BufferedPort<Bottle> port;
        string command;
        Bottle cmd;
        Manager manager;
        Executable* exec;
        ExecutablePContainer modules;  
        ExecutablePIterator moditr;  
        CnnContainer connections;
        CnnIterator cnnitr;
        ResourcePIterator itrS;
        KnowledgeBase* kb;
        ErrorLogger* logger;
        
        string cyanColor = "\033[96m";
        string blueColor = "\033[94m";
        string yellowColor = "\033[93m";
        string greenColor = "\033[92m";
        string redColor = "\033[91m";
        string endColor = "\033[0m";

        char* szAppName = nullptr;

    public: 
        void messageConsole(string portName, int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList) {
            
            /* Opening portName */
            port.open(portName);

            cout << endl << cyanColor << "Console is running..." << endColor << endl;
            cout << endl << cyanColor << "**************************************************" << endColor << endl;
            cout << endl << "Type '" << greenColor << "help" << endColor << "' to see all avaiable commands." << endl << endl;

            int esc = 0;
            while (true) {
                getInput(appNum, appPath, appList, modNum, modPath, modList, esc);
                if (esc == 1)
                    break;
            }
        }
        
        /*
         * Getting input
         */
        void getInput(int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList, int& esc) {
            /* Variables */
            string fileName = "";
            string in = "";
            string msg = "";
            string instr[4] = {"", "", "", ""};
            bool check = false;
            int IDs = -1;
            const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};

            /* Get instruction */
            getline(cin,in);

            /* Parse instruction */
            int j = 0;
            int flag = 0;
            for (int i = 0; i < in.length(); i++) {
                if (in.at(i) != ' ') {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1) {
                for (int i = 0; j < 3 && i < in.length(); i++) {
                    if (in.at(i) == ' ') {
                        while (in.at(i) == ' ')
                            i++;
                        j++;
                    }
                    instr[j] += in.at(i);
                }
            }

            if (in != "")
                cout << endl;

            /*
            HEADER = "\033[01;35m";
            OKBLUE = "\033[34m";
            OKGREEN = "\033[32m";
            WARNING = "\033[33m";
            FAIL = "\033[31m";
            INFO = "\033[0m";
            ENDC = "\033[0m";
            */

            /*
             * Help
             */
            if (instr[0] == "help") {
                cout << greenColor << "help                     " << endColor << " : show help." << endl;
                cout << greenColor << "exit                     " << endColor << " : exit printer." << endl;
                cout << greenColor << "list mod                 " << endColor << " : list available modules." << endl;
                cout << greenColor << "list app                 " << endColor << " : list available application" << endl;
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
                cout << greenColor << "set <option> <value>     " << endColor << " : set value to an option." << endl;
                cout << greenColor << "get <option>             " << endColor << " : show value of an option." << endl;
                cout << greenColor << "export <filename>        " << endColor << " : export application's graph as Graphviz dot format." << endl;
                cout << greenColor << "show mod <modname>       " << endColor << " : display module information (description, input, output,...)." << endl;
                cout << greenColor << "assign hosts             " << endColor << " : automatically assign modules to proper nodes using load balancer." << endl;
            }        

            /*
             * exit
             */
            else if (instr[0] == "exit") {
                do {
                    cout << yellowColor << "WARNING:" << endColor << " If some modules are running or/and connections are enable, these will be closed. Are you sure? [y/n] ";
                    cin >> in;
                    if (in == "y") {
                        manager.stop();
                        manager.kill();
                        manager.disconnect();
                        port.close();
                        esc = 1;
                        cout << endl << cyanColor << "Bye. :)" << endColor << endl << endl;
                    }
                } while (in != "y" && in != "n");
            }
            
            /*
             * list mod
             */
            else if (instr[0] == "list" && instr[1] == "mod") {
                /*
                KnowledgeBase* kb = manager.getKnowledgeBase();
                ModulePContainer mods =  kb->getModules();
                int id = 0;
                // cout << mods.size() << endl;
                for (auto& mod : mods) {
                    string fname;
                    string fpath = mod->getXmlFile();
                    size_t pos = fpath.rfind(directorySeparator);

                    if(pos != string::npos)
                        fname = fpath.substr(pos);
                    else
                        fname = fpath;
                    cout<< "(" << id++ << ") " << mod->getName() 
                        << " [" << fname << "]" << endl;
                }
                */
                
                cout << "Modules list (" << modNum << "): " << endl;
                for (int i = 0; i < modList.size(); i++) {
                    if (modList[i] == "") continue;
                    cout << "(" << i << ") " << blueColor << modList[i] << endColor << endl;
                }  
            }
            
            /*
             * list app
             */
            else if (instr[0] == "list" && instr[1] == "app") {      
                /*
                KnowledgeBase* kb = manager.getKnowledgeBase();
                ApplicaitonPContainer mods =  kb->getApplications();
                int id = 0;
                // cout << mods.size() << endl;
                for (auto& mod : mods) {
                    string fname;
                    string fpath = mod->getXmlFile();
                    size_t pos = fpath.rfind(directorySeparator);

                    if(pos != string::npos)
                        fname = fpath.substr(pos);
                    else
                        fname = fpath;
                    cout<< "(" << id++ << ") " << mod->getName() 
                        << " [" << fname << "]" << endl;
                }
                */                
 
                cout << "Applications list (" << appNum << "): " << endl;
                for (int i = 0; i < appList.size(); i++) {
                    if (appList[i] == "") continue;
                    cout << "(" << i << ") " << blueColor << appList[i] << endColor << endl;
                }
            }
            
            /*
             * list res
             */
            else if (instr[0] == "list" && instr[1] == "res") {
                /*
                KnowledgeBase* kb = manager.getKnowledgeBase();
                ResourcePContainer resources = kb->getResources();
                int id = 0;
                // cout << resources.size() << endl;
                for (auto& resource : resources) {
                    auto* comp = dynamic_cast<Computer*>(resource);
                    if (comp) {
                        string fname;
                        string fpath = comp->getXmlFile();
                        size_t pos = fpath.rfind(directorySeparator);
                         
                        if(pos != string::npos)
                            fname = fpath.substr(pos);
                        else
                            fname = fpath;
                        cout << "(" << id++ << ") ";
                        if(comp->getDisable())
                            cout << comp->getName();
                        else
                            cout << comp->getName();
                        cout << " [" << fname << "]" << endl;
                    }
                }
                */
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
                /*
                fileName = instr[2];
                check = manager.addResource((resPath+fileName).c_str());
                if (check == 1)
                    cout << "Resource" << fileName << " added." << endl;
                else 
                    cout << "FAIL: Resource" << fileName << " did not add." << endl;
                */
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
            }
            
            /*
             * run 
             */
            else if (instr[0] == "run") {
                
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.run(IDs);
                    if (check == 1) {
                        cout << "IDs " << blueColor << IDs << endColor << " is running." << endl;
                        msg = yellowColor + "[YMC2-INFO]" + endColor + " Module " + to_string(IDs) + 
                            " of Application " + blueColor + string(manager.getApplicationName()) + endColor + " runned";
                        sendMessage(msg);
                    }
                    else 
                        cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " is not running" << endl; 
                }
                else {
                    manager.run();
                    cout << "Command launched." << endl;
                    msg = yellowColor + "[YMC2-INFO]" + endColor + " Modules of Application " + blueColor + string(manager.getApplicationName()) + endColor + " runned";
                    sendMessage(msg);
                }
            }

            /*
             * stop
             */
            else if (instr[0] == "stop") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.stop(IDs);
                    if (check == 1) {
                        cout << "IDs " << blueColor << IDs << endColor << " stopped." << endl;
                        msg = yellowColor + "[YMC2-INFO]" + endColor + " Module " + to_string(IDs) + 
                            " of Application " + blueColor + string(manager.getApplicationName()) + endColor + " stopped";
                        sendMessage(msg);
                    }
                    else 
                        cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not stopped." << endl;
                }
                else {
                    manager.stop();
                    cout << "Command launched." << endl;
                    msg = yellowColor + "[YMC2-INFO]" + endColor + " Modules of Application " + blueColor + string(manager.getApplicationName()) + endColor + " stopped";
                    sendMessage(msg);
                }
            }

            /*
             * kill
             */
            else if (instr[0] == "kill") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.kill(IDs);
                    if (check == 1) {
                        cout << "IDs " << blueColor << IDs << endColor << " killed." << endl;
                        msg = yellowColor + "[YMC2-INFO]" + endColor + " Module " + to_string(IDs) + 
                            " of Application " + blueColor + string(manager.getApplicationName()) + endColor + " killed";
                        sendMessage(msg);
                    }
                    else 
                        cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not killed." << endl; 
                }
                else {
                    manager.kill();
                    cout << "Command launched." << endl;
                    msg = yellowColor + "[YMC2-INFO]" + endColor + " Modules of Application " + blueColor + string(manager.getApplicationName()) + endColor + " killed";
                    sendMessage(msg);
                }
            }

            /*
             * connect
             */
            else if (instr[0] == "connect") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.connect(IDs);
                    if (check == 1) 
                        cout << "IDs " << blueColor << IDs << endColor << " connected." << endl;
                    else 
                        cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not connected." << endl; 
                }
                else {
                    manager.connect();
                    cout << "Command launched." << endl;
                }
            }

            /*
             * disconnect
             */
            else if (instr[0] == "disconnect") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.disconnect(IDs);
                    if (check == 1)
                        cout << "IDs " << blueColor << IDs << endColor << " disconnected." << endl;
                    else 
                        cout << yellowColor << "FAIL:" << endColor << " IDs " << blueColor << IDs << endColor << " did not disconnected." << endl; 
                }
                else {
                    manager.disconnect();
                    cout << "Command launched." << endl;
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
                }
                else 
                    checkConnections();
            }

            /*
             * set
             */
            else if (instr[0] == "set" && instr[1] != "" && instr[2] != "") {
                if (instr[1] == "watchdog") {
                    if (instr[2] == "yes")
                        manager.enableWatchDog();
                    else 
                        manager.disableWatchod();
                }

                if (instr[1] == "auto_dependency") {
                    if (instr[2] == "yes")
                        manager.enableAutoDependency();
                    else 
                        manager.disableAutoDependency();
                }

                if (instr[1] == "auto_connect") {
                    if (instr[2] == "yes")
                        manager.enableAutoConnect();
                    else 
                        manager.disableAutoConnect();
                }

                /*
                if (instr[1] == "color_theme") {
                    if (instr[2] == "dark")
                        // setColorTheme(THEME_DARK);
                    else if (instr[2] == "light")
                        // setColorTheme(THEME_LIGHT);
                    else
                        setColorTheme(NONE);
                }
                */
            }

            /*
             * get
             */
            else if (instr[0] == "get" && instr[1] != "") {
                /*
                if (config.check(instr[1])) {
                    cout << instr[1] << " = ";
                    cout << config.find(instr[1]).asString() << endl;
                }
                else
                    cout << "ERROR: " << "'" << instr[1].c_str() << "' not found." << endl;
                */            
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
             * show mod
             */
            else if (instr[0] == "show" && instr[1] == "mod" && instr[2] != "") {
                kb = manager.getKnowledgeBase();
                check = kb->getModule(instr[2].c_str());
                if (check == 1)
                    cout << kb->getModule(instr[2].c_str()) << endl;
                else 
                    cout << yellowColor << "FAIL: " << endColor << instr[2] << " did not find." << endl;
            }

            /*
             * assign host
             */
            else if (instr[0] == "assign" && instr[1] == "hosts") {
                manager.loadBalance();
            }

            else 
                if (flag == 1)
                    cout << "Instruction not valid, type '" << greenColor << "help" << endColor << "' for more informations about it." << endl;
    
            reportErrors();
            if (esc == 0) {
                if (in != "")
                    cout << endl << greenColor << ">> " << endColor;
                else 
                    cout << greenColor << ">> " << endColor;
            }
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
         * Getting errors
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
         * Sending message to yarpmanager-printer
         */
        void sendMessage(string msg) {
            Bottle& output = port.prepare();
            output.clear();
            output.addString(msg);
            port.write(); 
        }
};

/*
 * Initialization
 */
class Init {
    private: 
        DIR* dir;
        struct dirent* pDir;
        
    public:
        /*
         * Applications path
         */
        string getApplicationsPath() {
            string appPath;
            //appPath = "/usr/share/yarp/applications/";
        
            while (true) {
                cout << "Please supply the path to manager applications: ";
                cin >> appPath;
                if (appPath[appPath.size()-1] != '/')
                    appPath.append("/");
                if (appPath[0] != '/')
                    appPath = "/" + appPath;
                dir  = opendir(appPath.c_str());
                if (dir == NULL) {
                    cout << "\033[91m" << "ERROR:" << "\033[0m" << " directory '" << "\033[94m" << appPath << "\033[0m" << " not found." << endl;
                    continue;
                }
                else break;
            }
            cout << endl;
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
            string modPath;
            //modPath = "/usr/share/yarp/modules/";

            while (true) {
                cout << "Please supply the path to manager modules: ";
                cin >> modPath;
                if (modPath[modPath.size()-1] != '/')
                    modPath.append("/");
                if (modPath[0] != '/')
                    modPath = "/" + modPath;
                dir  = opendir(modPath.c_str());
                if (dir == NULL) {
                    cout << "\033[91m" << "ERROR:" << "\033[0m" << " directory '" << "\033[94m" << modPath << "\033[0m" << " not found." << endl;
                    continue;
                }
                else break;
            }
            cout << endl;
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
};

/* 
 * Catching CTRL+C 
 */ 
void signal_callback_handler(int sig) {
   cout << endl << "Type '" << "\033[92m" << "exit" << "\033[0m" << "' to quit. :(" << endl;
}

/*
 * Start program
 */
int main(int argc, char* argv[]) {
    
    Init objInit;
    Console objConsole;

    /* Start message */
    cout << "\033[96m" << endl;
    cout << "********************************" << endl;
    cout << "*                              *" << endl;
    cout << "*        Yarp Manager          *" << endl;   
    cout << "*    yarpmanager-console2      *" << endl;                  
    cout << "*   Author: Simone Contorno    *" << endl;
    cout << "*                              *" << endl;
    cout << "********************************" << endl;
    cout << "\033[0m" << endl;
    cout << "\033[93m" << "IMPORTANT:" << "\033[0m" << " to monitor yarpmanager-console2 open another terminal and run yarpmanager-printer." << endl << endl;
    
    /* Register signal and signal handler */
    signal(SIGINT, signal_callback_handler);

    /* Supply data */
    string portName;
    cout << "Please supply a port name for the Console: ";
    cin >> portName;
    if (portName.find("exit") != string::npos) {
        cout << endl << "\033[96m" << "Bye. :)" << "\033[0m" << endl << endl;
        return 0;
    }
    if (portName.at(0) != '/')
        portName = "/" + portName;
    cout << endl;

    /* Reading Applications path */ 
    string appPath = objInit.getApplicationsPath();

    /* Creating Applications list */
    int appNum = 0;
    vector<string> appList = objInit.getApplicationsList(appNum);
     
    /* Reading Modules path */
    string modPath = objInit.getModulesPath();
    
    /* Creating Modules list */
    int modNum = 0;
    vector<string> modList = objInit.getModulesList(modNum);
    
    /* Call Console */
    objConsole.messageConsole(portName, appNum, appPath, appList, modNum, modPath, modList);

    return 0;
}