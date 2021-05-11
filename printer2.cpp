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

using yarp::os::Bottle;
using yarp::os::Network;
using yarp::os::RpcServer;

using namespace std;
using namespace yarp::os;
using namespace yarp::os::impl;
using namespace yarp::manager;

/*
 * Start Printer
 */ 
class Printer {
    private:
        yarp::os::Network yarp;
        RpcServer port;
        string command;
        Bottle cmd;
        Manager manager;
        Executable* exec;
        ExecutablePContainer modules;    

        char* szAppName = nullptr;

    public: 
        void messagePrinter(string portName, int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList) {
            
            /* Opening port_name and connect with /root */
            port.open(portName);
            //yarp.connect("/root",portName);
            command = "yarp connect /root " + portName;
            system(command.c_str());
            
            cout << "Message Printer is running..." << endl; 
            cout << endl << "**************************************************" << endl << endl;
        
            cout << "Type 'help' to see all avaiable commands." << endl;
            while (true) {
                getInput(appNum, appPath, appList, modNum, modPath, modList);
            }

            cout << endl << "**************************************************" << endl << endl;

            //command = "gnome-terminal";
            //system(command.c_str());
            while (true) {
                /* Get time */ 
                string current_time = getTime();
                
                /* Get connection name */ 
                port.read(cmd,true);
                string conn_name = getProcessName(cmd);
                
                /* Print message */
                if (cmd.toString().find("[add]") == 0 ||
                        cmd.toString().find("[del]") == 0) {
                    for (int i = 0; i < modules.size(); i++) {
                        manager.getExecutableById(i);
                        /*
                        check = manager.running(i);
                        if (check == true)
                            cout << "Application running" << endl;
                        else 
                            cout << "Application not running" << endl;
                        */
                        execState(exec, i);
                    }
                }
                if (cmd.toString().find("[add]") == 0) {
                    cout << "[+] Connection '" << conn_name << "' opened at: " << current_time << endl;
                }
                if (cmd.toString().find("[del]") == 0) {
                    cout << "[-] Connection '" << conn_name << "' closed at: " << current_time << endl;
                }
                cout << endl;
            }
            port.close();
        }
        
        /*
         * Getting Time
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

        /*
         * Getting Connection name
         */
        string getProcessName(Bottle cmd) {
            string conn_name = "";
            for (int i=0; i<cmd.toString().length(); i++) {
                if(cmd.toString().at(i) == 34) {
                    for (int j=i+1; cmd.toString().at(j) != 34; j++) {
                        conn_name += cmd.toString().at(j);
                        i = j+1;
                    }
                }
            }   
            return conn_name;
        }

        /*
         * Getting Executable State
         */
        void execState(Executable* exec, int i) {
            cout << "Executable state by id '" << i << "': ";
            switch(exec->state()) {
                case 0: cout << "SUSPENDED"; break;
                case 1: cout << "READY"; break;
                case 2: cout << "CONNECTING"; break;
                case 3: cout << "RUNNING"; break;
                case 4: cout << "DYING"; break;
                case 5: cout << "DEAD"; break;
                case 6: cout << "STUNKNOW"; break;
            }
            cout << endl;
        }

        /*
         * Getting input
         */
        void getInput(int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList) {
            string fileName = "";
            bool check = false;
            int IDs = -1;
            string in = "";
            
            const std::string directorySeparator{yarp::conf::filesystem::preferred_separator};

            getline(cin,in);
            string instr[4] = {"", "", "", ""};
            int j = 0;
            
            //in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
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

            cout << endl;

            /*
             * Help
             */
            if (instr[0] == "help") {
                cout << "help                   : show help." << endl;
                cout << "exit                   : exit printer." << endl;
                cout << "list mod               : list available modules." << endl;
                cout << "list app               : list available application" << endl;
                cout << "list res               : list available resources." << endl;
                cout << "add mod <filename>     : add a module from its description file." << endl;
                cout << "add app <filename>     : add an application from its description file." << endl;
                cout << "add res <filename>     : add resources from a description file." << endl;
                cout << "load app <application> : load an application to run." << endl;
                cout << "run [IDs]              : run application or a modules indicated by IDs." << endl;
                cout << "stop [IDs]             : stop running application or modules indicated by IDs." << endl;
                cout << "kill [IDs]             : kill running application or modules indicated by IDs." << endl;
                cout << "connect [IDs]          : stablish all connections or just one connection indicated by IDs." << endl;
                cout << "disconnect [IDs]       : remove all connections or just one connection indicated by IDs." << endl;
                cout << "which                  : list loaded modules, connections and resource dependencies." << endl;
                cout << "check dep              : check for all resource dependencies." << endl;
                cout << "check state [id]       : check for running state of application or a module indicated by id." << endl;
                cout << "check con [id]         : check for all connections state or just one connection indicated by id." << endl;
                cout << "set <option> <value>   : set value to an option." << endl;
                cout << "get <option>           : show value of an option." << endl;
                cout << "export <filename>      : export application's graph as Graphviz dot format." << endl;
                cout << "show mod <modname>     : display module information (description, input, output,...)." << endl;
                cout << "assign hosts           : automatically assign modules to proper nodes using load balancer." << endl;
            }        

            /*
             * exit
             */
            else if (instr[0] == "exit") {
                do {
                    cout << "If some modules are running or/and connections are enable, these will be closed. Are you sure? [y/n] ";
                    cin >> in;
                    if (in == "y") {
                        manager.stop();
                        manager.kill();
                        manager.disconnect();
                    }
                    else 
                        cout << "Bye :)";
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
                    cout << i << "." << modList[i] << endl;
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
                    cout << i << "." << appList[i] << endl;
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
            else if (instr[0] == "add" && instr[1] == "mod") {
                fileName = instr[2];
                check = manager.addModule((modPath+fileName).c_str());
                if (check == 1)
                    cout << "Module " << fileName << " added." << endl;
                else 
                    cout << "FAIL: Module " << fileName << " did not add." << endl;
            }

            /*
             * add app
             */ 
            else if (instr[0] == "add" && instr[1] == "app") {
                fileName = instr[2];
                check = manager.addApplication((appPath+fileName).c_str(), &szAppName, true);
                if (check == 1)
                    cout << "Application " << fileName << " added." << endl;
                else 
                    cout << "FAIL: Application " << fileName << " did not add." << endl;
            }

            /*
             * add res
             */
            else if (instr[0] == "add" && instr[1] == "res") {
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
                    cout << "Application " << fileName << " loaded." << endl;
                else 
                    cout << "FAIL: Application " << fileName << " did not load." << endl; 
            }

            /*
             * run 
             */
            else if (instr[0] == "run") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.run(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " is running." << endl;
                    else 
                        cout << "FAIL: IDs " << IDs << " is not running." << endl; 
                }
                else 
                    manager.run();
                // cout << "Please insert ad IDs." << endl;
            }

            /*
             * stop
             */
            else if (instr[0] == "stop") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.stop(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " stopped." << endl;
                    else 
                        cout << "FAIL: IDs " << IDs << " did not stopped." << endl;
                }
                else 
                    manager.stop();
                // cout << "Please insert ad IDs." << endl;
            }

            /*
             * kill
             */
            else if (instr[0] == "kill") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.kill(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " killed." << endl;
                    else 
                        cout << "FAIL: IDs " << IDs << " did not killed." << endl; 
                }
                else 
                    manager.kill();
                // cout << "Please insert ad IDs." << endl;
            }

            /*
             * connect
             */
            else if (instr[0] == "connect") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.connect(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " connected." << endl;
                    else 
                        cout << "FAIL: IDs " << IDs << " did not connected." << endl; 
                }
                else 
                    manager.connect();
                // cout << "Please insert ad IDs." << endl;
            }

            /*
             * disconnect
             */
            else if (instr[0] == "disconnect") {
                if (instr[1] != "") {
                    IDs = stoi(instr[1]);
                    check = manager.disconnect(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " disconnected." << endl;
                    else 
                        cout << "FAIL: IDs " << IDs << " did not disconnected." << endl; 
                }
                else 
                    manager.disconnect();
                // cout << "Please insert ad IDs." << endl;
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
                check = manager.checkDependency();
                if (check == 1)
                    cout << "All of resource dependencies are satisfied." << endl;
            }
            
            /*
             * check state 
             */
            else if (instr[0] == "check" && instr[1] == "state") {
                ExecutablePContainer modules = manager.getExecutables();
                if (instr[2] != "") {
                    IDs = stoi(instr[2]);
                    if (IDs > modules.size())
                        cout << "ERROR: Module IDs is out of range." << endl;

                    if (manager.running(IDs)) 
                        cout << "<RUNNING>" << endl;
                    else 
                        cout << "<STOPPED>" << endl;
                    cout << "(" << IDs << ") [" << modules[IDs]->getHost() << "]" << endl;

                    check = manager.disconnect(IDs);
                }
                else 
                    checkStates();
                // cout << "Please insert ad IDs." << endl;
            }

            /*
             * check con
             */
            else if (instr[0] == "check" && instr[1] == "con") {
                CnnContainer connections = manager.getConnections();
                if (instr[2] != "") {
                    IDs = stoi(instr[2]);
                    
                    if (IDs > connections.size())
                        cout << "ERROR: Connection IDs is out of range." << endl;

                    if (manager.connected(IDs)) 
                        cout << "<CONNECTED>" << endl;
                    else 
                        cout << "<DISCONNECTED>" << endl;
                    cout << "(" << IDs << ") [" << connections[IDs].from() << " - " 
                        << connections[IDs].to() << " [" << connections[IDs].carrier() << "]" << endl;
                }
                else 
                    checkConnections();
                // cout << "Please insert ad IDs." << endl;
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
                    cout << "FAIL: Cannot export graph to " << instr[1] << endl;
            }

            /*
             * show mod
             */
            else if (instr[0] == "show" && instr[1] != "mod" && instr[2] != "") {
                KnowledgeBase* kb = manager.getKnowledgeBase();
                check = kb->getModule(instr[2].c_str());
                if (check == 1)
                    cout << kb->getModule(instr[2].c_str()) << endl;
                else 
                    cout << "FAIL: "<< instr[2] << " did not find." << endl;
            }

            /*
             * assign host
             */
            else if (instr[0] == "assign" && instr[1] != "host") {
                manager.loadBalance();
            }

            else 
                if (flag == 1)
                    cout << "Instruction not valid, type 'help' for more informations about it." << endl;
    
            reportErrors();
            cout << endl << ">> ";
        }

        /*
         * which
         */
        void which() {
            modules = manager.getExecutables();
            CnnContainer connections  = manager.getConnections();
            ExecutablePIterator moditr;
            CnnIterator cnnitr;

            cout << endl << "Application: " << endl;
            cout << manager.getApplicationName() << endl;

            cout << endl << "Modules: " << endl;
            int id = 0;
            for(moditr = modules.begin(); moditr < modules.end(); moditr++) {
                cout << "("<<id++<<") " << (*moditr)->getCommand();
                cout << " [" << (*moditr)->getHost() << "] [" << (*moditr)->getParam() << "]";
                cout << " [" << (*moditr)->getEnv() << "]" << endl;
            }
            cout << endl << "Connections: " << endl;
            id = 0;
            for (cnnitr = connections.begin(); cnnitr < connections.end(); cnnitr++) {
                cout << "(" << id++ << ") ";
                cout << (*cnnitr).from() << " - " << (*cnnitr).to();
                    cout<<" [" << (*cnnitr).carrier() << "]";
                cout << endl;
            }

            cout << endl << "Resources:" << endl;
            id = 0;
            ResourcePIterator itrS;
            for (itrS = manager.getResources().begin(); itrS != manager.getResources().end(); itrS++) {
                cout << "(" << id++ << ") ";
                cout << (*itrS)->getName() << " [" << (*itrS)->getTypeName() << "]" << endl;
            }
            cout << endl;
        }

        /*
         * Check executables state
         */
        void checkStates() {
            modules = manager.getExecutables();
            ExecutablePIterator moditr;
            unsigned int id = 0;
            bool bShouldRun = false;
            for (moditr = modules.begin(); moditr < modules.end(); moditr++) {
                if(manager.running(id)) {
                    bShouldRun = true;
                    cout << "<RUNNING> ";
                }
                else
                    cout << "<STOPPED> ";
                cout << "(" << id << ") " << (*moditr)->getCommand();
                cout << " [" << (*moditr)->getHost() << "]" << endl;
                id++;
            }
        }
        
        /*
         * Check connections
         */ 
        void checkConnections() {
            CnnContainer connections = manager.getConnections();
            CnnIterator cnnitr;
            int id = 0;
            for (cnnitr = connections.begin(); cnnitr < connections.end(); cnnitr++) {
                if(manager.connected(id))
                    cout << "<CONNECTED> ";
                else
                    cout << "<DISCONNECTED> ";

                cout << "(" << id << ") " << (*cnnitr).from() << " - " << (*cnnitr).to();
                cout << " [" << (*cnnitr).carrier() << "]" << endl;
                id++;
            }
        }

        /*
         * Getting errors
         */
        void reportErrors() {
            ErrorLogger* logger  = ErrorLogger::Instance();
            if(logger->errorCount() || logger->warningCount()) {
                const char* msg;
                while((msg = logger->getLastError()))
                    cout << "ERROR: " << msg << endl;

                while((msg = logger->getLastWarning()))
                    cout << "WARNING: " << msg << endl;
            }
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
            string appPath = "/usr/share/yarp/applications/";
            //string appPath;
        
            while (true) {
                cout << "Please supply the path to manager applications: ";
                //cin >> appPath;
                cout << endl;
                if (appPath[appPath.size()-1] != '/')
                    appPath.append("/");
                dir  = opendir(appPath.c_str());
                if (dir == NULL) {
                    cout << "Directory: " << dir << " not found" << endl;
                    continue;
                }
                else break;
            }
            return appPath;
        }
        
        /*
        * Applications list
        */
        vector<string> getApplicationsList(int& appNum) {
            vector<string> appList = {""};
            cout << appList[0] << endl;
            while ((pDir = readdir(dir)) != NULL) {
                if (string(pDir->d_name).find(".xml") == string::npos)
                    continue; 
                appList.push_back(string(pDir->d_name));
                appNum++;
            } 
            return appList;
        }

        /* 
         * Modules path 
         */
        string getModulesPath() {
            string modPath = "/usr/share/yarp/modules/";
            //string modPath;

            while (true) {
                cout << "Please supply the path to manager modules: ";
                //cin >> modPath;
                cout << endl;
                if (modPath[modPath.size()-1] != '/')
                    modPath.append("/");
                dir  = opendir(modPath.c_str());
                if (dir == NULL) {
                    cout << "Directory: " << dir << " not found" << endl;
                    continue;
                }
                else break;
            }
            return modPath;
        }

        /*
        * Modules list
        */
        vector<string> getModulesList(int& modNum) {
            vector<string> modList = {""};
            cout << modList[0] << endl;
            while ((pDir = readdir(dir)) != NULL) {
                if (string(pDir->d_name).find(".xml") == string::npos)
                    continue; 
                modList.push_back(string(pDir->d_name));
                modNum++;
            }             
            return modList;
        }
};

/*
 * Start program
 */
int main(int argc, char* argv[]) {
    
    Init objInit;

    /* Starting message */
    cout << "********************************" << endl;
    cout << "*                              *" << endl;
    cout << "*        Yarp Manager          *" << endl;   
    cout << "*       Message Printer        *" << endl;                  
    cout << "*   Author: Simone Contorno    *" << endl;
    cout << "*                              *" << endl;
    cout << "********************************" << endl << endl;

    /* Sypplying data */
    //string portName = "/printer2";
    string portName;
    cout << "Please supply a port name for the server: ";
    cin >> portName;
    portName = "/" + portName;

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
    
    /* Calling Message Printer */
    Printer objPrinter;
    objPrinter.messagePrinter(portName, appNum, appPath, appList, modNum, modPath, modList);

    return 0;
}

/* NOTE */

/*
#include <yarp/manager/localbroker.h>
#include <yarp/manager/yarpdevbroker.h>
#include <yarp/manager/scriptbroker.h>
#include <yarp/manager/xmlapploader.h>
#include <yarp/manager/xmlmodloader.h>
#include <yarp/manager/xmlresloader.h>
#include <yarp/manager/xmlappsaver.h>
#include <yarp/manager/singleapploader.h>
#include <yarp/os/impl/NameClient.h>

#define BROKER_LOCAL            "local"
#define BROKER_YARPRUN          "yarprun"
#define BROKER_YARPDEV          "yarpdev"

#include <yarp/os/PeriodicThread.h>
#include <yarp/os/LogStream.h>
*/

/*
if (argc < 2) {
    fprintf(stderr, "Please supply a port name for the server\n");
    return 1;
}
*/
    
//objPrinter.messagePrinter(argv[1]);

//fileName = "/home/simone/yarp/data/yarpmanager/tests/xml/applications/eyesviewer-localhost.xml";
            //fileName = "/home/simone/yarp/build/share/yarp/applications/eyesviewer-localhost.xml";
            //fileName = "/home/simone/.local/share/Trash/files/eyesviewer-localhost.xml";
            //fileName = "/usr/share/yarp/./applications/eyesviewer-localhost.xml";

                        
            /* Adding and Loading Applications
            for (int i=0; i < appNum; i++) {
                fileName = appPath + appList[i+1];
                check = manager.addApplication(fileName.c_str(), &szAppName, true);
                cout << "Application " << fileName << " added: " << check << endl;
                //check = manager.loadApplication(szAppName);
                //cout << "Application loaded: " << check << endl;           
            }

            cout << endl << "**************************************************" << endl << endl;
            */

            /* Adding Modules
            for (int i=0; i < modNum; i++) {
                fileName = modPath + modList[i+1];
                check = manager.addModule(fileName.c_str());
                cout << "Module " << fileName << " added: " << check << endl;      
            }

            cout << endl << "**************************************************" << endl << endl;
            */
/*
            string fileName = "";
            bool check = false;
            char* szAppName = nullptr;
            fileName = "/usr/share/yarp/applications/eyesviewer-localhost.xml";
            check = manager.addApplication(fileName.c_str(), &szAppName, true);
            cout << "Application " << fileName << " added: " << check << endl;
            check = manager.loadApplication(szAppName);
            cout << "Application loaded: " << check << endl;  
            manager.removeApplication(fileName.c_str(), szAppName);
            
            // TEST 

            const char* appName = manager.getApplicationName();
            cout << "Application name: " << appName << endl;
            executablePContainer = manager.getExecutables();
            cout << "Number of Executables: " << executablePContainer.size() << endl << endl;
            exec = manager.getExecutableById(0);
            check = manager.getExecutableById(0); // check
            cout << "Get excutable by id '0': " << check << endl;
            execState(exec, 0);
            
            cout << endl << "Trying to run executable... " << endl;
            check = manager.run(0);
            cout << "Application runned: " << check << endl;
            execState(exec, 0);


            */
            /*
            exec = manager.getExecutableById(1);
            cout << endl << "Trying to run executable... " << endl;
            check = manager.run(0);
            cout << "Application runned: " << check << endl;
            execState(exec, 0);
            */

            /*
            cout << endl << "Trying to stop executable..." << endl;
            check = manager.stop(0);
            cout << "Application stopped: " << check << endl;
            execState(exec);

            cout << endl << "Trying to kill executable..." << endl;
            check = manager.kill(0);
            cout << "Application killed: " << check << endl;
            execState(exec);
            */