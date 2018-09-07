
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

class SimRank : public GraphChiProgram<VertexDataType, EdgeDataType>{
private:
	vid_t a, b;
	unsigned R, L;
	std::vector<vid_t> walksfroma;
	std::vector<vid_t> walksfromb;
    chivector<WalkDataType> restart_walks_a;
    chivector<WalkDataType> restart_walks_b;

public:

    bool is_source(vid_t v) {
        return ( v == a ||  v == b );
    }

	void initializeApp( vid_t _a, vid_t _b, unsigned _R, unsigned _L ){
		a = _a;
		b = _b;
		R = _R;
		L = _L;
		walksfroma.resize(R*L);
		walksfromb.resize(R*L);
		memset(walksfroma.data(), 0xffffffff, walksfroma.size()*sizeof(vid_t));
		memset(walksfromb.data(), 0xffffffff, walksfromb.size()*sizeof(vid_t));
        restart_walks_a.resize(0);
        restart_walks_b.resize(0);
	}

	void updateInfo(unsigned walk, unsigned r, vid_t dstId){
		vid_t s = (vid_t)( walk >> 16 ) & 0xffff;
		// std::cout << "update : " << s << " " << (unsigned)(walk & 0xfff) << " " << l << " " << dstId << " " << r << std::endl;
		if( s == a )
			walksfroma[r] = dstId;
		else if( s == b )
			walksfromb[r] = dstId;
		else{
			logstream(LOG_ERROR) << "Wrong source s : " << s << std::endl;
			assert(0);
		}
		// std::cout << "s , r, dstId, walk = " << s << " " << r << " " << dstId << " " << walk << std::endl;
	}

	/**
     *  Vertex update function.
     */
    void update(graphchi_vertex<VertexDataType, EdgeDataType > &vertex, graphchi_context &gcontext) {
        if (gcontext.iteration == 0) {
            
            if (is_source(vertex.id())) {
                for(unsigned i = 0; i < R; i++) {
                    /* Get random out edge's vector */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    if (outedge != NULL) {
                    	// std::cout << vertex.id() << " " << i << " " << outedge->vertex_id() << std::endl;
                        chivector<vid_t> * evector = outedge->get_vector();
                        /* Add a random walk particle, represented by the vertex-id of the source (this vertex) */
                        WalkDataType walk = (( vertex.id() & 0xffff ) << 16 ) | ( (i*L+1) & 0xffff ) ;
                        evector->add(walk);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                		updateInfo(walk, i*L, outedge->vertex_id() );
                    }else{
                        logstream(LOG_ERROR) << "Wrong source = "<< vertex.id() << " without out-edges!" << std::endl;
                        assert(false);
                    }
                }
            }
        } else {
            /* Check inbound edges for walks and advance them. */
            for(int i=0; i < vertex.num_inedges(); i++) {
                graphchi_edge<EdgeDataType> * edge = vertex.inedge(i);
                chivector<WalkDataType> * invector = edge->get_vector();
                int j;
                for(j=0; j < invector->size(); j++) {
                    /* Get one walk */
                    WalkDataType walk = invector->get(j);
                    unsigned hop = (unsigned)(walk & 0xffff);
                    assert(hop%L >= (WalkDataType)gcontext.iteration);
                    if(hop%L > (WalkDataType)gcontext.iteration ) break;
                    /* Move to a random out-edge */
                    graphchi_edge<EdgeDataType> * outedge = vertex.random_outedge();
                    float cc = ((float)rand())/RAND_MAX;
                    if (outedge != NULL && cc > 0.15 ) {
                        chivector<WalkDataType> * outvector = outedge->get_vector();
                        outvector->add(walk+1);
                        gcontext.scheduler->add_task(outedge->vertex_id()); // Schedule destination
                        updateInfo(walk, hop+1, outedge->vertex_id() );
                    }else{
                        if(vertex.id()==a){
                            restart_walks_a.add(walk+1);
                        }else{
                            restart_walks_b.add(walk+1);
                        }
                        gcontext.scheduler->add_task(vertex.id()); // Schedule destination
                    }
                }
                /* Remove all walks from the inbound vector */
                // invector->clear();
                invector->truncate(j);
            }
            if (vertex.id()==a) {
                int i;
                int total_walks = restart_walks_a.size();
                for(i=0; i < total_walks; i++) {
                    WalkDataType walk = restart_walks_a.get(i);
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
                        restart_walks_a.add(walk+1);
                        gcontext.scheduler->add_task(vertex.id()); // Schedule destination
                    }
                }
                if( i > 0 ) restart_walks_a.truncate(i);
            }else if (vertex.id()==b) {
                int i;
                int total_walks = restart_walks_b.size();
                for(i=0; i < total_walks; i++) {
                    WalkDataType walk = restart_walks_b.get(i);
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
                        restart_walks_b.add(walk+1);
                        gcontext.scheduler->add_task(vertex.id()); // Schedule destination
                    }
                }
                if( i > 0 ) restart_walks_b.truncate(i);
            }
        }
    }

	float computeResult(){
		float simrank = 0;
		for( unsigned i = 0; i < R; i++ )
			for( unsigned j = 0; j < R; j++ )
				for( unsigned l = 0; l < L; l++ ){
					// std::cout << i << " " << j << " " << l << " : " << walksfroma[i*L+l] << " " << walksfromb[j*R+l] << std::endl;
					if( walksfroma[i*L+l] == walksfromb[j*L+l] && walksfroma[i*L+l] != 0xffffffff ){
						simrank += (1.0/(R*R))*pow(0.8, l);
						// std::cout << i << " " << j << " " << l << " " << walksfroma[i*L+l] << " " << simrank << std::endl;
						break;
					}
				}
		return simrank;
	}

};

int main(int argc, const char ** argv) {
    /* Read the command line arguments and the configuration file. */
    graphchi_init(argc, argv);
    /* Metrics object for keeping track of performance count_invectorers and other information. Currently required. */
    metrics m("simrank");
    
    /* Basic arguments for application */
    std::string filename = get_option_string("file", "../DataSet/LiveJournal/soc-LiveJournal1.txt");  // Base filename
    int a = get_option_int("a", 0); // 
    int b = get_option_int("b", 1); // 
    int R = get_option_int("R", 1000); // Number of walks
    int L = get_option_int("L", 11); // Number of steps per walk
    
    /* Detect the number of shards or preprocess an input to create them */
    bool scheduler       = true;    
    bool preexisting_shards;
    int nshards          = convert_if_notexists<vid_t>(filename, get_option_string("nshards", "auto"), preexisting_shards);

    /* Run */
    SimRank program;
    program.initializeApp( a, b, R, L );
    graphchi_engine<VertexDataType, EdgeDataType> engine(filename, nshards, scheduler, m);
    if (preexisting_shards) {
        engine.reinitialize_edge_data(0);
    }
    engine.run(program, L);

    float simrank = program.computeResult();
    std::cout << "SimRank for " << a << " and " << b << " = " << simrank << std::endl;
    
    /* Report execution metrics */
    metrics_report(m);
    return 0;
}