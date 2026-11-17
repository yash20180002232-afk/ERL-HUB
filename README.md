# ERL-HUB: Emergency Relief Logistics Hub

A console-based C++17 project for managing relief supplies and transport after an earthquake.

## Features
- Depot inventory and emergency restocking
- Ground, aerial and amphibious transport vehicles
- Cargo capacity and dispatch checks
- Priority-based mission queue
- Binary dispatch log saved in `dispatch_log.dat`
- Exception handling for stock, cargo, fuel and route problems

## Build and run

### Windows (MinGW)
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o erl_hub.exe
erl_hub.exe
```

### Linux / macOS
```bash
g++ -std=c++17 -Wall -Wextra -pedantic main.cpp -o erl_hub
./erl_hub
```
