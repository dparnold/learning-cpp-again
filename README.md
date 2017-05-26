# learning-cpp-again

I needed to refresh my knowledge of C++. This is some kind of documentation combined with a code playground.

**C++ Setup**

First I run
```
sudo apt-get update
sudo apt-get install build-essential manpages-dev
```
to get build-essential which includes gcc and g++ and a lot more.
Afterwards, I install my favorite IDE eclipse.
```
sudo apt-get install eclipse
```
This might not be the newest version but I do not care because it is working well.
In the next step I install the "C++ Development Tooling (CDT)" plugin for eclipse.
To to so, go to "Help" and "Install New Software".
Work with "All available sites", search for C++ and install the C/C++ Developement Tools.
Restart eclipse after the installation.

Then it is time to start a new C++ project.

**Install Qt**

As I would like to create a GUI, I need to install Qt.
```
sudo apt-get install qt5-default qt5-doc  
```
For developing Qt Applications, it is very easy to use the Qt Creator. Just get it by:
```
sudo apt-get install qtcreator 
```
