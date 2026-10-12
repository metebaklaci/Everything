#include <bits/stdc++.h>

using namespace std;


class Node{
    public:

    double capacitance; //F
    double potential = 0; //V

    Node(double capacitance){
        this->capacitance = capacitance;
    }

};

class Connection{
    public:

    int node_A;
    int node_B;

    Connection(int node_A, int node_B){
        this->node_A = node_A;
        this->node_B = node_B;
    }

};

class Wire : public Connection{
    public:

    double resistance;

    Wire(int node_A, int node_B, double resistance) : Connection(node_A, node_B), resistance(resistance){}

};

class Battery : public Connection{
    public:

    double resistance;
    double voltage;

    Battery(int node_A, int node_B, double resistance, double voltage) : Connection(node_A, node_B), resistance(resistance), voltage(voltage){}

};

class Ammeter{
    public: 

    double reading = 0; //A
    int connectionID;

    Ammeter(int connectionID) : connectionID(connectionID){}

};


class Circuit{
    public:

    vector<Node> nodes;
    vector<Wire> wires;
    vector<Battery> batteries;
    vector<Ammeter> ammeters;


    Circuit(vector<Node> nodes, vector<Wire> wires, vector<Battery> batteries, vector<Ammeter> ammeters) : nodes(nodes), wires(wires), batteries(batteries), ammeters(ammeters){}

    Circuit timestep(double step /* s */){
        Circuit newCircuit(this->nodes, this->wires, this->batteries, this->ammeters);

        for(int i = 0; i < wires.size(); ++i){
            double pd = nodes[wires[i].node_A].potential - nodes[wires[i].node_B].potential; //V
            double current = pd/wires[i].resistance; //A
            newCircuit.nodes[wires[i].node_B].potential += current*step/nodes[wires[i].node_B].capacitance;
            newCircuit.nodes[wires[i].node_A].potential -= current*step/nodes[wires[i].node_A].capacitance;

            for(Ammeter& ammeter : newCircuit.ammeters){
                if(ammeter.connectionID == i){
                    ammeter.reading = current;
                }
            }
        }

        for(int i = 0; i < batteries.size(); ++i){
            double pd = nodes[batteries[i].node_A].potential - nodes[batteries[i].node_B].potential + batteries[i].voltage; //V
            double current = pd/batteries[i].resistance; //A
            newCircuit.nodes[batteries[i].node_B].potential += current*step/nodes[batteries[i].node_B].capacitance;
            newCircuit.nodes[batteries[i].node_A].potential -= current*step/nodes[batteries[i].node_A].capacitance;
        }

        return newCircuit;
    }

    void print(){
        cout << "Components:\nBatteries:\n";
        for(Battery battery : batteries){
            cout << "Voltage: " << battery.voltage << "   Internal Resistance: " << battery.resistance << "   From " << battery.node_A << " to " << battery.node_B << "\n";
        }
        cout << "Wires and resistances:\n";
        for(Wire wire : wires){
            cout << "Internal Resistance: " << wire.resistance << "   From " << wire.node_A << " to " << wire.node_B << "\n";
        }

        cout << "\nPoints along the circuit:\n";
        for(int i = 0; i < nodes.size(); ++i){
            Node node = nodes[i];
            cout << "Index: " << i << "   Capacitance: " << node.capacitance << "   Potential: " << node.potential << "\n";
        }

        cout << "\nAmmeter readings:\n";
        for(int i = 0; i < ammeters.size(); ++i){
            Ammeter ammeter = ammeters[i];
            cout << "Index: " << i << "   Reading: " << ammeter.reading << "   From " << wires[ammeter.connectionID].node_A << " to " << wires[ammeter.connectionID].node_B << "\n";
        }
    }

};


int main(){
    vector<Node> nodes = {
        Node(0.001),
        Node(0.001),
        Node(0.001),
        Node(0.001),
        Node(0.001),
        Node(0.001),
        Node(0.001),
        Node(0.001)
    };

    vector<Wire> wires = {
        Wire(0, 1, 0.01),
        Wire(1, 2, 0.01),
        Wire(2, 3, 1000),
        Wire(3, 4, 0.01),
        Wire(4, 5, 0.01),
        Wire(6, 1, 0.01),
        Wire(4, 7, 0.01)
    };

    vector<Battery> batteries = {
        Battery(5, 0, 0.01, 2),
        Battery(7, 6, 0.01, 2)
    };

    vector<Ammeter> ammeters = {
        Ammeter(5),
        Ammeter(0),
        Ammeter(1)
    };

    Circuit circuit(nodes, wires, batteries, ammeters);

    for(int i = 0; i < 10000; ++i){
        if(i % 100 == 0) {
            circuit.print();
            cout << "\n------------------------------------------------------------------\n\n";
        }
        circuit = circuit.timestep(0.000001);
    }
}