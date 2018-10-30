
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
typedef unsigned int VertexDataType;
typedef chivector<vid_t>  EdgeDataType;

 
struct Reachability : public GraphChiProgram<VertexDataType, EdgeDataType> {

private:
    vid_t a, b;
    unsigned R, L;
    bool ans;
    chivector<WalkDataType> restart_walks;

public:

    bool is_source(vid_t v) {
        return ( v == a );
    }

	void initializeApp( vid_t _a, vid_t _b, unsigned _R, unsigned _L ){
		a = _a;
		b = _b;
		R = _R;
		L = _L;
        restart_walks.resize(0);
        ans = false;
	}

    void updateInfo(vid_t dstId){
        if(dstId == b)
            ans = true;
	}
    
    /**
     *  Vertex update function.
     */
    void update1(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
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
                        ;
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
                    if (outedge != NULL ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }else{ //random jump
                        restart_walks.add(walk+1);
                        gcontext.scheduler->add_task(a); // Schedule destination
                        updateInfo(outedge->vertex_id());
                    }
                    num_walks ++;
                }
                /* Remove all walks from the inbound vector */
                // invector->clear();
                if( j > 0 ) invector->truncate(j);
            }

            //update the walks in random_jump_walks
            if(vertex.id()==a){
                int i;
                int total_walks = restart_walks.size();
                for(i=0; i < total_walks; i++) {
                    WalkDataType walk = restart_walks.get(i);
                    assert(walk >= (WalkDataType)gcontext.iteration);
                    if(walk > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    if (outedge != NULL) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    }
                    num_walks ++;
                }
                if( i > 0 ) restart_walks.truncate(i);
                vertex.set_data(vertex.get_data() + num_walks);
            }
        }
    }//later


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
                    if (outedge != NULL ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                        updateInfo(outedge->vertex_id());
                    }else{
                        // restart_walks.add(walk+1);
                        // gcontext.scheduler->add_task(vertex.id()); // Schedule destination
                    }
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
                    if (outedge != NULL ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                        updateInfo(outedge->vertex_id());
                    }
                }
                if( i > 0 ) restart_walks.truncate(i);
            }
        }
    }//right
    
    /**
     * Called before an iteration starts.
     */
    void before_iteration(int iteration, graphchi_context &gcontext) {
        logstream(LOG_DEBUG) << "restart_walks.size() capacity : " << restart_walks.size() << " " << restart_walks.capacity() << std::endl;
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

    bool getAns(){
        return ans;
    }
    
};

 

int main(int argc, const char ** argv) {
    /* GraphChi initialization will read the command line
     arguments and the configuration file. */
    graphchi_init(argc, argv);
    
    /* Metrics object for keeping track of performance counters
     and other information. Currently required. */
    metrics m("Reachability");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int a = get_option_int("a", 0); // 
    int b = get_option_int("b", 1); // 
    int R = get_option_int("R", 1000); // Number of walks
    int L = get_option_int("L", 100); // Number of steps per walk

    /* Detect the number of shards or preprocess an input to create them */
    bool scheduler       = true;                       // Whether to use selective scheduling
    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);
    
    /* Run */
    Reachability program;
    program.initializeApp(a,b,R,L);
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m);
    if (preexisting_shards) {
        engine.reinitialize_edge_data(0);
    }
    engine.run(program, L);

    std::cout << "\n === print result ===\n";
    std::cout << "ans=" << program.getAns() << std::endl;
    
    /* List top 20 */
    /*
    int ntop = 20;
    std::vector< vertex_value<VertexDataType> > top = get_top_vertices<VertexDataType>(filename, ntop);
    std::cout << "Print top 20 vertices: " << std::endl;
    for(int i=0; i < (int) top.size(); i++) {
        std::cout << (i+1) << ". " << top[i].vertex << "\t" << top[i].value << std::endl;
    }
    */

    /* Report execution metrics */
    metrics_report(m);
    return 0;
}
