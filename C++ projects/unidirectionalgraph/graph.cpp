#include "fitness.h"

using namespace std;

int main(){

    mt19937 randEngine(random_device{}());
    
    /* Sample cyclic graph 1 -> 3 -> 2 -> 1

    vector<Graph::Node> nodelist = {Graph::Node(0, 'i', 0, 0), Graph::Node(1, 'h', 0, 0), Graph::Node(2, 'h', 0, 0), Graph::Node(3, 'h', 0, 0), Graph::Node(4, 'o', 0, 0)};
    vector<Graph::Connection> cnctnlist = {Graph::Connection(0, 0, 1, 0, true), Graph::Connection(1, 1, 3, 0, true), Graph::Connection(2, 2, 1, 0, true), Graph::Connection(3, 3, 2, 0, true), Graph::Connection(4, 3, 4, 0, true)};

    Graph graph = Graph(nodelist, cnctnlist);
    graph.node_inno_number = 4;
    graph.connection_inno_number = 4;

    graph.updateDepths();
    */

    /* Sample normal graph
    vector<Graph::Node> nodelist = {Graph::Node(0, 'i', 0, 0), Graph::Node(1, 'h', 0, 0), Graph::Node(2, 'h', 0, 0), Graph::Node(3, 'h', 0, 0), Graph::Node(4, 'o', 0, 0)};
    vector<Graph::Connection> cnctnlist = {Graph::Connection(0, 0, 1, 0, true), Graph::Connection(1, 1, 3, 0, true), Graph::Connection(2, 1, 2, 0, true), Graph::Connection(3, 2, 3, 0, true), Graph::Connection(4, 3, 4, 0, true)};

    Graph graph = Graph(nodelist, cnctnlist);
    graph.node_inno_number = 4;
    graph.connection_inno_number = 4;

    graph.updateDepths();

    graph.prettyPrint();
    */
    
    /*
    vector<Graph::Node> nodelist = {Graph::Node(0, 'i', 0, 0), Graph::Node(1, 'h', 0, 0), Graph::Node(2, 'h', 0, 0), Graph::Node(3, 'h', 0, 0), Graph::Node(4, 'o', 0, 0)};
    vector<Graph::Connection> cnctnlist = {Graph::Connection(0, 0, 1, 0, true), Graph::Connection(1, 1, 3, 0, true), Graph::Connection(2, 1, 2, 0, true), Graph::Connection(3, 2, 3, 0, true), Graph::Connection(4, 3, 4, 0, true)};

    Graph graph1 = Graph(nodelist, cnctnlist);
    this_thread::sleep_for(std::chrono::milliseconds(1000)); //Since the random devices are initialized based on time(0), this delay ensures that different seeds are given to each agent
    Graph graph2 = Graph(nodelist, cnctnlist);

    Population pop = Population({graph1, graph2}, 4, 4);
    pop.updatePerturbationSettings(0.1, 0.1, -1, 1);

    for(int i = 0; i < pop.agents.size(); ++i){
        for(int j = 0; j < 2; ++j){
            pop.insertRandNode(i);
            pop.perturbWeights(i);
            pop.randConnection(i);
        }
    }

    for(int i = 0; i < pop.agents.size(); ++i){
        pop.agents[i].prettyPrint();
    }

    pop.crossover(0, 1).prettyPrint();
    */

    /*
    Graph graph1 = Graph({Graph::Node(0, 'i', 0, 0, 'x'), Graph::Node(1, 'h', 0, 1, 'l'), Graph::Node(2, 'h', 0, -1, 'l'), Graph::Node(3, 'o', 0, -2, 'l'), Graph::Node(4, 'o', 0, -2, 'l')}, {Graph::Connection(0, 0, 1, 0.5, true), Graph::Connection(1, 0, 2, 1, true), Graph::Connection(2, 1, 3, 3, true), Graph::Connection(3, 2, 3, -1.7, true), Graph::Connection(4, 2, 4, 2, true), Graph::Connection(5, 0, 4, -0.5, true)});
    
    node_value_pair input1;
    input1.id = 0;
    input1.value = 1;

    vector<node_value_pair> result = compute(graph1, {input1});

    cout << result[0].id << " " << result[0].value << "\n";
    cout << result[1].id << " " << result[1].value << "\n";
    */

    //TODO: NEAT framework is done and seems to be working! XOR test to be done
    Graph initial = Graph({Graph::Node(0, 'i', 0, 0, 'x'), Graph::Node(1, 'i', 1, 0, 'x'), Graph::Node(2, 'o', 1, 0, 'l')}, {Graph::Connection(0, 0, 2, 0, true), Graph::Connection(1, 1, 2, 0, true)}); //Initialize a single graph with 2 inputs and 1 output, with no hidden neurons. Then add 50 instances in a new population and do He initialization
    Population pop = Population({}, 2, 1);

    for(int i = 0; i < 50; ++i){
        pop.agents.push_back(initial);
    }

    pop.initialize_he();

    //Test each agent for XOR(0,0), XOR(0,1), XOR(1,0), XOR(1,1) per generation.
    
    node_value_pair in1_00; in1_00.id = 0; in1_00.value = 0;
    node_value_pair in2_00; in2_00.id = 1; in2_00.value = 0;
    node_value_pair out1_00; out1_00.id = 2; out1_00.value = 0;

    node_value_pair in1_01; in1_01.id = 0; in1_01.value = 0;
    node_value_pair in2_01; in2_01.id = 1; in2_01.value = 1;
    node_value_pair out1_01; out1_01.id = 2; out1_01.value = 1;

    node_value_pair in1_10; in1_10.id = 0; in1_10.value = 1;
    node_value_pair in2_10; in2_10.id = 1; in2_10.value = 0;
    node_value_pair out1_10; out1_10.id = 2; out1_10.value = 1;

    node_value_pair in1_11; in1_11.id = 0; in1_11.value = 1;
    node_value_pair in2_11; in2_11.id = 1; in2_11.value = 1;
    node_value_pair out1_11; out1_11.id = 2; out1_11.value = 0;

    vector<node_value_pair> in_00 = {in1_00, in2_00};
    vector<node_value_pair> out_00 = {out1_00};

    vector<node_value_pair> in_01 = {in1_01, in2_01};
    vector<node_value_pair> out_01 = {out1_01};

    vector<node_value_pair> in_10 = {in1_10, in2_10};
    vector<node_value_pair> out_10 = {out1_10};

    vector<node_value_pair> in_11 = {in1_11, in2_11};
    vector<node_value_pair> out_11 = {out1_11};

    for(int gen = 0; gen < 50; ++gen){ //Each gen, take top 5, take mutated versions of top 20, take 25 randomly crossed and mutated children

        double fitnesses[pop.agents.size()];

        for(int i = 0; i < pop.agents.size(); ++i){
            fitnesses[i] = fitness(compute(pop.agents[i], in_00), out_00);
            fitnesses[i] += fitness(compute(pop.agents[i], in_01), out_01);
            fitnesses[i] += fitness(compute(pop.agents[i], in_10), out_10);
            fitnesses[i] += fitness(compute(pop.agents[i], in_11), out_11);
        }

        for(int i = 0; i < 50; ++i){
            int maxindex = i;
            for(int j = i; j < 50; ++j){
                if(fitnesses[j] > fitnesses[maxindex]){
                    maxindex = j;
                }
            }
            swap(fitnesses[maxindex], fitnesses[i]);
            swap(pop.agents[maxindex], pop.agents[i]);
        }

        cout << "Generation " << gen << " best agent fitness: " << fitnesses[0] << "\n" << "Best agent architecture: ";
        pop.agents[0].prettyPrint();

        Population pop_new = Population({}, pop.node_inno_number, pop.connection_inno_number);

        //Add top 5 from prev. gen
        pop_new.agents.push_back(pop.agents[0]);
        pop_new.agents.push_back(pop.agents[1]);
        pop_new.agents.push_back(pop.agents[2]);
        pop_new.agents.push_back(pop.agents[3]);
        pop_new.agents.push_back(pop.agents[4]);

        for(int i = 0; i < 20; ++i){
            uniform_int_distribution<int> dist(0, 99);
            int mutationrand = dist(randEngine);
            if(0 <= mutationrand && mutationrand < 2){
                pop_new.agents.push_back(pop.agents[i]);
                pop_new.insertRandNode(i+5);
            } else if(2 <= mutationrand && mutationrand < 10){
                pop_new.agents.push_back(pop.agents[i]);
                pop_new.randConnection(i+5);
            } else if(10 <= mutationrand && mutationrand < 100){
                pop_new.agents.push_back(pop.agents[i]);
                pop_new.perturbWeights(i+5);
            }
        }

        for(int i = 0; i < 25; ++i){
            uniform_int_distribution<int> dist(0, 29);

            int index1 = dist(randEngine);
            int index2 = dist(randEngine);

            if(index1 < index2){
                pop_new.agents.push_back(pop.crossover(index1, index2)); //TODO: Perturb weights of child
            } else {
                pop_new.agents.push_back(pop.crossover(index2, index1));
            }

        }

        pop = pop_new;

        cout << "Enter to proceed: \n";

    }

    return 0;
}