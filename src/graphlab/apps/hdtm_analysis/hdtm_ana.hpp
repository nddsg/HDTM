#ifndef __CON_HDTM_HPP__
#define __CON_HDTM_HPP__

#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <vector>

/*! Option's default value is defined in opts.cpp */
double GAMMA;
double ALPHA;
double ETA;
uint64_t num_tokens;
uint32_t MAX_ITER;
uint32_t burn_in;
uint32_t sample;

double LOG_GAMMA = 0;
double LOG_1_GAMMA = 0;
double ETA_SUM = 0;
uint64_t root_node = 0;
std::string binary_file_prefix;
bool load_from_binary = false;

/*! Use this to store children vertices, although graphlab::vertex_id_type
 *  is uint64_t, the total number of vertices in wikipedia fits in uint32_t. */
typedef boost::unordered_set<uint32_t> uint32_set;
typedef std::vector<uint32_t> uint32_vector;
typedef boost::unordered_map<uint32_t, uint32_t> uint32_map;
typedef std::pair<uint32_t, uint32_t> uint32_pair;
typedef std::vector<uint32_pair> uint32_pair_vector;
typedef boost::unordered_map<uint32_t, int32_t> token_update_map;
typedef boost::unordered_map<uint16_t, uint32_map> feature_sequence_map;
typedef boost::unordered_map<uint32_t, token_update_map> vertex_token_map;
typedef boost::unordered_map<uint16_t, double> uint16_double_map;
typedef boost::unordered_map<uint32_t, double> uint32_double_map;
typedef boost::unordered_map<uint32_t, uint16_t> vertex_level_map;
typedef std::pair<uint16_t, double> uint16_double_pair;
typedef boost::unordered_map<uint32_t, uint16_double_pair> vertex_level_rwr_map;
typedef uint16_double_map score_map;
typedef uint32_double_map likelihood_map;

feature_sequence_map global_feature_sequence_map;

inline uint32_t u64_to_u32(const uint64_t& u64_val)
{
	return static_cast<uint32_t>(u64_val);
}

inline uint64_t u32_to_u64(const uint32_t& u32_val)
{
	return static_cast<uint64_t>(u32_val);
}

inline uint32_map operator+=(uint32_map& lvalue, const uint32_map& rvalue) {
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			lvalue = rvalue;
		} else {
			//lvalue.reserve(lvalue.size() + rvalue.size());
			BOOST_FOREACH(const uint32_map::value_type& i, rvalue) {
				lvalue[i.first] += i.second;
			}
		}
	} 

	return lvalue;
}

inline token_update_map operator+=(token_update_map& lvalue, const token_update_map& rvalue) {
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			lvalue = rvalue;
		} else {
			//lvalue.reserve(lvalue.size() + rvalue.size());
			BOOST_FOREACH(const token_update_map::value_type& i, rvalue) {
				lvalue[i.first] += i.second;
			}
		}
	} 

	return lvalue;

}

inline uint32_set& operator+=(uint32_set& lvalue, const uint32_set& rvalue)
{
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			 lvalue = rvalue;
		} else {
			lvalue.reserve(lvalue.size() + rvalue.size());
			BOOST_FOREACH(const uint32_t &element, rvalue){
				lvalue.insert(element);
			}
		}
	}
	return lvalue;
}

inline uint32_set& operator+=(uint32_set& lvalue, const uint32_t& rvalue)
{
	lvalue.insert(rvalue);
	return lvalue;
}

inline uint32_set& operator+=(uint32_set& lvalue, const uint64_t& rvalue)
{
	lvalue.insert(u64_to_u32(rvalue));
	return lvalue;
}
inline uint32_vector& operator+=(uint32_vector& lvalue,
																 const uint32_vector& rvalue)
{
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			lvalue = rvalue;
		} else {
			lvalue.reserve(lvalue.size() + rvalue.size());
			BOOST_FOREACH(const uint32_t & element, rvalue) {
				lvalue.push_back(element);
			}
		}
	}
	return lvalue;
}

inline uint32_vector& operator+=(uint32_vector& lvalue,
																 const uint32_t& rvalue)
{
	lvalue.push_back(rvalue);
	return lvalue;
}

inline uint32_vector& operator+=(uint32_vector& lvalue,
																 const uint64_t& rvalue)
{
	lvalue.push_back(u64_to_u32(rvalue));
	return lvalue;
}

inline vertex_level_map& operator+=(vertex_level_map& lvalue,
																		const vertex_level_map& rvalue)
{
	BOOST_FOREACH(const vertex_level_map::value_type& i, rvalue) {
		lvalue[i.first] += i.second;
	}
	return lvalue;
}

inline vertex_level_rwr_map& operator+=(vertex_level_rwr_map& lvalue,
																				const vertex_level_rwr_map& rvalue)
{
	BOOST_FOREACH(const vertex_level_rwr_map::value_type& i, rvalue) {
		lvalue[i.first] = i.second;
	}
	return lvalue;
}

inline feature_sequence_map& operator+=(feature_sequence_map& lvalue,
																				const feature_sequence_map& rvalue)
{
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			lvalue = rvalue;
		} else {
			BOOST_FOREACH(const feature_sequence_map::value_type& i, rvalue) {
				lvalue[i.first] += i.second;
			}
		}
	}
	return lvalue;
}

inline vertex_token_map& operator+=(vertex_token_map& lvalue,
																		const vertex_token_map& rvalue)
{
	if(!rvalue.empty()) {
		if(lvalue.empty()) {
			lvalue = rvalue;
		} else {
			BOOST_FOREACH(const vertex_token_map::value_type& i, rvalue) {
				lvalue[i.first] += i.second;
			}
		}
	}
	return lvalue;
}

#include <graphlab.hpp>

/**
 * \brief Vertex data type
 */
struct vertex_data {
	/*! Random walk w/ restart score */
	double rwr_score;
	/*! New parent for next iteration */
	uint32_t new_parent;

	double node_weight;

	bool finished;

	uint32_set children;

	uint32_t nchanged;

	/*! Path from root to this vertex */
	uint32_vector root_path;

	uint32_map change_map;


	vertex_data(): rwr_score(0.0), finished(false), nchanged(0) {}

	void save(graphlab::oarchive& oarc) const
	{
		 oarc << rwr_score << new_parent << node_weight 
					<< finished << children << root_path << nchanged << change_map;
		 // << token_map 
		 //			<< update_map << fs_map << children_fs_map << candidates_token_map;
	}

	void load(graphlab::iarchive& iarc)
	{
		iarc >> rwr_score >> new_parent >> node_weight 
				 >> finished >> children >> root_path >> nchanged >> change_map;
		// >> token_map 
		//		 >> update_map >> fs_map >> children_fs_map >> candidates_token_map;
	}
};

struct edge_data {
	bool is_tree_edge;

	edge_data() {
		is_tree_edge = false;
	}

	void save(graphlab::oarchive& oarc) const
	{
		oarc << is_tree_edge;
	}
	void load(graphlab::iarchive& iarc)
	{
		iarc >> is_tree_edge;
	}
};

struct cate_vertex_data {
	bool is_cat;
	bool src;
	bool dest;
	cate_vertex_data() {
		is_cat = true;
		src = false;
		dest = false;
	}
	cate_vertex_data(bool is_cat_type) {
		is_cat = is_cat_type;
		src = false;
		dest = false;
	}

	void save(graphlab::oarchive& oarc) const {oarc << is_cat << src << dest;}
	void load(graphlab::iarchive& iarc) {iarc >> is_cat >> src >> dest;}
	
};

inline bool is_tree_edge(const edge_data& edge) 
{
	return edge.is_tree_edge;
}

typedef graphlab::distributed_graph<vertex_data, edge_data> page_graph_type;
typedef graphlab::distributed_graph<cate_vertex_data, graphlab::empty> cate_graph_type;

#endif
