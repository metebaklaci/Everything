//MAJOR TODO: (maybe) Phase out nodes, switch to implied node architecture
//TODO: Add bias perturbation mutation (mirror logic for connection weight perturbation)
//TODO: Add enable/disable mutation

#include <bits/stdc++.h>

#ifndef AGENT_H
#define AGENT_H

using namespace std;

struct ConnectionInnovation{ //Connection innovation
    int in;
    int out;
    int innovation_id;

    ConnectionInnovation(int _in, int _out, int _innovation_id){
        in = _in;
        out = _out;
        innovation_id = _innovation_id;
    }
};

struct NodeSplitInnovation{ //Connection innovation
    int split_cnctn_id;
    int innovation_id;

    int new_cnctn_1;
    int new_cnctn_2;

    NodeSplitInnovation(int _split_cnctn_id, int _innovation_id, int _new_cnctn_1, int _new_cnctn_2){
        split_cnctn_id = _split_cnctn_id;
        innovation_id = _innovation_id;

        new_cnctn_1 = _new_cnctn_1;
        new_cnctn_2 = _new_cnctn_2;
    }
};

class Graph{
    public:
        class Node{
            public:
                int id = 0;
                char type = 'h'; //'i' = input, 'h' = hidden, 'o' = output
                int depth = 0;
                double bias = 0;
                char activation = 'x'; //'x' = linear (x=y), 's' = sigmoid, 'r' = ReLU, 'l' = LReLU, 't' = tanh

                Node(int _id, char _type, int _depth, double _bias, char _activation){
                    id = _id;
                    type = _type;
                    depth = _depth;
                    bias = _bias;
                    activation = _activation;
                }
        };
        class Connection{
            public:
                int id = 0; //Connection's ID
                int nodein = 0; //ID of incoming node
                int nodeout = 0; //ID of outgoing node
                double weight = 0; //Other metadata is suitable
                bool enabled = true; //Whether connection is activated

                Connection(int _id, int _nodein, int _nodeout, double _weight, int _enabled){
                    id = _id;
                    nodein = _nodein;
                    nodeout = _nodeout;
                    weight = _weight;
                    enabled = _enabled;
                }
        };

        //Agent structure:

        vector<Node> nodes = {};
        unordered_map<int, int> node_id_to_index = {};

        vector<Connection> connections = {};
        unordered_map<int, int> connection_id_to_index = {};

        //Utilities:
        mt19937 rng;
    
        //Methods
    public:

        void updateRng(){
            random_device rd;
            rng.seed(rd());
        }

        void updateMaps(){
            //Flush maps and refill
            node_id_to_index.clear();
            connection_id_to_index.clear();

            for(int i = 0; i < nodes.size(); ++i){
                node_id_to_index.insert({nodes[i].id, i});
            }

            for(int i = 0; i < connections.size(); ++i){
                connection_id_to_index.insert({connections[i].id, i});
            }
        }

        void updateDepths(){

            if(cyclicTest()) throw runtime_error("Tried to update depths on cyclic graph");

            vector<int> ids = {};
            for(int i = 0; i < nodes.size(); ++i){
                if(nodes[i].type == 'o'){
                    ids.push_back(nodes[i].id);
                }
            }
            
            for(int i = 0; i < ids.size(); ++i){
                depthrecurse(ids[i]);
            }
        }

        Graph(vector<Node> _nodes, vector<Connection> _connections){ //Initialized constructor
            nodes = _nodes;
            connections = _connections;
            updateMaps();
            updateDepths();
            updateRng();
        }

        Graph(){ //Uninitialized constructor
            updateRng();
        }

        void prettyPrint(){
            cout << "\nNodes:\n";
            for(int i = 0; i < nodes.size(); ++i){
                cout << "ID: " << nodes[i].id << "     ";
                cout << "Type: " << nodes[i].type << "     ";
                cout << "Depth: " << nodes[i].depth << "     ";
                cout << "Bias: " << nodes[i].bias << "\n";
            }
            cout << "\nConnections:\n";
            for(int i = 0; i < connections.size(); ++i){
                cout << "ID: " << connections[i].id << "     ";
                cout << "Input: " << connections[i].nodein << "     ";
                cout << "Output: " << connections[i].nodeout << "     ";
                cout << "Weight: " << connections[i].weight << "     ";
                cout << "Enabled: " << connections[i].enabled << "\n";
            }
            cout << "\n";
        }

    private:
        bool cyclicrecurse(int _id, vector<int> path){
            int index = node_id_to_index[_id];

            if(nodes[index].type == 'i') return false; //If node at hand is an input node, return false as cyclicity is not possible

            vector<int> cnctnindex = {};

            for(int i = 0; i < connections.size(); ++i){
                if(connections[i].nodeout == _id){
                    cnctnindex.push_back(i);

                    for(int j = 0; j < path.size(); ++j){
                        if(connections[i].nodein == path[j]) return true; //If nodes that have connections to node at hand are in the path, return true as we have detected cyclicity
                    }
                }
            }

            for(int i = 0; i < cnctnindex.size(); ++i){
                vector<int> newpath = path; //Copy path
                newpath.push_back(_id); //Add node at hand to path for recursive call

                if(cyclicrecurse(connections[cnctnindex[i]].nodein, newpath)) return true; //If cyclicity is present in one of the nodes that link to the node at hand, propagate that
            }

            return false; //If no cyclicity has been detected and none of the nodes that link to this node have detected cyclicity, return false
        }
    
    public:
        bool cyclicTest(){ //Test for cyclicity in graph. This test assumes there are no connections going into input nodes or exiting output nodes. False is no cycles are present.
            for(int i = 0; i < nodes.size(); ++i){
                if(cyclicrecurse(nodes[i].id, {})) return true; //Call recursive test function on all output nodes which will propagate down to hidden and input neurons, if one detects cyclicity, return true
            }

            return false; //If no cyclicity is detected, return false
        }
        
        bool inputOutputTest(){ //Validity check for no incoming connections to input nodes and no outgoing connections from output nodes. True is test is passed.
            for(int i = 0; i < nodes.size(); ++i){

                if(nodes[i].type == 'i'){
                    for(int j = 0; j < connections.size(); ++j){
                        if(connections[j].nodeout == nodes[i].id) return false; //If the output end of a connection is an input node, fail test
                    }
                }

                if(nodes[i].type == 'o'){
                    for(int j = 0; j < connections.size(); ++j){
                        if(connections[j].nodein == nodes[i].id) return false; //If the input end of a connection is an output node, fail test
                    }
                }

            }

            return true;
        }

    private: 
        int depthrecurse(int _id){

            int index = node_id_to_index[_id];

            if(nodes[index].type == 'i'){
                nodes[index].depth = 0;
                return 1;
            }

            vector<int> ids = {};
            for(int i = 0; i < connections.size(); ++i){ //Add the indexes of the input nodes of connections that lead to node with id _id to ids
                if(connections[i].nodeout == _id){
                    ids.push_back(connections[i].nodein);
                }
            }
            int maxindex = 0;
            for(int id : ids){
                int depth = depthrecurse(id);
                if(depth > maxindex){
                    maxindex = depth;
                }
            }

            nodes[index].depth = maxindex;

            return maxindex + 1;
        }

    private:
        void insertnode(int _id/*Connection inno id to split*/, int& node_inno_number, int& connection_inno_number, vector<NodeSplitInnovation>* node_lookup){
            int index = connection_id_to_index[_id];
            int node_in = connections[index].nodein;
            int node_out = connections[index].nodeout;
            double weight = connections[index].weight;

            //If the innovation is already found in node_lookup, necessary info will be stored in the variables below.
            int foundinno_id = -1;
            int foundinno_cnctnin = -1;
            int foundinno_cnctnout = -1;

            for(NodeSplitInnovation inno : *node_lookup){
                if(inno.split_cnctn_id == _id){
                    foundinno_id = inno.innovation_id;
                    foundinno_cnctnin = inno.new_cnctn_1;
                    foundinno_cnctnout = inno.new_cnctn_2;
                    break;
                }
            }
            
            connections[index].enabled = false;

            if(foundinno_id == -1){ //Node innovation not found in node_lookup
                node_inno_number++;
                nodes.push_back(Graph::Node(node_inno_number, 'h', 0, 0, 'l'));

                connection_inno_number++;
                connections.push_back(Graph::Connection(connection_inno_number, node_in, node_inno_number, weight, true));
                connection_inno_number++;
                connections.push_back(Graph::Connection(connection_inno_number, node_inno_number, node_out, 1, true));

                //New inno is found, should be added to node_lookup
                (*node_lookup).push_back(NodeSplitInnovation(_id, node_inno_number, connection_inno_number - 1, connection_inno_number));

            } else { //Node innovation found in node_lookup
                nodes.push_back(Graph::Node(foundinno_id, 'h', 0, 0, 'l'));

                connections.push_back(Graph::Connection(foundinno_cnctnin, node_in, foundinno_id, weight, true));
                connections.push_back(Graph::Connection(foundinno_cnctnout, foundinno_id, node_out, 1, true));

                //Did not update inno numbers if existing innovation is found
            }

            updateMaps();
            updateDepths();
        }



    public:
        void insertRandNode(int& node_inno_number, int& connection_inno_number, vector<NodeSplitInnovation>* node_lookup){
            
            vector<int> enabled_connections;

            for(int i = 0; i < connections.size(); ++i){
                if(connections[i].enabled){
                    enabled_connections.push_back(i);
                }
            }

            if(enabled_connections.empty()) throw runtime_error("Tried to add node to agent with no enabled connections");
            
            
            uniform_int_distribution<int> dist(0, enabled_connections.size() - 1);
            int index = dist(rng);
            insertnode(connections[enabled_connections[index]].id, node_inno_number, connection_inno_number, node_lookup);
        }

        void randConnection(int& node_inno_number, int& connection_inno_number, vector<ConnectionInnovation>* inno_lookup){
            
            vector< pair<int, int> > candidates;

            for(const Node& n1 : nodes){
                for(const Node& n2 : nodes){

                    if(n1.id == n2.id) continue;

                    if(n1.type == 'o') continue;
                    if(n2.type == 'i') continue;

                    if(n1.depth >= n2.depth) continue;

                    bool exists = false;

                    for(const Connection& c : connections){
                        if(c.nodein == n1.id && c.nodeout == n2.id){
                            exists = true;
                            break;
                        }
                    }

                    if(!exists){
                        candidates.push_back({n1.id, n2.id});
                    }
                }
            }

            if(candidates.empty()) throw runtime_error("Tried to add connection to dense agent"); //TODO: Maybe add a setting where programmer may choose to just skip (return) instead of throwing exception

            /*
            uniform_int_distribution<int> dist(0, nodes.size() - 1);
            int node1 = nodes[dist(rng)].id;
            int node2 = nodes[dist(rng)].id;
            bool existingcnctn = false;

            for(int i = 0; i < connections.size(); ++i){ //Check if connection from node1 to node2 exists
                if(connections[i].nodein == node1 && connections[i].nodeout == node2){
                    existingcnctn = true;
                    break;
                }
            }
            
            while(nodes[node_id_to_index[node1]].depth > nodes[node_id_to_index[node2]].depth //Case where node1 is deeper than node2
            || existingcnctn //Case where there is a connection from node1 to node2
            || node1 == node2){ //Case where node1 and node2 are identical

                node1 = nodes[dist(rng)].id;
                node2 = nodes[dist(rng)].id;
                existingcnctn = false;

                for(int i = 0; i < connections.size(); ++i){ //Check if connection from node1 to node2 exists
                    if(connections[i].nodein == node1 && connections[i].nodeout == node2){
                        existingcnctn = true;
                        break;
                    }
                }
            }

            */

            uniform_int_distribution<int> dist(0, candidates.size() - 1);
            pair<int, int> selected = candidates[dist(rng)];

            int foundinno = -1;
            for(int i = 0; i < (*inno_lookup).size(); ++i){
                if((*inno_lookup)[i].in == selected.first && (*inno_lookup)[i].out == selected.second){
                    foundinno = (*inno_lookup)[i].innovation_id;
                    break;
                }
            }

            if(foundinno == -1){
                connection_inno_number++;
                connections.push_back(Connection(connection_inno_number, selected.first, selected.second, 0, true));
                (*inno_lookup).push_back(ConnectionInnovation(selected.first, selected.second, connection_inno_number));
            } else {
                connections.push_back(Connection(foundinno, selected.first, selected.second, 0, true));
            }
            updateMaps();
            updateDepths();
        }

    private:
        void perturbweight(int id, double mutation_std_dev){
            int index = connection_id_to_index[id];

            normal_distribution<double> dist(0, mutation_std_dev);
            
            connections[index].weight += dist(rng);
        }

    public:
        void perturbWeights(double reset_probability, double mutation_std_dev, double weight_reset_lower, double weight_reset_upper){ //TODO: update biases as well
            
            uniform_real_distribution<double> prob(0, 1);

            for(int i = 0; i < connections.size(); ++i){
                if(reset_probability > prob(rng)){
                    uniform_real_distribution<double> dist(weight_reset_lower, weight_reset_upper); //Uniform distribution [-1,1)
                    connections[i].weight = dist(rng); 
                } else {
                    perturbweight(connections[i].id, mutation_std_dev);
                }
            }
        }

        void initialize_he(){ //Works optimally for ReLU / LReLU
            
            int fan_in = 0;
            for(int i = 0; i < nodes.size(); ++i){
                if(nodes[i].type == 'i') fan_in++;
            }

            normal_distribution<double> he_dist(0, sqrt(2.0/fan_in));

            for(Connection& cnctn : connections){
                cnctn.weight = he_dist(rng);
            }

            for(Node& nd : nodes){
                nd.bias = 0;
            }

        }

};





class Population{

    public:

        vector<Graph> agents = {};
        
        vector<int> species = {}; //Parallel to agents
        vector< pair<int, int> > representatives = {}; //First in pair: species ID. Second in pair: representative agent index in population

        int node_inno_number = 0;
        int connection_inno_number = 0;
        vector<ConnectionInnovation> connection_inno_lookup = {};
        vector<NodeSplitInnovation> node_inno_lookup = {};

        Population(vector<Graph> _agents, int node_inno, int connection_inno){
            agents = _agents;
            node_inno_number = node_inno;
            connection_inno_number = connection_inno;
        }

        mt19937 rng;

        void updateRng(){
            random_device rd;
            rng.seed(rd());
        }

    private:
        double mutation_std_dev = 0.1;
        double reset_probability = 0.1;
        double weight_reset_lower = -1;
        double weight_reset_upper = 1;

        //Speciation distance formula tuning constants
        double tuning_excessgene = 1;
        double tuning_disjointgene = 1;
        double tuning_avgweightdiff = 0.4;
        
        double genome_distance_threshold = 3; //Distance threshold for considering two agents as same species

        //randMutate probabilities
        double prob_perturbation = 0.90;
        double prob_connection = 0.07;
        double prob_node = 0.03;

    public:
        /*
         * Updates weight perturbation/reset settings.
         * @param stddev Standard deviation of perturbation value
         * @param resetProbability Probability for a connection reset to occur rather than perturbation
         * @param reset_lower Lower bound of uniform distribution during weight reset
         * @param reset_upper Upper bound of uniform distribution during weight reset
         */
        void updatePerturbationSettings(double stddev, double resetProbability, double reset_lower, double reset_upper){
            if(resetProbability < 0 || resetProbability > 1) throw runtime_error("Reset probability cannot be negative or >1");
            if(stddev <= 0) throw runtime_error("Standard deviation cannot be negative or 0");

            reset_probability = resetProbability;
            mutation_std_dev = stddev;
            weight_reset_lower = reset_lower;
            weight_reset_upper = reset_upper;
        }

        /*
         * Updates tuning constants for genome distance calculations.
         * @param excess Tuning value for the importance of excess genes in genome distance
         * @param disjoint Tuning value for the importance of disjoint genes in genome distance
         * @param weightdiff Tuning value for the importance of connection weight differences in genome distance
         */
        void updateTuningConstants(double excess, double disjoint, double weightdiff){
            tuning_excessgene = excess;
            tuning_disjointgene = disjoint;
            tuning_avgweightdiff = weightdiff;
        }

        /*
         * Updates mutation probabilities for randMutate() function.
         * @throws Runtime error is thrown if probabilities do not add to 1 (with toleration of 0.00005 to account for precision errors)
         * @param perturbation Probability of carrying out connection perturbation upon randMutate() call
         * @param connection Probability of carrying out new connection mutation upon randMutate() call
         * @param node Probability of carrying out node insertion mutation upon randMutate() call
         */
        void updateMutationProbabilities(double perturbation, double connection, double node){
            if(round((perturbation + connection + node)*10000) != 10000) throw runtime_error("Probabilities in update function must add to 1.");

            prob_perturbation = perturbation;
            prob_connection = connection;
            prob_node = node;

        }

        void insertRandNode(int index){ //Insert a random node to member of population at index
            agents[index].insertRandNode(node_inno_number, connection_inno_number, &node_inno_lookup);
        }

        void randConnection(int index){ //Add a random connection to member of population at index
            agents[index].randConnection(node_inno_number, connection_inno_number, &connection_inno_lookup);
        }

        void perturbWeights(int index){ //Perturb or reset all connections in member of population at index
            agents[index].perturbWeights(reset_probability, mutation_std_dev, weight_reset_lower, weight_reset_upper);
        }

        /* Crosses over 2 parents to form a child. Deterministic.
         * @param fitparent The parent with a higher fitness score. Child will be structurally identical to this parent.
         * @param unfitparent The parent with a lower fitness score. Child will be structurally different from this parent.
         * @param inno_nodes Global innovation number for nodes.
         * @param inno_cnctns Global innovation number for connections.
         */
        Graph crossover(int fitparent_index, int unfitparent_index){

            uniform_int_distribution<int> dist(0, 1);

            vector<bool> fitgenomenodes(node_inno_number + 1, false);
            vector<bool> unfitgenomenodes(node_inno_number + 1, false);
            vector<bool> fitgenomecnctns(connection_inno_number + 1, false);
            vector<bool> unfitgenomecnctns(connection_inno_number + 1, false);

            for(int i = 0; i < agents[fitparent_index].nodes.size(); ++i){
                fitgenomenodes[agents[fitparent_index].nodes[i].id] = true;
            }
            for(int i = 0; i < agents[unfitparent_index].nodes.size(); ++i){
                unfitgenomenodes[agents[unfitparent_index].nodes[i].id] = true;
            }
            for(int i = 0; i < agents[fitparent_index].connections.size(); ++i){
                fitgenomecnctns[agents[fitparent_index].connections[i].id] = true;
            }
            for(int i = 0; i < agents[unfitparent_index].connections.size(); ++i){
                unfitgenomecnctns[agents[unfitparent_index].connections[i].id] = true;
            }

            vector<Graph::Node> child_nodes = {};
            vector<Graph::Connection> child_cnctns = {};

            for(int i = 0; i <= node_inno_number; ++i){
                if(fitgenomenodes[i] && unfitgenomenodes[i]){
                    int choice = dist(rng);
                    Graph::Node newnode = Graph::Node(0,0,0,0,0);
                    if(choice == 0)      newnode = Graph::Node(i, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].type, 0, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].bias, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].activation);
                    else if(choice == 1) newnode = Graph::Node(i, agents[unfitparent_index].nodes[agents[unfitparent_index].node_id_to_index[i]].type, 0, agents[unfitparent_index].nodes[agents[unfitparent_index].node_id_to_index[i]].bias, agents[unfitparent_index].nodes[agents[unfitparent_index].node_id_to_index[i]].activation);
                    child_nodes.push_back(newnode);
                } else if (fitgenomenodes[i]){
                    Graph::Node newnode = Graph::Node(i, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].type, 0, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].bias, agents[fitparent_index].nodes[agents[fitparent_index].node_id_to_index[i]].activation);
                    child_nodes.push_back(newnode);
                }
            }

            for(int i = 0; i <= connection_inno_number; ++i){
                if(fitgenomecnctns[i] && unfitgenomecnctns[i]){
                    int choice = dist(rng);
                    Graph::Connection newcnctn = Graph::Connection(0,0,0,0,0);
                    uniform_real_distribution<double> prob(0,1);
                    if(choice == 0)      newcnctn = Graph::Connection(i, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].nodein, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].nodeout, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].weight, (agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].enabled && agents[unfitparent_index].connections[agents[unfitparent_index].connection_id_to_index[i]].enabled) || prob(rng) < 0.25);
                    else if(choice == 1) newcnctn = Graph::Connection(i, agents[unfitparent_index].connections[agents[unfitparent_index].connection_id_to_index[i]].nodein, agents[unfitparent_index].connections[agents[unfitparent_index].connection_id_to_index[i]].nodeout, agents[unfitparent_index].connections[agents[unfitparent_index].connection_id_to_index[i]].weight, (agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].enabled && agents[unfitparent_index].connections[agents[unfitparent_index].connection_id_to_index[i]].enabled) || prob(rng) < 0.25);
                    child_cnctns.push_back(newcnctn);
                } else if (fitgenomecnctns[i]){
                    Graph::Connection newcnctn = Graph::Connection(i, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].nodein, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].nodeout, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].weight, agents[fitparent_index].connections[agents[fitparent_index].connection_id_to_index[i]].enabled);
                    child_cnctns.push_back(newcnctn);
                }
            }

            Graph child = Graph(child_nodes, child_cnctns);
            return child;
        }

        void initialize_he(){ //Works optimally for ReLU / LReLU
            for(int i = 0; i < agents.size(); ++i){
                agents[i].initialize_he();
            }
        }

        /*
         * Returns a vector that is parallel to agents. Each subvector is the set of outputs of the respective agent.
         * @param inputs Inputs to each agent. This vector should be parallel to agents.
         */
        vector< vector<node_value_pair> > compute_all(vector< vector <node_value_pair> > inputs){ 

            if(inputs.size() != agents.size()) throw runtime_error("Size mismatch between inputs and size of population.");

            vector< vector<node_value_pair> > returner(agents.size());
            for(int i = 0; i < agents.size(); ++i){
                returner[i] = compute(agents[i], inputs[i]);
            }
            return returner;
        }
        
        double genomeRawDistance(int agent1_index, int agent2_index){
            double normalizer = max(agents[agent1_index].connections.size(), agents[agent2_index].connections.size()); //normalize number of excess/disjoint nodes by the max genome size
            
            if(normalizer < 20) normalizer = 1;

            vector<bool> agent1genomecnctns(connection_inno_number + 1, false);
            vector<bool> agent2genomecnctns(connection_inno_number + 1, false);

            for(int i = 0; i < agents[agent1_index].connections.size(); ++i){
                agent1genomecnctns[agents[agent1_index].connections[i].id] = true;
            }
            for(int i = 0; i < agents[agent2_index].connections.size(); ++i){
                agent2genomecnctns[agents[agent2_index].connections[i].id] = true;
            }
            
            double excess = 0;
            double disjoint = 0;
            double weight_diff = 0;

            double agent1_maxinno = 0;
            double agent2_maxinno = 0;

            for(int i = connection_inno_number; i >= 0; --i){
                if(agent1genomecnctns[i]){
                    agent1_maxinno = i;
                    break;
                }
            }

            for(int i = connection_inno_number; i >= 0; --i){
                if(agent2genomecnctns[i]){
                    agent2_maxinno = i;
                    break;
                }
            }

            for(int i = 0; i <= min(agent1_maxinno, agent2_maxinno); ++i){
                if(agent1genomecnctns[i] ^ agent2genomecnctns[i]) disjoint++;
            }

            for(int i = min(agent1_maxinno, agent2_maxinno) + 1; i < agent1genomecnctns.size(); ++i){
                if(agent1genomecnctns[i] || agent2genomecnctns[i]) excess++;
            }

            int homologous_count = 0;
            for(int i = 0; i <= min(agent1_maxinno, agent2_maxinno); ++i){
                if(agent1genomecnctns[i] && agent2genomecnctns[i]){
                    homologous_count++;
                    weight_diff += abs(agents[agent1_index].connections[agents[agent1_index].connection_id_to_index[i]].weight - agents[agent2_index].connections[agents[agent2_index].connection_id_to_index[i]].weight);
                }
            }
            (homologous_count != 0) ? weight_diff /= homologous_count : weight_diff = 0;

            return (tuning_excessgene * excess + tuning_disjointgene * disjoint)/normalizer + tuning_avgweightdiff * weight_diff;

        }

        void assignSpecies(){
            species.clear(); representatives.clear();

            int next_species = 0;
            for(int i = 0; i < agents.size(); ++i){
                for(int j = 0; j < representatives.size(); ++j){
                    if(genomeRawDistance(i, representatives[j].second) < genome_distance_threshold){
                        species.push_back(representatives[j].first);
                        break;
                    }
                }
                if(species.size() <= i){ //Condition where a similar gene is not found in representatives and hence the species ID for index i was not pushed back
                    species.push_back(next_species);
                    representatives.push_back(pair(next_species, i));
                    next_species++;
                    
                }
            }
        }

        void randMutate(int index){
            uniform_real_distribution<double> prob(0, 1);

            double rand = prob(rng);

            if(rand <= prob_perturbation) perturbWeights(index);
            else if(rand <= prob_perturbation + prob_connection) randConnection(index);
            else insertRandNode(index);

        }

};

#endif