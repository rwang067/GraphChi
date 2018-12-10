
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

struct PageRank : public GraphChiProgram<VertexDataType, EdgeDataType> {

public:
    unsigned N, R, L;
    chivector<WalkDataType> *random_jump_walks;
    // unsigned used_edges[4], total_edges[4];
    // unsigned nshards;
    // std::vector< vid_t > internal_end; 
    // std::vector< std::priority_queue<vid_t> > random_jump_walks;

    
public:

    bool is_source(vid_t v) {
        return true;
    }

    // unsigned Ivl_belong(vid_t v){
    //     for(unsigned p = 0; p < nshards; p++)
    //         if( v <= internal_end[p] )
    //             return p;
    //     assert(false);
    //     return -1;
    // }

    void initialization(unsigned _N, unsigned _R, unsigned _L, unsigned _nshards, std::string basefilename){
        N = _N;
        R = _R;
        L = _L;
        // nshards = _nshards;
        // logstream(LOG_DEBUG) << "random_jump_walks size : " << sizeof(std::vector< std::priority_queue<vid_t> >) << " " << nshards << std::endl;
        // random_jump_walks.resize(nshards);
        // internal_end.resize(nshards);
        // std::ifstream ivlfile;
        // ivlfile.open(filename_intervals(basefilename, nshards));
        // for(unsigned p = 0; p < nshards; p++)
        //     ivlfile >> internal_end[p];
        // ivlfile.close();

        logstream(LOG_DEBUG) << "random_jump_walks size : " << sizeof(chivector<WalkDataType>) << " " << N << std::endl;
        random_jump_walks = new chivector<WalkDataType>[N];
        for(unsigned i=0; i<N; i++)
            random_jump_walks[i].resize(0);

        // for(unsigned i=0; i<4; i++){
        //     used_edges[i] = 0;
        //     total_edges[i] = 0;
        // }
    }
    
    /**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
        // total_edges[omp_get_thread_num()] += vertex.num_outedges() + vertex.num_inedges();
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
                        // random_jump_walks[Ivl_belong(rand_vert)].push( rand_vert );
                        random_jump_walks[rand_vert].add(walk);
                        gcontext.scheduler->add_task(rand_vert); // Schedule destination
                    }
                }
                vertex.set_data(R);
                // used_edges[omp_get_thread_num()] += R;
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
                    // assert(walk >= (WalkDataType)gcontext.iteration);
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
                        // random_jump_walks[Ivl_belong(rand_vert)].push( rand_vert );
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
                // used_edges[omp_get_thread_num()]++;
                // Move to a random out-edge
                graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                float cc = ((float)rand())/RAND_MAX;
                if (outedge != NULL && cc > 0.15 ) {
                    chivector<WalkDataType> * outvector = outedge->get_vector();
                    //Add a random walk particle, represented by the vertex-id of the source (this vertex)
                    outvector->add(walk+1);
                    gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                }else{ //random jump
                    vid_t rand_vert = rand() % N;
                    random_jump_walks[rand_vert].add(walk+1);
                    gcontext.scheduler->add_task(rand_vert); // Schedule destination
                }
                num_walks ++;
            }
            if( i > 0 ) random_jump_walks[vertex.id()].truncate(i);
            


            vertex.set_data(vertex.get_data() + num_walks);
            // used_edges[omp_get_thread_num()] += num_walks;
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

    void writeFile(std::string basefilename){
        // compute the sum of counting
        unsigned *vertex_value = (unsigned*)malloc(sizeof(unsigned)*N);
        int fv = open(filename_vertex_data<VertexDataType>(basefilename).c_str(), O_RDONLY | O_CREAT, S_IROTH | S_IWOTH | S_IWUSR | S_IRUSR);
        assert(fv >= 0);
        preada(fv, vertex_value, sizeof(unsigned)*N, sizeof(unsigned)*0);
        close(fv);
        unsigned sum = 0;
        for( unsigned i = 0; i < N; i++ )
            sum += vertex_value[i];
        logstream(LOG_INFO) << "sum : " << sum << std::endl;
        free(vertex_value);

        // conpute the counting probability
        unsigned maxwindow = 400000000;
        vid_t st = 0, len = 0;
        while( st < N ){
            len = N-st < maxwindow ? N-st : maxwindow;
            logstream(LOG_INFO) << " s , len : " << st << " " << len << std::endl;
            // len = min( maxwindow, N - st );
            vertex_value = (unsigned*)malloc(sizeof(unsigned)*len);
            fv = open(filename_vertex_data<VertexDataType>(basefilename).c_str(), O_RDONLY | O_CREAT, S_IROTH | S_IWOTH | S_IWUSR | S_IRUSR);
            assert(fv >= 0);
            preada(fv, vertex_value, sizeof(unsigned)*len, sizeof(unsigned)*st);
            close(fv);
            float *visit_prob = (float*)malloc(sizeof(float)*len);
            for( unsigned i = 0; i < len; i++ )
                visit_prob[i] = vertex_value[i] * 1.0 / sum;
            int fp = open(filename_vertex_data<VertexDataType>(basefilename).c_str(), O_WRONLY | O_CREAT, S_IROTH | S_IWOTH | S_IWUSR | S_IRUSR);
            assert(fp >= 0);
            pwritea(fp, visit_prob, sizeof(unsigned)*len, sizeof(unsigned)*st);
            close(fp);
            free(vertex_value);
            free(visit_prob);
            st += len;
        }
    }

    void computeError(std::string basefilename, int ntop){
        //read the vertex value
        float* visit_prob = (float*)malloc(sizeof(float)*N);
        int fv = open(filename_vertex_data<VertexDataType>(basefilename).c_str(), O_RDONLY | O_CREAT, S_IROTH | S_IWOTH | S_IWUSR | S_IRUSR);
        assert(fv >= 0);
        preada(fv, visit_prob, sizeof(float)*N, 0);
        close(fv);

        // read the accurate value and compute the error
        std::string pr_file = basefilename + "_CompError/accurate_pr_top100.value";
        std::ifstream fin(pr_file.c_str());
        int vid ;
        float err=0, appv; //accurate pagerank value
        for(int i = 0; i < ntop; i++ ){
            fin >> vid >> appv;
            // logstream(LOG_INFO) << "vid appv vertex_value err: " << vid << " " << appv << " " << visit_prob[vid] << " " << fabs(visit_prob[vid]-appv)/appv << std::endl;
            err += fabs(visit_prob[vid]-appv)/appv;
        }
        free(visit_prob);
        err = err / ntop;
        logstream(LOG_DEBUG) << "Error : " << err << std::endl;

        std::string error_file = basefilename + "_CompError/GraphChi_pr_top100.error";
        std::ofstream errfile;
        errfile.open(error_file.c_str(), std::ofstream::app);
        errfile << err << "\n" ;
        errfile.close();
    }

    // void compUtilization(std::string basefilename){
    //     for(int i = 1; i < 4; i++){
    //         used_edges[0] += used_edges[i];
    //         total_edges[0] += total_edges[i];
    //     }
    //     float utilization = (float)used_edges[0] / (float)total_edges[0];
    //     logstream(LOG_DEBUG) << "IO utilization = " << utilization << std::endl;
    //     std::ofstream utilizationfile;
    //     utilizationfile.open(basefilename + "_CompError/GraphChi_pr_utilization.csv", std::ofstream::app);
    //     utilizationfile << total_edges[0] << "\t" << used_edges[0] << "\t" << utilization << "\n" ;
    //     utilizationfile.close();

    //     for(unsigned i=0; i<4; i++){
    //         used_edges[i] = 0;
    //         total_edges[i] = 0;
    //     }
    // }
    
};

 

int main(int argc, const char ** argv) {
    /* GraphChi initialization will read the command line
     arguments and the configuration file. */
    graphchi_init(argc, argv);
    
    /* Metrics object for keeping track of performance counters
     and other information. Currently required. */
    metrics m("PageRank");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int N           = get_option_int("N", 4847571); // Number of iterations
    int R           = get_option_int("R", 1); // 
    int L           = get_option_int("L", 20); // Number of iterations
    bool scheduler       = true;                       // Whether to use selective scheduling
    
    /* Detect the number of shards or preprocess an input to create them */

    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);
    
    /* Run */
    PageRank program;
    program.initialization(N,R,L,nshards,filename);
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
    program.computeError(filename,100);

    /* Report execution metrics */
    metrics_report(m);
    return 0;
}
