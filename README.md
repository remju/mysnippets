# mysnippets
Command line interface code snippets manager

## Install
```
sudo make install
```
I suggest adding an alias for the *mysnippets* command.

## Usage
**Save a snippet**:
```
mysnippets save <file> '<snippet>'
```
On the first ever save, it will create a .mysnippets directory in your $(HOME). Snippets are aved there.

**Get a snippet**:
```
mysnippets get <file>
```
