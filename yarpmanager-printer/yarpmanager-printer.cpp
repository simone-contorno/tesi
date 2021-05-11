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
        int flag = -1;
        int check_flag = -1;
    public: 
        void messagePrinter(string portNamePrinter, string portNameConsole) {
            
            /* Opening portNamePrinter and connect with /root and /portNameConsole */
            port.open(portNamePrinter);
            command = "yarp connect " + portNameConsole + " " + portNamePrinter;
            system(command.c_str());
            command = "yarp connect /root " + portNamePrinter;
            system(command.c_str());
            
            cout << "Printer is running..." << endl; 
            cout << endl << "**************************************************" << endl << endl;
        
            /* Listening on portName */
            while (true) {
                /* Getting time */ 
                string currentTime = getTime();
                
                /* Getting Connection name */ 
                port.read(cmd,true);
                string connName = getProcessName(cmd);
  
                /* Printing Connection Message */
                printMessage(connName, currentTime, flag, check_flag);
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
         * Printing Connection Message
         */
        void printMessage(string connName, string currentTime, int& flag, int& check_flag) {          
            if (cmd.toString().find("[add]") == 0) {
                flag = 0;
                if (check_flag != flag)
                    cout << endl;
                cout << "[+] Connection '" << connName << "' opened at: " << currentTime << endl;
            }
            else if (cmd.toString().find("[del]") == 0) {
                flag = 1;
                if (check_flag != flag)
                    cout << endl;
                cout << "[-] Connection '" << connName << "' closed at: " << currentTime << endl;
            }
            else 
                cout << endl << cmd.toString().c_str() << " at: " << currentTime << endl;

            check_flag = flag;
        }
};

/*
 * Start program
 */
int main(int argc, char* argv[]) {

    /* Start message */
    cout << "********************************" << endl;
    cout << "*                              *" << endl;
    cout << "*        Yarp Manager          *" << endl;   
    cout << "*       Message Printer        *" << endl;                  
    cout << "*   Author: Simone Contorno    *" << endl;
    cout << "*                              *" << endl;
    cout << "********************************" << endl << endl;

    /* Sypply data */
    string portNamePrinter;
    string portNameConsole;

    cout << "Please supply a port name for the Printer: ";
    cin >> portNamePrinter;
    if (portNamePrinter.at(0) != '/')
        portNamePrinter = "/" + portNamePrinter;

    cout << "What is the name of the Console's port? ";
    cin >> portNameConsole;
    if (portNameConsole.at(0) != '/')
        portNameConsole = "/" + portNameConsole;

    /* Call Printer */
    Printer objPrinter;
    objPrinter.messagePrinter(portNamePrinter, portNameConsole);

    return 0;
}