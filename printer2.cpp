/*
 * Copyright (C) 2006-2021 Istituto Italiano di Tecnologia (IIT)
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms of the
 * BSD-3-Clause license. See the accompanying LICENSE file for details.
 */

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

#include <yarp/os/Bottle.h>
#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <yarp/manager/broker.h>
#include <yarp/manager/manager.h>
#include <yarp/manager/yarpbroker.h>

#include <yarp/manager/execstate.h>
#include <yarp/manager/executable.h>
#include <yarp/os/Time.h>

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

    public: 
        void messagePrinter(string portName, int appNum, string appPath, vector<string> appList,
                                int modNum, string modPath, vector<string> modList) {
            
            string fileName = "";
            bool check = false;
            char* szAppName = nullptr;
            
            /*
            for (int i=0; i < appNum; i++) {
                fileName = appPath + appList[i+1];
                check = manager.addApplication(fileName.c_str(), &szAppName, true);
                cout << "Application " << fileName << " added: " << check << endl;
                //check = manager.loadApplication(szAppName);
                //cout << "Application loaded: " << check << endl;           
            }

            cout << endl << "**************************************************" << endl << endl;
            */

            /*
            for (int i=0; i < modNum; i++) {
                fileName = modPath + modList[i+1];
                check = manager.addModule(fileName.c_str());
                cout << "Module " << fileName << " added: " << check << endl;      
            }

            cout << endl << "**************************************************" << endl << endl;
            */

            /* Opening port_name and connect with /root */
            port.open(portName);
            //yarp.connect("/root",portName);
            command = "yarp connect /root " + portName;
            system(command.c_str());

            cout << "Message Printer is running..." << endl; 
            cout << endl << "**************************************************" << endl << endl;
            
            //fileName = "/home/simone/yarp/data/yarpmanager/tests/xml/applications/eyesviewer-localhost.xml";
            //fileName = "/home/simone/yarp/build/share/yarp/applications/eyesviewer-localhost.xml";
            //fileName = "/home/simone/.local/share/Trash/files/eyesviewer-localhost.xml";
            //fileName = "/usr/share/yarp/./applications/eyesviewer-localhost.xml";
            fileName = "/usr/share/yarp/applications/eyesviewer-localhost.xml";
            check = manager.addApplication(fileName.c_str(), &szAppName, true);
            cout << "Application " << fileName << " added: " << check << endl;
            check = manager.loadApplication(szAppName);
            cout << "Application loaded: " << check << endl;  

            Executable* exec;
            ExecutablePContainer executablePContainer;
            const char* appName = manager.getApplicationName();
            cout << "Application name: " << appName << endl << endl;
            executablePContainer = manager.getExecutables();
            cout << "Number of executables: " << executablePContainer.size() << endl;
            exec = manager.getExecutableById(0);
            execState(exec);
            check = manager.run(0);
            cout << "Application runned: " << check << endl;
            execState(exec);
            /*
            check = manager.stop(0);
            cout << "Application stopped: " << check << endl;
            execState(exec);
            check = manager.kill(0);
            cout << "Application killed: " << check << endl;
            execState(exec);
            */
            cout << endl << "**************************************************" << endl << endl;

            while (true) {
                /* Getting time */ 
                string current_time = getTime();
                
                /* Getting Connection name */ 
                port.read(cmd,true);
                string conn_name = getProcessName(cmd);
                
                printf("Got: %s\n", cmd.toString().c_str());

                /*
                 * Printing Connection Message
                 */
                /*
                if (cmd.toString().find("[add]") == 0 ||
                        cmd.toString().find("[del]") == 0) {
                    for (int i = 0; i < appMod; i++) {
                        check = manager.running(i);
                        if (check == true)
                            cout << "Module running: " << endl;
                        else 
                            cout << "Module not running: " << endl;
                    }
                }
                */
               if (cmd.toString().find("[add]") == 0 ||
                        cmd.toString().find("[del]") == 0) {
                    for (int i = 0; i < 1; i++) {
                        check = manager.running(i);
                        if (check == true)
                            cout << "Application running" << endl;
                        else 
                            cout << "Application not running" << endl;
                        execState(exec);
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
        void execState(Executable* exec) {
            cout << "Executable state: ";
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

            cout << "Applications list (" << appNum << "): " << endl;
            for (int i = 0; i < appList.size(); i++) {
                if (appList[i] == "") continue;
                cout << i << "." << appList[i] << endl;
            }

            cout << endl << "**************************************************" << endl << endl;

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
            
            cout << "Modules list (" << modNum << "): " << endl;
            for (int i = 0; i < modList.size(); i++) {
                if (modList[i] == "") continue;
                cout << i << "." << modList[i] << endl;
            }

            cout << endl << "**************************************************" << endl << endl;

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
    string portName = "/printer2";
    //string portName;
    cout << "Please supply a port name for the server: ";
    //cin >> portName;
    //portName = "/" + portName;

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
if (argc < 2) {
    fprintf(stderr, "Please supply a port name for the server\n");
    return 1;
}
*/
    
//objPrinter.messagePrinter(argv[1]);

