# 100DaysOfCode in IoT Challenge
## Day 8

ESP8266 HTTP Client - making request to locally hosted web server

## Requirements
1. Visual Studio Code IDE:
https://code.visualstudio.com/
2. C/C++ VSCode extension:
https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools
3. Clang-Format VSCode extension:
https://marketplace.visualstudio.com/items?itemName=xaver.clang-format
4. PlatformIO VSCode extension:
https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide

## Building 
### ESP8266 Sketch
In Visual Studio Code go to:

`View -> Command Pallette...`

Type `PlatformIO: Build`

Press Enter

### Go Server
- `cd local-server`
- `go build`

## Running

- open terminal
- cd to the project directory
- `cd local-server`
- `go run main.go`
- deploy sketch using the 'PlatformIO: Upload' button or command (instructions below)


## Deploying code to ESP8266
Connect ESP8266 to your computer

In Visual Studio Code go to:

`View -> Command Pallette...`

Type `PlatformIO: Upload`

Press Enter

## Link to YouTube Video (Day8):

https://www.youtube.com/watch?v=JEf1iy4YApU

## All Days List:

https://github.com/ttarnowski/100DaysOfCodeInIoT

