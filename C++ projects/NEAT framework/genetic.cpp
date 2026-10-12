#include "neat.h"

//Before anything, srand(time(0)) must be called to randomize the seed for generation

using namespace std;

void weightmutate(connectiongene& gene, double stddev, double resetprobability){ //Recommendation for stddev: 0.5

    if(static_cast<double>(rand())/RAND_MAX < resetprobability){ //Perturb existing weight according to standard deviation.
        default_random_engine generator;
        normal_distribution<double> distribution(0,stddev);
        double change = distribution(generator);
        gene.weight += change;
    } else { //An acceptable range of initialization values for LReLU is [-1,1]. As training progresses, may be reduced to [-0.5, 0,5] to improve exploitation and suppress exploration
        gene.weight = 2*static_cast<double>(rand())/RAND_MAX - 1;
    }
}

void weightmutate(agent& agnt, double stddev, double resetprobability, double mutationprobability){ //Recommendation for mutationprobability: 0.8 - 0.9

    for(connectiongene& connection : agnt.connectiongenome){
        if(static_cast<double>(rand())/RAND_MAX < mutationprobability){
            weightmutate(connection, stddev, resetprobability);
        }
    }
}

int computedepth(node& nd, agent& agnt){ //Depth of a node is equal to the maximum depth of incoming neurons + 1. Input neurons are depth 0.
    if(nd.type == 'i') return 0; //Return 0 if node is an input node
    
    int maxdepth = 0;
    for(int i = 0; i < agnt.connectiongenome.size(); ++i){
        if(agnt.connectiongenome[i].node_out == nd.id){
            int nodein_index;
            bool found = false;
            for(int j = 0; j < agnt.nodes.size(); ++j){
                if(agnt.nodes[j].id == agnt.connectiongenome[i].node_in){
                    nodein_index = j;
                    found = true;
                    break;
                }
            }

            if(!found) continue;

            int nodedepth = computedepth(agnt.nodes[nodein_index], agnt);
            if(nodedepth > maxdepth){
                maxdepth = nodedepth;
            }
        }
    }
    return maxdepth + 1;
}

void insertnode(connectiongene& cnctn, agent& agnt, int& inno_number, int& nodecounter){
    cnctn.enabled = false;
    node newnode;

    /*
    int maxnode = -1;
    for(int i = 0; i < agnt.nodes.size(); ++i){
        if(agnt.nodes[i].id > maxnode){
            maxnode = agnt.nodes[i].id;
        }
    }
    */

    newnode.id = nodecounter + 1;
    nodecounter++;
    newnode.type = 'h';
    agnt.nodes.push_back(newnode);

    connectiongene newcnctn1;
    newcnctn1.id = inno_number + 1;
    newcnctn1.node_in = cnctn.node_in;
    newcnctn1.node_out = agnt.nodes[agnt.nodes.size()-1].id;
    newcnctn1.weight = 1.0;
    agnt.connectiongenome.push_back(newcnctn1);

    connectiongene newcnctn2;
    newcnctn2.id = inno_number + 2;
    newcnctn2.node_in = agnt.nodes[agnt.nodes.size()-1].id;
    newcnctn2.node_out = cnctn.node_out;
    newcnctn2.weight = cnctn.weight;
    agnt.connectiongenome.push_back(newcnctn2);

    inno_number += 2;

}

void insertnode(agent& agnt, int& inno_number, int& nodecounter){

    int targetcnctn = rand() % agnt.connectiongenome.size();

    while(!agnt.connectiongenome[targetcnctn].enabled) targetcnctn = rand() % agnt.connectiongenome.size(); //Look for a different gene if current gene is dormant

    insertnode(agnt.connectiongenome[targetcnctn], agnt, inno_number, nodecounter);

}

bool newconnection(node& node_in, node& node_out, agent& agnt, int& inno_number){
    if(computedepth(node_in, agnt) > computedepth(node_out, agnt)) return false; //Return false when output node is shallower than input node (ensuring feedforwardness)

    if (node_out.type == 'i') return false;
    if (node_in.type == 'o') return false;


    for(int i = 0; i < agnt.connectiongenome.size(); ++i){
        if(agnt.connectiongenome[i].node_in == node_in.id && agnt.connectiongenome[i].node_out == node_out.id) return false; //Return false when connection already exists
    }

    connectiongene newcnctn;
    newcnctn.id = inno_number + 1;
    newcnctn.node_in = node_in.id;
    newcnctn.node_out = node_out.id;
    newcnctn.weight = 0; //Initialized to 0 to not disrupt preexisting functionality
    agnt.connectiongenome.push_back(newcnctn);

    inno_number++;

    return true;
}

void newconnection(agent& agnt, int& inno_number){

    int node1 = rand() % agnt.nodes.size();
    int node2 = rand() % agnt.nodes.size();
    while (node1 == node2) node2 = rand() % agnt.nodes.size();

    /*
    int index1 = 0;
    for(int i = 0; i < agnt.nodes.size(); ++i){
        if(agnt.nodes[i].id == node1){
            index1 = i;
            break;
        }
    }

    int index2 = 0;
    for(int i = 0; i < agnt.nodes.size(); ++i){
        if(agnt.nodes[i].id == node2){
            index2 = i;
            break;
        }
    }
    */

    int depth1 = computedepth(agnt.nodes[node1], agnt);
    int depth2 = computedepth(agnt.nodes[node2], agnt);

    if(depth1 < depth2){
        newconnection(agnt.nodes[node1], agnt.nodes[node2], agnt, inno_number);
    } else {
        newconnection(agnt.nodes[node2], agnt.nodes[node1], agnt, inno_number);
    }
}

agent crossover(agent& fitparent, agent& unfitparent, int& inno_number){

    vector<bool> fitgenes(inno_number + 1, false);
    vector<bool> unfitgenes(inno_number + 1, false);

    vector<int> fitgeneindexes(inno_number + 1, -1);
    vector<int> unfitgeneindexes(inno_number + 1, -1);

    for(int i = 0; i < fitparent.connectiongenome.size(); ++i){
        fitgenes[fitparent.connectiongenome[i].id] = true;
        fitgeneindexes[fitparent.connectiongenome[i].id] = i;
    }

    for(int i = 0; i < unfitparent.connectiongenome.size(); ++i){
        unfitgenes[unfitparent.connectiongenome[i].id] = true;
        unfitgeneindexes[unfitparent.connectiongenome[i].id] = i;
    }

    vector<connectiongene> genome = {};
    vector<node> nodelist = {};

    for(int i = 0; i <= inno_number; ++i){
        if(fitgenes[i] && unfitgenes[i]){ //Matching gene
            connectiongene gene;
            if(fitparent.connectiongenome[fitgeneindexes[i]].enabled && unfitparent.connectiongenome[unfitgeneindexes[i]].enabled){ //When both parents have gene enabled
                gene.enabled = true;
            } else if(fitparent.connectiongenome[fitgeneindexes[i]].enabled || unfitparent.connectiongenome[unfitgeneindexes[i]].enabled){
                if(static_cast<double>(rand())/RAND_MAX > 0.75){ //75% probability that gene is disabled if disabled on either parent
                    gene.enabled = true;
                } else {
                    gene.enabled = false;
                }
            } else {
                gene.enabled = false;
            }

            gene.id = i;

            gene.node_in = fitparent.connectiongenome[fitgeneindexes[i]].node_in;
            gene.node_out = fitparent.connectiongenome[fitgeneindexes[i]].node_out;

            gene.weight = (fitparent.connectiongenome[fitgeneindexes[i]].weight + unfitparent.connectiongenome[unfitgeneindexes[i]].weight) / 2; //Average of weights

            genome.push_back(gene);

        } else if(fitgenes[i]){ //Disjoint or excess gene - inherit from fit parent
            connectiongene gene;
            gene.enabled = fitparent.connectiongenome[fitgeneindexes[i]].enabled;
            gene.id = fitparent.connectiongenome[fitgeneindexes[i]].id;
            gene.node_in = fitparent.connectiongenome[fitgeneindexes[i]].node_in;
            gene.node_out = fitparent.connectiongenome[fitgeneindexes[i]].node_out;
            gene.weight = fitparent.connectiongenome[fitgeneindexes[i]].weight;

            genome.push_back(gene);
        } //It is not mandatory to do anything if only unfit parent carries a gene; it will not be inherited.
    }

    for(int i = 0; i < fitparent.nodes.size(); ++i){ //Inherit input-output nodes
        if(fitparent.nodes[i].type == 'i' || fitparent.nodes[i].type == 'o'){
            node newnode;
            newnode.id = fitparent.nodes[i].id;
            newnode.type = fitparent.nodes[i].type;

            nodelist.push_back(newnode);
        }
    }

    for(int i = 0; i < genome.size(); ++i){
        bool flag = false;
        for(int j = 0; j < nodelist.size(); ++j){
            if(nodelist[j].id == genome[i].node_in){
                flag = true;
                break;
            }
        }
        if(!flag){
            node newnode;
            newnode.id = genome[i].node_in;
            newnode.type = 'h';
            nodelist.push_back(newnode);
        }
    }

    agent returner;

    returner.connectiongenome = genome;
    returner.nodes = nodelist;

    return returner;
}

void prettyprint(agent& agnt, bool depth){
    cout << "Nodes: \n";
    for(int i = 0; i < agnt.nodes.size(); ++i){
        cout << "ID: " << agnt.nodes[i].id << "   ";
        if(depth) cout << "Depth: " << computedepth(agnt.nodes[i], agnt) << "   ";
        cout << "Type: " << agnt.nodes[i].type << "\n";
    }
    cout << "\n";
    cout << "Connections: \n";
    for(int i = 0; i < agnt.connectiongenome.size(); ++i){
        cout << "ID: " << agnt.connectiongenome[i].id << "   ";
        cout << "Enabled: " << agnt.connectiongenome[i].enabled << "   ";
        cout << "Input node: " << agnt.connectiongenome[i].node_in << "   ";
        cout << "Output node: " << agnt.connectiongenome[i].node_out << "   ";
        cout << "Weight: " << agnt.connectiongenome[i].weight << "\n";
    }
    cout << "\n";
}


int main(){

    srand(time(0));

    int innovation_number = 4;
    int node_id_counter = 4;

    agent agnt1;
    agent agnt2;

    node nd1;
    nd1.id = 0;
    nd1.type = 'i';

    node nd2;
    nd2.id = 1;
    nd2.type = 'h';

    node nd3;
    nd3.id = 2;
    nd3.type = 'h';

    node nd4;
    nd4.id = 3;
    nd4.type = 'o';

    vector<node> nds = {nd1, nd2, nd3, nd4};

    connectiongene c1;
    c1.id = 0;
    c1.node_in = 0;
    c1.node_out = 1;

    connectiongene c2;
    c2.id = 1;
    c2.node_in = 0;
    c2.node_out = 2;

    connectiongene c3;
    c3.id = 2;
    c3.node_in = 1;
    c3.node_out = 2;

    connectiongene c4;
    c4.id = 3;
    c4.node_in = 2;
    c4.node_out = 3;

    vector<connectiongene> cs = {c1, c2, c3, c4};

    agnt1.nodes = nds;
    agnt1.connectiongenome = cs;

    agnt2.nodes = nds;
    agnt2.connectiongenome = cs;

    for(int i = 0; i < 4; ++i){
        insertnode(agnt1, innovation_number, node_id_counter);
        insertnode(agnt2, innovation_number, node_id_counter);

        if(i%2==0){
            newconnection(agnt1, innovation_number);
            newconnection(agnt1, innovation_number);
        }
    }

    prettyprint(agnt1, true);
    prettyprint(agnt2, true);

    agent agnt3 = crossover(agnt1, agnt2, innovation_number);

    agent agnt4 = crossover(agnt2, agnt1, innovation_number);

    agent agnt5 = crossover(agnt3, agnt4, innovation_number);

    prettyprint(agnt3, true);
    prettyprint(agnt4, true);
    prettyprint(agnt5, true);

    return 0;
}

