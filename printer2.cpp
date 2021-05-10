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
        Executable* exec;
        ExecutablePContainer executablePContainer;    

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
        
            cout << "Digit 'help' to see all avaiable commands" << endl;
            while (true) {
                getInput(appNum, appPath, appList, modNum, modPath, modList);
            }
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

            cout << endl << "**************************************************" << endl << endl;

            
            //command = "gnome-terminal";
            //system(command.c_str());
            while (true) {
                /* Getting time */ 
                string current_time = getTime();
                
                /* Getting Connection name */ 
                port.read(cmd,true);
                string conn_name = getProcessName(cmd);
                
                /* Printing message */
                if (cmd.toString().find("[add]") == 0 ||
                        cmd.toString().find("[del]") == 0) {
                    for (int i = 0; i < executablePContainer.size(); i++) {
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
            cout << ">> ";
 
            getline(cin,in);
            in.erase(remove_if(in.begin(), in.end(), ::isspace), in.end());
            
            cout << endl;

            if (in.find("help") != std::string::npos) {
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
                cout << "connect [IDs]          : stablish connection indicated by IDs." << endl;
                cout << "disconnect [IDs]       : remove connection indicated by IDs." << endl;
            }        

            //else if (in == "exit")
                // ToDo: close all connections, ports and close program
                
            else if (in.find("listmod") != std::string::npos) {
                cout << "Modules list (" << modNum << "): " << endl;
                for (int i = 0; i < modList.size(); i++) {
                    if (modList[i] == "") continue;
                    cout << i << "." << modList[i] << endl;
                }    
            }
            
            else if (in.find("listapp") != std::string::npos) {           
                cout << "Applications list (" << appNum << "): " << endl;
                for (int i = 0; i < appList.size(); i++) {
                    if (appList[i] == "") continue;
                    cout << i << "." << appList[i] << endl;
                }
            }

            else if (in.find("listres") != std::string::npos) {
                // ToDo: print resources list
            }

            else if (in.find("addmod") != std::string::npos) {
                fileName = modPath + in.erase(0,6);
                check = manager.addModule(fileName.c_str());
                if (check == 1)
                    cout << "Module " << fileName.erase(0,modPath.length()) << " added." << endl;
                else 
                    cout << "An error occurred: Module " << fileName.erase(0,modPath.length()) << " did not add." << endl;
            }

            else if (in.find("addapp") != std::string::npos) {
                fileName = appPath + in.erase(0,6);
                check = manager.addApplication(fileName.c_str(), &szAppName, true);
                if (check == 1)
                    cout << "Application " << fileName.erase(0,appPath.length()) << " added." << endl;
                else 
                    cout << "An error occurred: Application " << fileName.erase(0,appPath.length()) << " did not add." << endl;
            }

            else if (in.find("addres") != std::string::npos) {
                /*
                fileName = resPath + in.erase(0,6);
                check = manager.addResource(fileName.c_str());
                if (check == 1)
                    cout << "Resource" << fileName << " added." << endl;
                else 
                    cout << "An error occurred: Resource" << fileName << " did not add." << endl;
                */
            }

            else if (in.find("loadapp") != std::string::npos) {
                fileName = appPath + in.erase(0,7);
                check = manager.loadApplication(szAppName);
                if (check == 1)
                    cout << "Application " << fileName.erase(0,appPath.length()) << " loaded." << endl;
                else 
                    cout << "An error occurred: Application " << fileName.erase(0,appPath.length()) << " did not load." << endl; 
            }

            else if (in.find("run") != std::string::npos) {
                if (in.length() > 3) {
                    IDs = stoi(in.erase(0,3));
                    check = manager.run(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " is running." << endl;
                    else 
                        cout << "An error occurred: IDs " << IDs << " is not running." << endl; 
                }
                else cout << "Please insert ad IDs." << endl;
            }

            else if (in.find("stop") != std::string::npos) {
                if (in.length() > 4) {
                    IDs = stoi(in.erase(0,4));
                    check = manager.stop(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " stopped." << endl;
                    else 
                        cout << "An error occurred: IDs " << IDs << " did not stopped." << endl;
                }
                else cout << "Please insert ad IDs." << endl;
            }

            else if (in.find("kill") != std::string::npos) {
                if (in.length() > 4) {
                    IDs = stoi(in.erase(0,4));
                    check = manager.kill(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " killed." << endl;
                    else 
                        cout << "An error occurred: IDs " << IDs << " did not killed." << endl; 
                }
                else cout << "Please insert ad IDs." << endl;
            }

            else if (in.find("connect") != std::string::npos && 
                        in.find("disconnect") == std::string::npos) {
                if (in.length() > 7) {
                    IDs = stoi(in.erase(0,7));
                    check = manager.connect(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " connected." << endl;
                    else 
                        cout << "An error occurred: IDs " << IDs << " did not connected." << endl; 
                }
                else cout << "Please insert ad IDs." << endl;
            }

            else if (in.find("disconnect") != std::string::npos) {
                if (in.length() > 10) {
                    IDs = stoi(in.erase(0,10));
                    check = manager.disconnect(IDs);
                    if (check == 1)
                        cout << "IDs " << IDs << " disconnected." << endl;
                    else 
                        cout << "An error occurred: IDs " << IDs << " did not disconnected." << endl; 
                }
                else cout << "Please insert ad IDs." << endl;
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