<h1>YARP - Yet Another Robot Platform</h1>
<h2>Runtime monitors of YARP modules</h2>

<h3>Introduction</h3>
<p>yarpmanager-console2 is a program to manage and monitor YARP modules.<br>
It looks like this at start:</p>

![start](https://github.com/simone-contorno/tesi/blob/main/images/schermata_iniziale.png)

<h3>How it works</h3>
<p>yarpmanager-console2 allows many commands to lauch, this is the list: </p>

![start](https://github.com/simone-contorno/tesi/blob/main/images/help.png)

<h3>Installation</h3>
<p>Download the zip code:</p>

![start](https://github.com/simone-contorno/tesi/blob/main/images/download.png)

<p>Go in the folder where you download the zip and extract it:</p>

<pre>
 <code>
  unzip file.zip 
 </code>
</pre>

<p>Configure the ymc2-config.ini file and set:
 <ul>
  <li>appPath: path of your applications files.</li>
  <li>modPath: path of your modules files.</li>
  <li>resPath: path of your resources files.</li>
  <li>anyRes: write 'y' if you have any resource file, otherwise write 'n'.</li>
  <li>sleepTimer: timer to check modules and connections states; it is '5000' (milliseconds) to deafult.</li>
 </ul>
</p>


