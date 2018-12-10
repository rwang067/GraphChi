
#define DYNAMICEDATA 1

#include <string>

#include "graphchi_basic_includes.hpp"
#include "api/dynamicdata/chivector.hpp"
#include "util/toplist.hpp"

using namespace graphchi;

/**
 * Type definitions. Remember to create suitable graph shards using the
 * Sharder-program.
 */
typedef unsigned WalkDataType;
typedef float VertexDataType;
typedef chivector<vid_t>  EdgeDataType;

 
struct PersonalizedPageRank : public GraphChiProgram<VertexDataType, EdgeDataType> {

public:
    vid_t s;
    unsigned R, L;
    chivector<WalkDataType> restart_walks;
    
public:
    
    bool is_source(vid_t v) {
        return (v == s );
    }

    void initialization(unsigned _s, unsigned _R, unsigned _L){
        s = _s;
        R = _R;
        L = _L;
        restart_walks.resize(0);
    }
    
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
        if (gcontext.iteration == 0) {
            
            if (is_source(vertex.id())) {
                /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                WalkDataType walk = 1;
                for(unsigned i=0; i < R; i++) {
                    /* Get random out edge's vector */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    if (outedge != NULL) {
                        chivector<WalkDataType> * evector = outedge->get_vector();
                        evector->add(walk);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }else{
                        logstream(LOG_ERROR) << "Wrong source = "<< vertex.id() << " without out-edges!" << std::endl;
                        assert(false);
                    }
                }
                vertex.set_data(R);
            }else{
                vertex.set_data(0);
            }
        } else {
            /* Check inbound edges for walks and advance them. */
            int num_walks = 0;
            //update the walks from in_edges
            for(int i=0; i < vertex.num_inedges(); i++) {
                graphchi_edge<EdgeDataType> * edge = vertex.inedge(i);
                chivector<WalkDataType> * invector = edge->get_vector();
                int j;
                for(j=0; j < invector->size(); j++) {
                    /* Get one walk */
                    WalkDataType walk = invector->get(j);
                    //assert(walk >= (WalkDataType)gcontext.iteration);
                    if(walk > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    float cc = ((float)rand())/RAND_MAX;
                    if (outedge != NULL && cc > 0.15 ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }else{
                        restart_walks.add(walk+1);
                        gcontext.scheduler->add_task(s); // Schedule destination
                    }
                    num_walks ++;
                }
                /* Remove all walks from the inbound vector */
                // invector->clear();
                invector->truncate(j);
            }

            if (is_source(vertex.id())) {
                int i;
                int total_walks = restart_walks.size();
                for(i=0; i < total_walks; i++) {
                    WalkDataType walk = restart_walks.get(i);
                    assert(walk >= (WalkDataType)gcontext.iteration);
                    if(walk > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    float cc = ((float)rand())/RAND_MAX;
                    if (outedge != NULL && cc > 0.15 ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }
                    else{ //random jump
                        restart_walks.add(walk+1);
                        gcontext.scheduler->add_task(s); // Schedule destination
                    }
                    num_walks ++;
                }
                if( i > 0 ) restart_walks.truncate(i);
            }
            /* Keep track of the walks passed by via this vertex */
            vertex.set_data(vertex.get_data() + num_walks);
        }
    }
    
    /**
     * Called before an iteration starts.
     */
    void before_iteration(int iteration, graphchi_context &gcontext) {
    }
    
    /**
     * Called after an iteration has finished.
     */
    void after_iteration(int iteration, graphchi_context &gcontext) {
    }
    
    /**
     * Called before an execution interval is started.
     */
    void before_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }
    
    /**
     * Called after an execution interval has finished.
     */
    void after_exec_interval(vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
    }
    
};

 

int main(int argc, const char ** argv) {
    /* GraphChi initialization will read the command line
     arguments and the configuration file. */
    graphchi_init(argc, argv);
    
    /* Metrics object for keeping track of performance counters
     and other information. Currently required. */
    metrics m("PersonalizedPageRank");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int source           = get_option_int("source", 0); // 
    // int N           = get_option_int("N", 4847571); // Number of iterations
    int R           = get_option_int("R", 1000); // 
    int L           = get_option_int("L", 10); // Number of iterations
    bool scheduler       = true;                       // Whether to use selective scheduling
    
    /* Detect the number of shards or preprocess an input to create them */
    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);
    
    /* Run */
    PersonalizedPageRank program;
    program.initialization(source,R,L);
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m);
    if (preexisting_shards) {
        engine.reinitialize_edge_data(0);
    }
    engine.run(program, L);
    
    /* List top 20 */
    int ntop = 50;
    std::vector< vertex_value<VertexDataType> > top = get_top_vertices<VertexDataType>(filename, ntop);
    std::cout << "Print top 20 vertices: " << std::endl;
    for(int i=0; i < (int) top.size(); i++) {
        std::cout << (i+1) << ". " << top[i].vertex << "\t" << top[i].value << std::endl;
    }

    // program.writeFile();
    // program.computeError(100);

    /* Report execution metrics */
    metrics_report(m);
    return 0;
}
