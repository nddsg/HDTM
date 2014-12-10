#ifndef __MAP_DATA_H__
#define __MAP_DATA_H__


#include <boost/unordered_map.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <time.h>
#include <sparsehash/dense_hash_map>

#include "concurrent_hdtm.hpp"

static const uint32_t MAX_CAP = 4606884;
static const uint32_t MAX_TOKEN_CAP = 6064216;
static const uint32_t EMPTY_KEY= 9000000;

typedef boost::unordered_map<uint32_t, feature_sequence_map> fs_map_type;

class corpus_data {

private:

	dense_hash_map<double, double, hash<double>, eqdl> g_log_map;

	dense_hash_map<uint32_t, uint32_g_map, hash<uint32_t>, eqint> g_fs_token_map;

	dense_hash_map<uint32_t, feature_sequence_g_map, hash<uint32_t>, eqint> g_fs_map;

	uint32_g_map g_fs_map_size;

	dense_hash_map<uint32_t, uint32_g_map, hash<uint32_t>, eqint> g_token_map;

	uint32_g_map g_token_map_size;

	boost::unordered_map<uint32_t, boost::shared_ptr<boost::mutex> > token_map_mutex;

	dense_hash_map<uint16_t, uint32_g_map, hash<uint16_t>, eqint_16> g_level_map;

	boost::mutex _mtx;
	
public:

	corpus_data() {}

	corpus_data(uint32_t num_vertices) {
		token_map_mutex.reserve(num_vertices);

		g_fs_map = 
			dense_hash_map<uint32_t, feature_sequence_g_map, hash<uint32_t>, eqint>(MAX_CAP);

		g_fs_map.set_empty_key(0);
		g_fs_map.set_deleted_key(1);

		g_fs_map_size = uint32_g_map(MAX_CAP);
		g_fs_map_size.set_empty_key(0);
		g_fs_map_size.set_deleted_key(1);

		g_token_map_size = uint32_g_map(MAX_CAP);
		g_token_map_size.set_empty_key(0);
		g_token_map_size.set_deleted_key(1);

		g_token_map = dense_hash_map<uint32_t, uint32_g_map, hash<uint32_t>, eqint>(MAX_CAP);
		g_token_map.set_empty_key(0);
		g_token_map.set_deleted_key(1);

		g_level_map = dense_hash_map<uint16_t, uint32_g_map, hash<uint16_t>, eqint_16>(65535);
		g_level_map.set_empty_key(65535);
		g_level_map.set_deleted_key(65534);

		g_log_map = dense_hash_map<double, double, hash<double>, eqdl>(5000000);
		g_log_map.set_empty_key(-5.0);

		g_fs_token_map = dense_hash_map<uint32_t, uint32_g_map, hash<uint32_t>, eqint>(MAX_CAP);
		g_fs_token_map.set_empty_key(0);
		g_fs_token_map.set_deleted_key(1);

	}

	const feature_sequence_g_map& get_feature_sequence(const uint32_t& key)
	{
		return g_fs_map[key];
	}

	const uint32_t& get_feature_sequence_size(const uint32_t& key)
	{
		return g_fs_map_size[key];
	}

	const uint32_g_map& get_token_map(const uint32_t& key)
	{

		return g_token_map[key];
	}

	const uint32_t get_token_count_by_id(const uint32_t& id,
																			  const uint32_t& token)
	{
		uint32_g_map& g_map = g_token_map[id];
		if (g_map.find(token) != g_map.end())
			return g_map[token];
		return 0;
	}

	const uint32_t& get_token_map_size(const uint32_t& key)
	{
		return g_token_map_size[key];
	}

	const uint32_g_map& get_level_map(const uint16_t& level)
	{
		return g_level_map[level];
	}

	const uint32_g_map& get_fs_token_map(const uint32_t& key) 
	{
		return g_fs_token_map[key];
	}

	const uint32_t get_fs_token_size_by_id(const uint32_t& key, const uint32_t& token)
	{
		if (g_fs_token_map[key].find(token) != g_fs_token_map[key].end()) {
			return g_fs_token_map[key][token];
		}
		return 0;
	}

	void construct_level_map()
	{
		g_level_map.clear();
		for(uint16_t l = 0; l < 65534; l++) {
			g_level_map[l] = uint32_g_map(MAX_TOKEN_CAP);
			g_level_map[l].set_empty_key(EMPTY_KEY);
		}
		BOOST_FOREACH(const vertex_feature_sequence_g_map::value_type& i, g_fs_map) {
			BOOST_FOREACH(const feature_sequence_g_map::value_type& j, i.second) {
				BOOST_FOREACH(const uint32_g_map::value_type& z, j.second) {
					g_level_map[j.first][z.first] += z.second;
				}
			}
		}
	}

	void init_token_map(const uint32_t& key)
	{
		token_map_mutex[key] = boost::shared_ptr< boost::mutex >(new boost::mutex);
		g_token_map[key] = 
			dense_hash_map<uint32_t, uint32_t, hash<uint32_t>, eqint>(MAX_TOKEN_CAP);
		g_token_map[key].set_empty_key(EMPTY_KEY);
		g_token_map_size[key] = 0;
	}

	void init_feature_sequence(const uint32_t& key, const feature_sequence_map& new_map)
	{
		bool init_size = false;
		if(g_fs_map_size.find(key) == g_fs_map_size.end()) {
			init_size = true;
		}
		g_fs_map[key] = feature_sequence_g_map(65535);
		g_fs_map[key].set_empty_key(65535); // MAX_LEVEL
		g_fs_map[key].set_deleted_key(65534);
		g_fs_token_map[key] = uint32_g_map(MAX_TOKEN_CAP);
		g_fs_token_map[key].set_empty_key(EMPTY_KEY);
		BOOST_FOREACH(const feature_sequence_map::value_type& i, new_map) {
			g_fs_map[key][i.first] = uint32_g_map(MAX_TOKEN_CAP);
			g_fs_map[key][i.first].set_empty_key(EMPTY_KEY);
			g_fs_map[key][i.first].set_deleted_key(EMPTY_KEY+1);
			BOOST_FOREACH(const uint32_map::value_type& j, i.second) {
				if (init_size) {
					g_fs_map_size[key] += j.second;
				}
				g_fs_map[key][i.first][j.first] = j.second;
				g_fs_token_map[key][j.first] = j.second;
			}
		}
	}

	void update_feature_sequence(const uint32_t& key, const feature_sequence_g_map& new_map)
	{
		g_fs_map[key] = new_map;
	}

	void update_token_map(const vertex_token_map& map) {
		BOOST_FOREACH(const vertex_token_map::value_type& i, map) {
			const uint32_t& vid = i.first;
			token_map_mutex[vid]->lock();
			BOOST_FOREACH(const token_update_map::value_type& token, i.second) {
				g_token_map_size[vid] += token.second;
				g_token_map[i.first][token.first] += token.second;
			}
			token_map_mutex[vid]->unlock();
		}
	}

	void remove_token_map(const uint32_t& key)
	{
		g_token_map.erase(key);
		g_token_map_size.erase(key);
	}

	void remove_fs_map(const uint32_t& key)
	{
		g_fs_map.erase(key);
		g_fs_map_size.erase(key);
	}

	void remove_article(const uint32_t& key)
	{
		token_map_mutex[key]->lock();
		remove_token_map(key);
		remove_fs_map(key);
		g_fs_token_map.erase(key);
		token_map_mutex[key]->unlock();
	}

	void clear_deleted_articles(uint32_t& key)
	{
		g_token_map.clear_deleted_key();
		g_token_map_size.clear_deleted_key();
		g_fs_map_size.clear_deleted_key();
	}

	double get_log(const double& key)
	{
		if (g_log_map.find(key) == g_log_map.end()) {
			double res = log(key);
			g_log_map[key] = res;
			return res;
		}
		return g_log_map[key];
	}
};

#endif
