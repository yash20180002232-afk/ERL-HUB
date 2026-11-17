# PROJECT REPORT
## Post-Earthquake Multi-Agency Emergency Relief Logistics Hub (ERL-HUB)

**Subject:** Object-Oriented Programming Using C++ (CIC-211)  
**Institution:** Maharaja Agrasen Institute of Technology  
**Department:** Computer Science and Engineering  
**Submitted by:** [Yash]  
**Roll Number:** [16214802725]  
**Class / Section:** [3-CSE-C8]  
**Submission date:** [17/11/2026]

---

## 1. Introduction

Earthquakes can damage roads, buildings, communication networks and public utilities within a short time. After such an event, relief teams need to move food, drinking water, medicines and rescue equipment to affected areas. Different vehicles are used for this work. Trucks carry large quantities of supplies, helicopters reach places where roads are blocked, and amphibious vehicles can operate near flooded or waterlogged areas.

The Post-Earthquake Multi-Agency Emergency Relief Logistics Hub (ERL-HUB) is a C++ console application designed around this situation. It provides a small model of a relief distribution centre. The program keeps track of supplies, represents different transport vehicles, places urgent missions in priority order and saves dispatch records to a binary file.

The project is an academic demonstration of object-oriented programming. It does not connect to real vehicles, GPS devices or emergency services.

## 2. Problem Statement

During a disaster, relief material may be stored at different depots and dispatched by different types of vehicles. If inventory and dispatch details are handled manually, coordinators may face problems such as:
- not knowing how much stock is available;
- assigning more cargo than a vehicle can carry;
- treating a routine delivery as more urgent than a medical emergency;
- dispatching a vehicle with insufficient fuel;
- selecting a route that has been reported as damaged; and
- losing a record of completed dispatches.

The project models these basic problems and demonstrates how a C++ program can help organize the information.

## 3. Objectives

1. Represent relief cargo and distribution depots using classes.
2. Maintain item quantities and allow restocking and dispatch.
3. Represent ground, aerial and amphibious vehicles.
4. use inheritance and runtime polymorphism for vehicle operations.
5. Check cargo capacity, fuel and route status before dispatch.
6. Arrange relief missions according to urgency.
7. Save dispatch information in a binary file.
8. Demonstrate the major concepts from all four units of the CIC-211 syllabus.

## 4. Scope

The current version is a console-based prototype. It uses sample data for one central depot and a small vehicle fleet. It demonstrates the logic of inventory management and dispatch planning, but it is not a production disaster-management system. Real deployment would require verified live data, secure communication, user authentication, a database, map services and testing with emergency-response professionals.

## 5. System Design

### 5.1 Main modules

**Inventory module:** `ReliefCargo` stores the name, quantity and unit weight of an item. `DistributionDepot` stores cargo entries and supports restocking and dispatch.

**Vehicle module:** `ReliefVehicle` is the abstract base class. `GroundConvoy`, `AerialTransport` and `AmphibiousCraft` represent different transport types.

**Mission module:** `Mission` stores a mission name, urgency value and destination. `PriorityDispatchQueue` keeps missions ready for dispatch.

**Logging module:** Dispatch details are appended to `dispatch_log.dat` using binary file operations.

### 5.2 Basic workflow

1. The program creates a central relief depot.
2. Initial water, ration kits and medicine stock is added.
3. The incident commander can approve an emergency restock.
4. The vehicle fleet is created.
5. The program displays vehicle information and estimated travel times.
6. Missions are arranged by urgency.
7. Cargo is loaded and the selected vehicle is checked before dispatch.
8. A dispatch record is saved and can be read back from the log file.

## 6. Implementation of Unit I: Core Foundations and Call Semantics

- **Namespaces:** The code is organized into `ReliefOps::Inventory` and `ReliefOps::Logistics` to keep related classes and functions together.
- **Inline functions:** `calculateDistance()` and `calculatePayload()` are small utility functions.
- **Function overloading:** `allocateSupply()` has two versions. One accepts available and requested quantities; the other also accepts a medical-emergency flag.
- **Call by reference:** `updateMissionDistance()` receives the mission distance by reference and updates the original value.
- **Return by reference:** The same function returns a reference to the updated distance.
- **Friend class:** `IncidentCommander` is declared as a friend of `DistributionDepot`. This allows the commander to access depot stock directly for a critical restock operation.

## 7. Implementation of Unit II: Encapsulation, Dynamic Memory and Operator Overloading

- **Encapsulation:** Data members in `ReliefCargo` and `DistributionDepot` are private. Public methods control how the data is changed.
- **Parameterized constructor:** `ReliefCargo` accepts the item name, quantity, unit weight and number of batch codes.
- **Dynamic memory:** Batch codes are stored in a dynamically allocated integer array.
- **Copy constructor:** `ReliefCargo` performs a deep copy of the batch-code array.
- **Destructor:** The destructor releases the dynamically allocated array with `delete[]`.
- **Static members:** `ReliefVehicle::totalOperationalVehicles` and `totalDispatchedTonnage` keep fleet-wide statistics.
- **Operator overloading:** `+=` restocks cargo, `-=` removes dispatched stock, `==` compares cargo entries, and `<<` / `>>` provide stream input and output support.

## 8. Implementation of Unit III: Inheritance, Containership and Polymorphism

- **Abstract base class:** `ReliefVehicle` contains pure virtual functions `vehicleType()` and `estimateTravelTime()`.
- **Inheritance:** `GroundConvoy` and `AerialTransport` derive from `ReliefVehicle`.
- **Virtual inheritance:** Both intermediate vehicle types virtually inherit from `ReliefVehicle`. `AmphibiousCraft` inherits from both, avoiding two separate `ReliefVehicle` base objects.
- **Containership:** Each vehicle contains independent `GPSBeacon` and `EmergencyLifeSupportUnit` objects.
- **Runtime polymorphism:** The fleet is stored using `unique_ptr<ReliefVehicle>`. Calls to virtual functions use the actual vehicle type at runtime.
- **Virtual destructor:** `ReliefVehicle` has a virtual destructor so derived objects are destroyed correctly through base-class pointers.

## 9. Implementation of Unit IV: Streams, Exceptions and STL

- **Custom exceptions:** `CargoOverweightException`, `FuelDepletionException` and `DamagedCorridorException` provide specific error messages.
- **Binary file handling:** `saveDispatchLog()` writes record lengths and record contents to `dispatch_log.dat` using binary streams. The log reader restores the records.
- **STL containers:** `vector` stores the vehicle fleet and mission list. `unordered_map` stores depot inventory.
- **STL algorithms:** `sort()` and `for_each()` are used to arrange and display missions.
- **Template class:** `PriorityDispatchQueue<T>` provides a reusable queue for items that support priority comparison.
- **Functor:** `UrgencyFirst` compares missions by their urgency value.
- **Custom stream manipulator:** `urgentTable` prints a heading and separator for the mission table.

## 10. Testing

The following checks should be performed before submission:

| Test | Input / action | Expected result |
|---|---|---|
| Initial stock | Start the program | Depot stock is displayed |
| Emergency restock | Commander adds medicine | Medicine quantity increases |
| Vehicle display | Display the fleet | Three vehicle types appear |
| Mission priority | Add missions with different urgency | Highest urgency is processed first |
| Cargo loading | Load cargo within capacity | Cargo is accepted |
| Capacity limit | Load cargo above capacity | `CargoOverweightException` is raised |
| Fuel check | Dispatch with fuel below 15% | `FuelDepletionException` is raised |
| Route check | Dispatch with a blocked route | `DamagedCorridorException` is raised |
| File logging | Complete a dispatch | Record is saved in `dispatch_log.dat` |
| Log reading | Read saved records | Previously saved dispatch is displayed |

The supplied demonstration uses a successful dispatch path. The exception cases can be tested by changing the sample values in `main()`.

## 11. Sample Output

The exact spacing may differ by compiler. A normal run displays:
- the central depot inventory;
- the emergency medicine restock message;
- the list of three vehicles;
- estimated travel times;
- missions in priority order;
- a successful vehicle dispatch message;
- fleet statistics; and
- the saved dispatch record.

## 12. Limitations

1. The program uses sample data rather than live disaster information.
2. Inventory is held in memory and is not restored from a permanent database.
3. The dispatch log is a basic binary record file, not a secure audit database.
4. Route availability and fuel values are entered or simulated by the program.
5. There is no graphical user interface or multi-user access.

## 13. Future Enhancements

- Add a database to preserve depot inventory between runs.
- Add login roles for coordinators and depot staff.
- Connect to verified map and route information.
- Add a graphical dashboard showing vehicle and stock status.
- Add report generation for completed missions.
- Validate dispatch operations with realistic disaster-response scenarios.

## 14. Conclusion

ERL-HUB demonstrates how object-oriented programming can be applied to a practical logistics problem. It brings inventory, transport vehicles, mission priorities, exception handling and file logging into one small C++ application. The project also provides examples of the concepts covered in all four units of the CIC-211 syllabus. Although it is only an academic prototype, it provides a foundation that can be extended with persistent storage, live route data and a user interface.

## 15. References

1. CIC-211 Project Assignment issued by the Department of Computer Science and Engineering, Maharaja Agrasen Institute of Technology.
2. Bjarne Stroustrup, *The C++ Programming Language*, 4th Edition.
3. cppreference.com — C++ language and standard library reference: https://en.cppreference.com/
