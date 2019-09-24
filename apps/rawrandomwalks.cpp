
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

 
struct Graphlet: public GraphChiProgram<VertexDataType, EdgeDataType> {

public:
    unsigned N, R, L;
    chivector<WalkDataType> *random_jump_walks;
    unsigned *vercount;
    unsigned *total_edges, *used_edges, *strandedwalks;
    int curblock;
    vid_t block_st, block_en;

public:
    
    bool is_source(vid_t v) {
        return ( v== 0 );
    }

    void initialization(unsigned _N, unsigned _R, unsigned _L){
        N = _N;
        R = _R;
        L = _L;
        logstream(LOG_DEBUG) << "random_jump_walks size : " << sizeof(chivector<WalkDataType>) << " " << N << std::endl;
        random_jump_walks = new chivector<WalkDataType>[N];
        for(unsigned i=0; i<N; i++)
            random_jump_walks[i].resize(0);
               
        int nThreads = omp_get_max_threads();
        total_edges = new unsigned[nThreads];
        used_edges = new unsigned[nThreads];
        strandedwalks = new unsigned[nThreads];
        vercount = new unsigned[nThreads];
         for(int i=0; i<4; i++){
            total_edges[i] = 0;
            used_edges[i] = 0;
            strandedwalks[i] = 0;
            vercount[i] = 0;
        }
        unlink("GraphChi_rwd_utilization.csv"); 

    }

    void updateInfo(unsigned walk, unsigned h, vid_t dstId){
	}
    
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
        int threadid = omp_get_thread_num();
        vercount[threadid]++;
        total_edges[threadid] += vertex.num_outedges() + vertex.num_inedges();
        if(!vertex.scheduled) return;
        int num_walks = 0;
        if (gcontext.iteration == 0) {
            
            if (is_source(vertex.id())) {
                /* Add a random walk particle, represented by the hop of the walk */
                num_walks = R;
                for(unsigned i=0; i < R; i++) {
                    /* Get random vertex to start walk */
                    vid_t rand_vert = rand() % N;
                    WalkDataType walk = (( rand_vert & 0xffff ) << 16 ) | ( 1 & 0xffff ) ;
                    random_jump_walks[rand_vert].add(walk);
                    gcontext.scheduler->add_task(rand_vert); // Schedule destination
                    updateInfo(walk, 0, rand_vert);
                }
            }
        } else {
            /* Check inbound edges for walks and advance them. */
            //update the walks from in_edges
            for(int i=0; i < vertex.num_inedges(); i++) {
                graphchi_edge<EdgeDataType> * edge = vertex.inedge(i);
                chivector<WalkDataType> * invector = edge->get_vector();
                int j;
                num_walks += invector->size();
                for(j=0; j < invector->size(); j++) {
                    /* Get one walk */
                    WalkDataType walk = invector->get(j);
                    unsigned hop = (unsigned)(walk & 0xffff);
                    assert(hop >= (WalkDataType)gcontext.iteration);
                    if(hop > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    float cc = ((float)rand())/RAND_MAX;
                    if (outedge != NULL && cc > 0.15 ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        vid_t nextHop = outedge->vertex_id();
                        if(nextHop >= block_st && nextHop <= block_en){
                            strandedwalks[threadid] ++;
                        }

                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                        updateInfo(walk, hop, outedge->vertex_id() );
                    }else{
                        vid_t rand_vert = rand() % N;
                        WalkDataType walk = (( rand_vert & 0xffff ) << 16 ) | ( 1 & 0xffff ) ;
                        // random_jump_walks[rand_vert].add(walk+1);
                        vid_t nextHop = rand_vert;
                        if(nextHop >= block_st && nextHop <= block_en){
                            strandedwalks[threadid] ++;
                        }
                        gcontext.scheduler->add_task(rand_vert); // Schedule destination
                        updateInfo(walk, 0, rand_vert);
                }
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
                unsigned hop = (unsigned)(walk & 0xffff);
                assert(hop >= (WalkDataType)gcontext.iteration);
                if(hop > (WalkDataType)gcontext.iteration ) break;
                /* Move to a random out-edge */
                graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                float cc = ((float)rand())/RAND_MAX;
                if (outedge != NULL && cc > 0.15 ) {
                    chivector<WalkDataType> * outvector = outedge->get_vector();
                    /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                    outvector->add(walk+1);
                    vid_t nextHop = outedge->vertex_id();
                    if(nextHop >= block_st && nextHop <= block_en){
                        strandedwalks[threadid] ++;
                    }
                    gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                    updateInfo(walk, hop, outedge->vertex_id() );
                }else{
                    vid_t rand_vert = rand() % N;
                    WalkDataType walk = (( rand_vert & 0xffff ) << 16 ) | ( 1 & 0xffff ) ;
                    // random_jump_walks[rand_vert].add(walk+1);
                    vid_t nextHop = rand_vert;
                    if(nextHop >= block_st && nextHop <= block_en){
                        strandedwalks[threadid] ++;
                    }
                    gcontext.scheduler->add_task(rand_vert); // Schedule destination
                    updateInfo(walk, 0, rand_vert);
                }
            }
            num_walks += i;
            if( i > 0 ) random_jump_walks[vertex.id()].truncate(i);
        }
        used_edges[threadid] += num_walks;
    }

    void compUtilization(std::string basefilename){
        for(int i = 1; i < 4; i++){
            total_edges[0] += total_edges[i];
            used_edges[0] += used_edges[i];
            strandedwalks[0] += strandedwalks[i];
            vercount[0] += vercount[i];
        }
        float utilization = (float)used_edges[0] / (float)total_edges[0];
        // logstream(LOG_DEBUG) << "IO utilization = " << utilization << std::endl;
        std::ofstream utilizationfile;
        utilizationfile.open("GraphChi_rwd_utilization.csv", std::ofstream::app);
        utilizationfile << curblock << "\t" << block_en-block_st+1 << "\t" << vercount[0] << "\t" << total_edges[0] << "\t" << used_edges[0] << "\t" << utilization << "\t" << strandedwalks[0] << "\n" ;
        utilizationfile.close();

        for(unsigned i=0; i<4; i++){
            total_edges[i] = 0;
            used_edges[i] = 0;
            strandedwalks[i] = 0;
            vercount[i] = 0;
        }
    }
    
    /**
     * Called before an execution interval is started.
     */
    void before_exec_interval(int exec_interval, vid_t window_st, vid_t window_en, graphchi_context &gcontext) {
        curblock = exec_interval;
        block_st = window_st;
        block_en = window_en;
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
    metrics m("Raw Random Walks");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int N = get_option_int("N", 4847571); // Number of vertices
    int R = get_option_int("R", 100000); // Number of walks
    int L = get_option_int("L", 10); // Number of steps per walk
    
    /* Detect the number of shards or preprocess an input to create them */
    bool scheduler       = true;                       // Whether to use selective scheduling
    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);
    
    /* Run */
    Graphlet program;
    program.initialization(N,R,L);
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m);
    if (preexisting_shards) {
        engine.reinitialize_edge_data(0);
    }
    engine.run(program, L);

    /* Report execution metrics */
    metrics_report(m);
    return 0;
}
