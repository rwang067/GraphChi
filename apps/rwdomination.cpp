
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
typedef unsigned VertexDataType;
typedef chivector<WalkDataType>  EdgeDataType;

struct RWDomination : public GraphChiProgram<VertexDataType, EdgeDataType> {

public:
    unsigned N, R, L;
    chivector<WalkDataType> *random_jump_walks;
    
public:

    bool is_source(vid_t v) {
        return true;
    }

    void initialization(unsigned _N, unsigned _R, unsigned _L){
        N = _N;
        R = _R;
        L = _L;
        logstream(LOG_DEBUG) << "random_jump_walks size : " << sizeof(chivector<WalkDataType>) << " " << N << std::endl;
        random_jump_walks = new chivector<WalkDataType>[N];
        for(unsigned i=0; i<N; i++)
            random_jump_walks[i].resize(0);
    }
    
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
        if (gcontext.iteration == 0) {
            
            if (is_source(vertex.id())) {
                /* Add a random walk particle, represented by the hop of the walk */
                WalkDataType walk = 1;
                for(unsigned i=0; i < R; i++) {
                    /* Get random out edge's vector */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    if (outedge != NULL) {
                        chivector<WalkDataType> * evector = outedge->get_vector();
                        evector->add(walk);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }else{ //random select a vertex to jump
                        vid_t rand_vert = rand() % N;
                        random_jump_walks[rand_vert].add(walk);
                        gcontext.scheduler->add_task(rand_vert); // Schedule destination
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
                    assert(walk >= (WalkDataType)gcontext.iteration);
                    if(walk > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    float cc = ((float)rand())/RAND_MAX;
                    if (outedge != NULL && cc > 0.15 ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }else{ //random jump
                        vid_t rand_vert = rand() % N;
                        random_jump_walks[rand_vert].add(walk+1);
                        gcontext.scheduler->add_task(rand_vert); // Schedule destination
                    }
                    num_walks ++;
                }
                /* Remove all walks from the inbound vector */
                // invector->clear();
                invector->truncate(j);
            }

            //update the walks in random_jump_walks
            int i;
            int total_walks = random_jump_walks[vertex.id()].size();
            for(i=0; i < total_walks; i++) {
                WalkDataType walk = random_jump_walks[vertex.id()].get(i);
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
                    vid_t rand_vert = rand() % N;
                    random_jump_walks[rand_vert].add(walk+1);
                    gcontext.scheduler->add_task(rand_vert); // Schedule destination
                }
                num_walks ++;
            }
            if( i > 0 ) random_jump_walks[vertex.id()].truncate(i);
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
    metrics m("RWDomination");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int N           = get_option_int("N", 4847571); // Number of iterations
    int R           = get_option_int("R", 1); // 
    int L           = get_option_int("L", 6); // Number of iterations
    bool scheduler       = true;                       // Whether to use selective scheduling
    
    /* Detect the number of shards or preprocess an input to create them */

    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);
    
    /* Run */
    RWDomination program;
    program.initialization(N,R,L);
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
