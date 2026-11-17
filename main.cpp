#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <memory>
#include <limits>

using namespace std;

namespace ReliefOps {
namespace Logistics { class IncidentCommander; }
namespace Inventory {

class CargoOverweightException : public exception {
public:
    const char* what() const noexcept override {
        return "Cargo weight is greater than the vehicle capacity.";
    }
};

class ReliefCargo {
private:
    string name;
    int quantity;
    double weightPerUnit;
    int* batchCodes;
    int batchCount;

public:
    ReliefCargo(string itemName = "Unspecified", int qty = 0,
                double unitWeight = 0.0, int batches = 0)
        : name(itemName), quantity(qty), weightPerUnit(unitWeight),
          batchCodes(nullptr), batchCount(batches) {
        if (quantity < 0 || weightPerUnit < 0 || batchCount < 0)
            throw invalid_argument("Cargo values cannot be negative.");

        if (batchCount > 0) {
            batchCodes = new int[batchCount];
            for (int i = 0; i < batchCount; ++i)
                batchCodes[i] = 1001 + i;
        }
    }

    ReliefCargo(const ReliefCargo& other)
        : name(other.name), quantity(other.quantity),
          weightPerUnit(other.weightPerUnit), batchCodes(nullptr),
          batchCount(other.batchCount) {
        if (batchCount > 0) {
            batchCodes = new int[batchCount];
            for (int i = 0; i < batchCount; ++i)
                batchCodes[i] = other.batchCodes[i];
        }
    }

    ReliefCargo& operator=(ReliefCargo other) {
        swap(name, other.name);
        swap(quantity, other.quantity);
        swap(weightPerUnit, other.weightPerUnit);
        swap(batchCodes, other.batchCodes);
        swap(batchCount, other.batchCount);
        return *this;
    }

    ~ReliefCargo() {
        delete[] batchCodes;
    }

    string getName() const { return name; }
    int getQuantity() const { return quantity; }
    double getTotalWeight() const { return quantity * weightPerUnit; }

    void add(int amount) {
        if (amount < 0) throw invalid_argument("Restock amount cannot be negative.");
        quantity += amount;
    }

    void remove(int amount) {
        if (amount < 0 || amount > quantity)
            throw invalid_argument("Not enough stock for this dispatch.");
        quantity -= amount;
    }

    ReliefCargo& operator+=(int amount) {
        add(amount);
        return *this;
    }

    ReliefCargo& operator-=(int amount) {
        remove(amount);
        return *this;
    }

    bool operator==(const ReliefCargo& other) const {
        return name == other.name && quantity == other.quantity;
    }

    friend ostream& operator<<(ostream& out, const ReliefCargo& cargo);
    friend istream& operator>>(istream& in, ReliefCargo& cargo);
};

ostream& operator<<(ostream& out, const ReliefCargo& cargo) {
    out << left << setw(18) << cargo.name
        << right << setw(8) << cargo.quantity
        << setw(12) << fixed << setprecision(2) << cargo.getTotalWeight();
    return out;
}

istream& operator>>(istream& in, ReliefCargo& cargo) {
    cout << "Item name (one word): ";
    in >> cargo.name;
    cout << "Quantity: ";
    in >> cargo.quantity;
    return in;
}

class DistributionDepot {
private:
    string depotName;
    unordered_map<string, ReliefCargo> stock;

public:
    explicit DistributionDepot(string name) : depotName(name) {}

    string getName() const { return depotName; }

    void addCargo(const ReliefCargo& cargo) {
        auto it = stock.find(cargo.getName());
        if (it == stock.end())
            stock.emplace(cargo.getName(), cargo);
        else
            it->second += cargo.getQuantity();
    }

    void dispatch(const string& item, int amount) {
        auto it = stock.find(item);
        if (it == stock.end())
            throw invalid_argument("This item is not available at the depot.");
        it->second -= amount;
    }

    void showStock() const {
        cout << "\nStock at " << depotName << "\n";
        cout << left << setw(18) << "Item" << right
             << setw(8) << "Units" << setw(12) << "Weight(kg)\n";
        cout << string(38, '-') << "\n";
        for (const auto& entry : stock)
            cout << entry.second << "\n";
    }

    friend class Logistics::IncidentCommander;
};

} // namespace Inventory

namespace Logistics {

using Inventory::ReliefCargo;
using Inventory::DistributionDepot;
using Inventory::CargoOverweightException;

inline double calculateDistance(double km) {
    return km < 0 ? 0 : km;
}

inline double calculatePayload(int units, double unitWeight) {
    return units * unitWeight;
}

double allocateSupply(double available, double requested) {
    return min(available, requested);
}

double allocateSupply(double available, double requested, bool medicalEmergency) {
    if (medicalEmergency)
        return min(available, requested);
    return min(available, requested * 0.75);
}

double& updateMissionDistance(double& currentDistance, double extraDistance) {
    currentDistance += extraDistance;
    return currentDistance;
}

class IncidentCommander {
public:
    void approveCriticalRestock(DistributionDepot& depot,
                                const string& item, int amount) {
        auto it = depot.stock.find(item);
        if (it != depot.stock.end()) {
            it->second += amount;
            cout << "Commander approved emergency restock of "
                 << amount << " " << item << ".\n";
        } else {
            cout << "Item not registered at this depot.\n";
        }
    }
};

class FuelDepletionException : public exception {
public:
    const char* what() const noexcept override {
        return "Vehicle cannot be dispatched because fuel is too low.";
    }
};

class DamagedCorridorException : public exception {
public:
    const char* what() const noexcept override {
        return "The selected route is blocked or damaged.";
    }
};

class GPSBeacon {
private:
    string coordinates;
public:
    explicit GPSBeacon(string location = "Not set") : coordinates(location) {}
    string getLocation() const { return coordinates; }
    void setLocation(const string& location) { coordinates = location; }
};

class EmergencyLifeSupportUnit {
private:
    bool working;
public:
    explicit EmergencyLifeSupportUnit(bool status = true) : working(status) {}
    bool isWorking() const { return working; }
};

class DisasterTelemetryDevice {
protected:
    string unitId;
public:
    explicit DisasterTelemetryDevice(string id = "UNIT-00") : unitId(id) {}
    virtual ~DisasterTelemetryDevice() = default;
};

class ReliefVehicle : public DisasterTelemetryDevice {
protected:
    string vehicleName;
    double capacityKg;
    double currentLoadKg;
    double fuelLevel;
    GPSBeacon gps;
    EmergencyLifeSupportUnit lifeSupport;

public:
    static int totalOperationalVehicles;
    static double totalDispatchedTonnage;

    ReliefVehicle(string id, string name, double capacity,
                  string location, double fuel = 100.0)
        : DisasterTelemetryDevice(id), vehicleName(name),
          capacityKg(capacity), currentLoadKg(0), fuelLevel(fuel),
          gps(location), lifeSupport(true) {
        ++totalOperationalVehicles;
    }

    ReliefVehicle(const ReliefVehicle& other)
        : DisasterTelemetryDevice(other.unitId), vehicleName(other.vehicleName),
          capacityKg(other.capacityKg), currentLoadKg(other.currentLoadKg),
          fuelLevel(other.fuelLevel), gps(other.gps), lifeSupport(other.lifeSupport) {
        ++totalOperationalVehicles;
    }

    virtual ~ReliefVehicle() {
        --totalOperationalVehicles;
    }

    virtual string vehicleType() const = 0;
    virtual double estimateTravelTime(double distanceKm) const = 0;

    void loadCargo(const ReliefCargo& cargo) {
        if (currentLoadKg + cargo.getTotalWeight() > capacityKg)
            throw CargoOverweightException();
        currentLoadKg += cargo.getTotalWeight();
    }

    void dispatch(double distanceKm, bool routeClear) {
        if (fuelLevel < 15)
            throw FuelDepletionException();
        if (!routeClear)
            throw DamagedCorridorException();
        if (!lifeSupport.isWorking())
            throw runtime_error("Life support system check failed.");
        fuelLevel -= min(40.0, distanceKm * 0.4);
        totalDispatchedTonnage += currentLoadKg / 1000.0;
        cout << vehicleName << " dispatched from " << gps.getLocation()
             << " for " << distanceKm << " km.\n";
        currentLoadKg = 0;
    }

    string getName() const { return vehicleName; }
    double getCapacity() const { return capacityKg; }

    friend ostream& operator<<(ostream& out, const ReliefVehicle& vehicle) {
        out << vehicle.vehicleType() << " | " << vehicle.vehicleName
            << " | Capacity: " << vehicle.capacityKg << " kg"
            << " | Fuel: " << vehicle.fuelLevel << "%";
        return out;
    }
};

int ReliefVehicle::totalOperationalVehicles = 0;
double ReliefVehicle::totalDispatchedTonnage = 0.0;

class GroundConvoy : virtual public ReliefVehicle {
public:
    GroundConvoy(string id, string name, double capacity, string location)
        : ReliefVehicle(id, name, capacity, location) {}
    string vehicleType() const override { return "Ground convoy"; }
    double estimateTravelTime(double km) const override { return km / 45.0; }
};

class AerialTransport : virtual public ReliefVehicle {
public:
    AerialTransport(string id, string name, double capacity, string location)
        : ReliefVehicle(id, name, capacity, location) {}
    string vehicleType() const override { return "Aerial transport"; }
    double estimateTravelTime(double km) const override { return km / 180.0; }
};

class AmphibiousCraft : public GroundConvoy, public AerialTransport {
public:
    AmphibiousCraft(string id, string name, double capacity, string location)
        : ReliefVehicle(id, name, capacity, location),
          GroundConvoy(id, name, capacity, location),
          AerialTransport(id, name, capacity, location) {}

    string vehicleType() const override { return "Amphibious hybrid"; }
    double estimateTravelTime(double km) const override { return km / 35.0; }
};

template <typename T>
class PriorityDispatchQueue {
private:
    vector<T> items;
public:
    void push(const T& item) {
        items.push_back(item);
        sort(items.begin(), items.end(), greater<T>());
    }
    bool empty() const { return items.empty(); }
    T pop() {
        if (items.empty()) throw out_of_range("Dispatch queue is empty.");
        T next = items.front();
        items.erase(items.begin());
        return next;
    }
    size_t size() const { return items.size(); }
};

struct Mission {
    string name;
    int urgency;
    string destination;

    bool operator>(const Mission& other) const {
        return urgency > other.urgency;
    }
};

struct UrgencyFirst {
    bool operator()(const Mission& a, const Mission& b) const {
        return a.urgency > b.urgency;
    }
};

struct MissionPriority {
    bool operator()(const Mission& a, const Mission& b) const {
        return a.urgency < b.urgency;
    }
};

ostream& urgentTable(ostream& out) {
    out << left << setw(24) << "MISSION"
        << setw(12) << "PRIORITY" << "DESTINATION\n";
    out << string(55, '-') << "\n";
    return out;
}

void saveDispatchLog(const string& filename, const string& record) {
    fstream file(filename, ios::in | ios::out | ios::binary);
    if (!file) {
        ofstream createFile(filename, ios::binary);
        createFile.close();
        file.open(filename, ios::in | ios::out | ios::binary);
    }

    file.seekp(0, ios::end);
    streampos oldPosition = file.tellp();
    unsigned int length = static_cast<unsigned int>(record.size());

    file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    file.write(record.c_str(), length);

    if (!file) {
        file.clear();
        file.seekp(oldPosition);
        file.put('\0'); // marks an incomplete record if a write fails
        throw runtime_error("Could not save the dispatch log.");
    }
    file.close();
}

void showDispatchLog(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cout << "No saved dispatch log found yet.\n";
        return;
    }

    cout << "\nSaved dispatch records\n";
    while (true) {
        unsigned int length = 0;
        file.read(reinterpret_cast<char*>(&length), sizeof(length));
        if (!file) break;
        if (length > 10000) {
            cout << "Invalid record length found. Stopping log read.\n";
            break;
        }
        string record(length, '\0');
        file.read(&record[0], length);
        if (!file) break;
        cout << "- " << record << "\n";
    }
}

} // namespace Logistics
} // namespace ReliefOps

using namespace ReliefOps::Inventory;
using namespace ReliefOps::Logistics;

int main() {
    try {
        DistributionDepot centralDepot("Central Relief Depot");
        centralDepot.addCargo(ReliefCargo("Water", 500, 1.0, 2));
        centralDepot.addCargo(ReliefCargo("RationKit", 250, 2.0, 2));
        centralDepot.addCargo(ReliefCargo("Medicine", 120, 0.25, 2));

        IncidentCommander commander;
        commander.approveCriticalRestock(centralDepot, "Medicine", 30);

        cout << "\n=== POST-EARTHQUAKE RELIEF LOGISTICS HUB ===\n";
        centralDepot.showStock();

        vector<unique_ptr<ReliefVehicle>> fleet;
        fleet.push_back(make_unique<GroundConvoy>(
            "TR-101", "Rescue Truck", 5000, "North Depot"));
        fleet.push_back(make_unique<AerialTransport>(
            "HL-201", "Medical Helicopter", 900, "City Helipad"));
        fleet.push_back(make_unique<AmphibiousCraft>(
            "HY-301", "Amphibious Medical Unit", 1500, "River Checkpoint"));

        cout << "\nAvailable vehicles\n";
        for (const auto& vehicle : fleet)
            cout << *vehicle << "\n";

        double distance = 42.0;
        updateMissionDistance(distance, 8.0);
        cout << "\nUpdated mission distance: " << distance << " km\n";

        cout << "\nTravel estimates for a 50 km mission\n";
        for (const auto& vehicle : fleet)
            cout << vehicle->getName() << ": "
                 << fixed << setprecision(2)
                 << vehicle->estimateTravelTime(50) << " hours\n";

        PriorityDispatchQueue<Mission> dispatchQueue;
        dispatchQueue.push({"Deliver trauma kits", 10, "District Hospital"});
        dispatchQueue.push({"Deliver drinking water", 6, "Village Camp"});
        dispatchQueue.push({"Evacuate injured people", 9, "Hill Sector"});

        cout << "\nPriority dispatch list\n";
        cout << urgentTable;
        vector<Mission> missions;
        while (!dispatchQueue.empty())
            missions.push_back(dispatchQueue.pop());

        sort(missions.begin(), missions.end(), UrgencyFirst());
        for_each(missions.begin(), missions.end(), [](const Mission& mission) {
            cout << left << setw(24) << mission.name
                 << setw(12) << mission.urgency
                 << mission.destination << "\n";
        });

        ReliefCargo water("Water", 40, 1.0, 1);
        fleet[0]->loadCargo(water);
        fleet[0]->dispatch(50, true);
        saveDispatchLog("dispatch_log.dat",
                        "Rescue Truck delivered water to the relief zone.");

        cout << "\nVehicles currently operational: "
             << ReliefVehicle::totalOperationalVehicles << "\n";
        cout << "Total dispatched cargo: "
             << ReliefVehicle::totalDispatchedTonnage << " tonnes\n";

        showDispatchLog("dispatch_log.dat");

        cout << "\nProject demonstration completed successfully.\n";
    }
    catch (const exception& error) {
        cerr << "\nError: " << error.what() << "\n";
        return 1;
    }
    return 0;
}
