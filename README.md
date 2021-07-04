<h1>YARP - Yet Another Robot Platform</h1>
<h2>Runtime monitors of YARP modules</h2>

### Introduction
An overview of the main concepts about this program.<br>
[Go to Introduction](#intro)

### How it works
A rapid description of how the program works (with references to the code).<br>
[Go to How it works](#how)

### Installation, Configuration and Execution
How install yarpmanager-console2 on Linux.<br>
[Go to Installation, Configuration and Execution](#installation)

### Tutorial (example)
A small tutorial to run a module and check if it terminate unexpectedly.<br>
[Go to Tutorial (example)](#tutorial)

### Acknowledgments
Conclusion of this documentation.<br>
[Go to Acknowledgments](#ack)

<br><br><br>
Bachelor's thesis degree in Computer Engineering by Simone Contorno.<br>
Documentation about yarpmanager-console2 program.

<a name="intro"></a>
### Introduction

YARP supports building a robot control system as a collection of programs communicating in a peer-to-peer way, with an extensible family of connection types (tcp, udp, multicast, ...).<br>
YARP offers a lot of command line tools like yarpmanager-console; this is a program to run and manage multiple programs on a set of machines; you can do it managing: 
 
 <ul>
  <li>Module: an executable binary file (program).</li>
  <li>Application: a collection of modules, connections and other applications.</li>
  <li>Resources: refers to any physical or logical resources which are needed by module.</li>
 </ul>

It is possible control the state of a module or an application:

 <ul> 
  <li>RUNNING: the module or the application is in execution.</li>
  <li>STOPPED: the module or the application is not in execution.</li>
 </ul>

and of a connection:

 <ul> 
   <li>CONNECTED</li>
   <li>DISCONNECTED</li>
  </ul>
  
yarpmanager-console2 implement a new function that works as a separate thread during the execution of the program: monitors modules, connections and reports if one of them terminates unexpectedly.<br>
<br>
It looks like this at start:

![schermata_iniziale](https://user-images.githubusercontent.com/83269499/124388487-74c9c800-dce3-11eb-8a19-3a754d8937dc.png)

<a name="how"></a><br>
### How it works
yarpmanager-console2 allows many commands to launch:
 
 <ul>
  <li>help:                   show help.</li>
  <li>exit:                   exit from the program.</li>
  <li>list mod:               list available modules.</li>
  <li>list app:               list available applications.</li>
  <li>list res:               list available resources.</li>
  <li>add mod [filename]:     add a module from its description file.</li>
  <li>add app [filename]:     add an application from its description file.</li>
  <li>add res [filename]:     add a resource from its description file.</li>
  <li>load app [application]: load an application to run.</li>
  <li>run [IDs]:              run an application or a module indicated by IDs.</li>
  <li>stop [IDs]:             stop running application or modules indicated by IDs.</li>
  <li>kill [IDs]:             kill running application or modules indicated by IDs</li>
  <li>connect [IDs]:          establish all connections or just one connection indicated by IDs.</li>
  <li>disconnect [IDs]:       remove all connections or just one connection indicated by IDs.</li>
  <li>which:                  list loaded modules, connection and resource dependencies.</li>
  <li>check dep:              check for all resource dependencies.</li>
  <li>check state [id]:       check for running state of application or a module indicated by id.</li>
  <li>check con [id]:         check for all connections state or just one connection indicated by id.</li>
  <li>set <option> [value]:   set value to an option. To see all available options type 'set --help'.</li>
  <li>export [filename]:      export application's graph ad Graphviz dot format.</li>
  <li>assign hosts:           automatically assign modules to proper nodes using load balancer.</li>
  </ul>

It appears like this in the program: 

![help_img](https://user-images.githubusercontent.com/83269499/124388485-6f6c7d80-dce3-11eb-9238-52c408a3c72b.png)

When you launch the program it starts with the main class; if you did not set the 'ymc2-config.ini' file, you need to insert the folder's paths that contain your applications, modules and resources (in this last one case, only if you have them, otherwise you can digit 'n' when the program will ask you, or set it in the 'ymc2-config.ini' file).<br>
After, the 'main' class starts the separate thread, that will check all modules and connections each 5 seconds (to default, but you can set it in the 'ymc2-config.ini' file), and calls the function 'messageConsole' by the 'Console' class; it starts a 'while' cicle that calls the function 'getInput', so you can start to use all the commands shown above; when you digit one of them, it starts a control with a conditional structure 'if-else if-else' to select the correct operation and call the correct functions.<br>
When you have finished to use the program, you need to ditig 'exit' to close it, because there is a control in the 'main' class to catch the 'CTRL+C' signal; in this way the program will close all connections and running modules before the end.

<a name="installation"></a><br>
### Installation, Configuration and Execution
Open the terminal and go in the folder where you want to download yarpmanager-console2 and digit:

<pre>
 <code>
  git clone https://github.com/simone-contorno/yarpmanager-console2
 </code>
</pre>

You can also download the zip code:

![download](https://user-images.githubusercontent.com/83269499/124388479-69769c80-dce3-11eb-90b7-744de32af414.png)

Go in the folder where you downloaded the zip and extract it:

<pre>
 <code>
  unzip tesi-main.zip
 </code>
</pre>

Enter in the folder:

<pre>
 <code>
  cd yarpmanager-console2-main
 </code>
</pre>

Open the 'ymc2-config.ini' file:

<pre>
 <code>
  nano ymn2-config.ini
 </code>
</pre>

Set:
 
<ul>
  <li><b>appPath</b>: path of your applications files.</li>
  <li><b>modPath</b>: path of your modules files.</li>
  <li><b>resPath</b>: path of your resources files.</li>
  <li><b>anyRes</b>: write 'y' if you have any resource file, otherwise write 'n'.</li>
  <li><b>sleepTimer</b>: timer to check modules and connections states; it is '5000' (milliseconds) to deafult.</li>
 </ul>
 
Save typing 'CTRL+X' and 'y' to confirm.

Create a folder named 'build':

<pre>
 <code>
  mkdir build
 </code>
</pre>

Enter in the 'build' folder:

<pre>
 <code>
  cd build
 </code>
</pre>

Generates the 'Makefile':

<pre>
 <code>
  cmake ..
 </code>
</pre>

Build the program:

<pre>
 <code>
  make
 </code>
</pre>

Run the program:

<pre>
 <code>
  ./yarpmanager-console2
 </code>
</pre>

Now you can play with yarpmanager-console2.

<a name="tutorial"></a><br>
### Tutorial (example)

Run the program and watch all the available commands:

<pre>
 <code>
  help
 </code>
</pre>

Show all available applications:

<pre>
 <code>
  list app
 </code>
</pre>

If you set, or insert, correctly the path of your application, you will see a list like this one:

![list_app_img](https://user-images.githubusercontent.com/83269499/124388462-5a8fea00-dce3-11eb-9650-dd6fccc00af6.png)

Add one of them (in this example i added 'eyesviewer-localhost.xml'):

<pre>
 <code>
  add app eyesviewer-localhost.xml
 </code>
</pre>

Load it:

<pre>
 <code>
  load app eyesviewer-localhost.xml
 </code>
</pre>

Show all informations (name, modules, connections and resources) about it: 

<pre>
 <code>
  which
 </code>
</pre>

You will see something like this: 

![which_img](https://user-images.githubusercontent.com/83269499/124388671-2a951680-dce4-11eb-8c34-e7c0ea80c3d4.png)

Run one of the available modules (in this example i runned the first one):

<pre>
 <code>
  run 0
 </code>
</pre>

If you digit 'run' without specifying the module's number, all modules will run.<br>

Check the state of the running module:

<pre>
 <code>
  check state 0
 </code>
</pre>

You will see something like this: 

![running_state_img](https://user-images.githubusercontent.com/83269499/124389044-ba879000-dce5-11eb-9824-9731aad286ce.png)

If you digit 'check state' without specifying the module's number, all module's states will be shown.<br>

Stop it:

<pre>
 <code>
  stop 0
 </code>
</pre>

If you digit 'stop' without specifying the module's number, all modules will stop.<br>
<br>
Check again the state of the module; you will see something like this:

![stopped_state_img](https://user-images.githubusercontent.com/83269499/124389068-d854f500-dce5-11eb-83e9-98397375cc18.png)

Now, to see what happen when a module terminate unexpectedly: run again a module and, if the module that you runned open a window you can try to close it, otherwise you can open the task manager, search the name of the module and terminate it. You will see a message like this one:

![error_massage_img](https://user-images.githubusercontent.com/83269499/124388789-ad1dd600-dce4-11eb-976f-328d5c6d0171.png)

Close the program:

<pre>
 <code>
  exit
 </code>
</pre>

You will see this message: 

![exit_img](https://user-images.githubusercontent.com/83269499/124389600-37b40480-dce8-11eb-8386-c0d676defc37.png)

If you are sure digit 'y', otherwise digit 'n'. 

<a name="ack"></a><br>
### Acknowledgments
I hope that this documentation helped you to understand the functioning of yarpmanager-console2 clearly.<br>
<br>
Bye.	:slightly_smiling_face:
