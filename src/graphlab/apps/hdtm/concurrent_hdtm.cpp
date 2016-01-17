#include "concurrent_hdtm.hpp"
#include "opts.hpp"
#include "map_data.hpp"

#include <graphlab.hpp>

corpus_data global_data(4606884);
uint32_t current_iter = 0;

/**
 * \brief Load vertex and save map to redis
 */
bool vertex_parser(graph_type& graph,
                   const std::string& file_name,
                   const std::string& textline)
{
  std::stringstream strm(textline);
  graphlab::vertex_id_type article_id;

  strm >> article_id;

  feature_sequence_map fs;

  while(1) {
    uint32_t token_id;
    strm >> token_id;
    if(!strm.good()) {
			/* Ignore empty vertex, which is unlikely to happen */
			if(fs.size() == 0) {
				return true;
			}

			{
				global_data.init_token_map(article_id);
				global_data.init_feature_sequence(static_cast<uint32_t>(article_id), fs);
			}

			graph.add_vertex(article_id, vertex_data());

      return true;
    }
    /*! When initializing, we assume all token belongs to root
     * After tree_labeler we will change it to a real distribution. */
    fs[0][token_id]++;
  }

  return true;
}

bool edge_parser(graph_type& graph,
                 const std::string& file_name,
                 const std::string& textline)
{
  std::stringstream strm(textline);
  graphlab::vertex_id_type src(0), dest(0);

  strm >> src >> dest;

	/* Ignore self-links */
  if(src != dest) { 
		/* Ignore empty articles */
		/*! Default edge's is_tree_edge == false */
		graph.add_edge(src, dest, edge_data());
	}

  return true;
}

void find_vertex(const graph_type::vertex_type& vertex, uint8_t& total)
{
	total += vertex.id() == root_node ? 1 : 0;
}

/**
 * \brief message structure for tree_labeler,
 * This message will not combine any other message
 */
struct labeler_message {
  uint32_vector path;
  labeler_message() {}
  labeler_message(const uint32_vector& other)
  {
    path = other;
  }
  labeler_message& operator+=(const labeler_message& rvalue)
  {
    return *this;
  }
  void save(graphlab::oarchive& oarc) const
  {
    oarc << path;
  }
  void load(graphlab::iarchive& iarc)
  {
    iarc >> path;
  }
};

/**
 * \brief Label a tree in original graph, distribute root_path to others, and
 * re-assign tokens based on root_path
 */
class tree_labeler:
  public graphlab::ivertex_program<graph_type, graphlab::empty, labeler_message>
{
private:
  uint32_vector tmp_path;
public:

  void init(icontext_type& context,
            const vertex_type& vertex,
            const message_type& msg)
  {
    tmp_path = msg.path;
  }

  edge_dir_type gather_edges(icontext_type& context,
                             const vertex_type& vertex) const
  {
    return graphlab::NO_EDGES;
  }

  void apply(icontext_type& context,
             vertex_type& vertex,
             const gather_type& total)
  {
    /* Before construct path, we need to make sure the path is empty */
		ASSERT_EQ(vertex.data().root_path.size(), 0);
    if (vertex.data().root_path.size() == 0) {
      uint32_vector& root_path = vertex.data().root_path;
      root_path.insert(root_path.end(), tmp_path.begin(), tmp_path.end());
      root_path.push_back(vertex.id());

      /* After root_path construction, we randomly assign level to tokens
       * and write back to redis */
      uint16_t level_size = root_path.size();
      feature_sequence_g_map new_fs;
			new_fs.set_empty_key(65535);

			const feature_sequence_g_map& old_fs = global_data.get_feature_sequence(vertex.id());
			vertex_token_map token_map;

      /*! Re-assign levels */
      BOOST_FOREACH(const feature_sequence_g_map::value_type& i, old_fs) {
        BOOST_FOREACH(const uint32_g_map::value_type& j, i.second) {
          for(uint32_t k = 0; k < j.second; k++) {
            /* Randomly select a new level for this token */
            uint16_t assign_level = graphlab::random::fast_uniform(
                                      static_cast<uint16_t>(0),
                                      static_cast<uint16_t>(level_size - 1));
						if (new_fs.find(assign_level) == new_fs.end()){
							new_fs[assign_level].set_empty_key(MAX_TOKEN_CAP);
						}
            new_fs[assign_level][j.first]++;
						/* Construct vertex->token->count map by new_fs */
						token_map[vertex.data().root_path[assign_level]][j.first]++;
          }
        }
      }

			{
				global_data.update_feature_sequence(static_cast<uint32_t>(vertex.id()), new_fs);
			}

			/*! Initialize token map for parents */
			{
				global_data.update_token_map(token_map);
			}
    }
  }

  edge_dir_type scatter_edges(icontext_type& context,
                              const vertex_type& vertex) const
  {
    return graphlab::ALL_EDGES;
  }

  void scatter(icontext_type& context,
               const vertex_type& vertex,
               edge_type& edge) const
  {
    const uint32_t& parent =
      vertex.data().root_path[vertex.data().root_path.size() - 2];
    if(edge.source().id() == parent) {
			// IN_EDGES
      edge.data().is_tree_edge = true;
    } else {
			// OUT_EDGES
      if(edge.target().id() != vertex.id() &&
					 edge.target().data().root_path.size() == 0) {
        context.signal(edge.target(), labeler_message(vertex.data().root_path));
      }
    }
  }

  void save(graphlab::oarchive& oarc) const
  {
    oarc << tmp_path;
  }

  void load(graphlab::iarchive& iarc)
  {
    iarc >> tmp_path;
  }
};

void remove_unreached_vertices(const graph_type::vertex_type& vertex,
															 uint32_t& total)
{
	if (vertex.data().root_path.size() == 0){
		global_data.remove_article(vertex.id());
		total++;
	}
}

/*! Aggregrate the number of vertices that have been visited */
uint64_t labeled_vertices(tree_labeler::icontext_type& context,
                          const graph_type::vertex_type& vertex)
{
  return vertex.data().root_path.size() != 0 ? 1 : 0;
}
/*! Check if all the vertices have been visited */
void validate_labeled_vertices(tree_labeler::icontext_type& context,
                               const uint64_t total)
{
  ASSERT_EQ(total, context.num_vertices());
}
/*! Aggregrate the number of vertices that have been labeled as tree edges */
uint64_t labeled_edges(tree_labeler::icontext_type& context,
                       const graph_type::edge_type& edge)
{
  return is_tree_edge(edge.data()) ? 1 : 0;
}


/*! Check if all we have labeled # (vertices - 1) edges  */
void validate_labeled_edges(tree_labeler::icontext_type& context,
                            const uint64_t total)
{
  ASSERT_EQ(total, context.num_vertices() - 1);
}

class rwr_calculator:
	public graphlab::ivertex_program<graph_type, uint32_t, double>
{
private:
	uint32_t out_degree;
	double parent_score;

public:
	void init(icontext_type& context, 
						const vertex_type& vertex,
						const message_type& msg)
	{
		parent_score = msg;
	}

	edge_dir_type gather_edges(icontext_type& context,
														 const vertex_type& vertex) const
	{
		return graphlab::OUT_EDGES;
	}

	uint32_t gather(icontext_type& context,
									const vertex_type& verte,
									edge_type& edge) const
	{
		return is_tree_edge(edge.data()) ? 1 : 0;
	}

	void apply(icontext_type& context,
						 vertex_type& vertex,
						 const gather_type& total)
	{
		vertex.data().rwr_score = parent_score + LOG_GAMMA;
		out_degree = total;
	}

	edge_dir_type scatter_edges(icontext_type& context,
	                            const vertex_type & vertex) const
	{
		return graphlab::OUT_EDGES;
	}

	void scatter(icontext_type& context, 
							 const vertex_type& vertex, edge_type& edge) const
	{
		if(is_tree_edge(edge.data())) {

			/* old root_path */
			uint32_vector old_path = edge.target().data().root_path;
			/* new root_path */
			uint32_vector& root_path = edge.target().data().root_path;
			root_path.clear();
			//root_path.reserve(vertex.data().root_path.size() + 1);
			root_path.insert(root_path.begin(), 
											 vertex.data().root_path.begin(),
											 vertex.data().root_path.end());
			root_path.push_back(edge.target().id());

			ASSERT_EQ(edge.target().data().root_path.size(),
								vertex.data().root_path.size() + 1);

			/* Exit if old and new paths are identical */
			bool identical = true;
			if (old_path.size() == root_path.size()) {
				for(size_t pos = 0; pos < root_path.size(); pos++) {
					identical &= (old_path[pos] == root_path[pos]);
				}
			} else {
				identical = false;
			}


			if (!identical) {
				
				const feature_sequence_g_map& map = global_data.get_feature_sequence(edge.target().id());

				/* Update feature sequence based on token_map */
				vertex_token_map token_map;
				feature_sequence_g_map new_fs_g_map;
				new_fs_g_map.set_empty_key(65535);

				BOOST_FOREACH(const feature_sequence_g_map::value_type& i, map) {
					if(new_fs_g_map.find(i.first) == new_fs_g_map.end()) {
						new_fs_g_map[i.first].set_empty_key(EMPTY_KEY);
					}
				}

				BOOST_FOREACH(const feature_sequence_g_map::value_type& i, map) {
					/* i.first level i.second.first token i.second.second value */
					const uint16_t& level = i.first;
					BOOST_FOREACH(const uint32_g_map::value_type& token, i.second) {
						/* Remove old assignment */
						token_map[old_path[level]][token.first] -= token.second;
						/* Construct new fs map, add new assignment */
						if(level < root_path.size()) {
							new_fs_g_map[level][token.first] += token.second;
							token_map[root_path[level]][token.first] += token.second;
						}else{
							if(new_fs_g_map.find(root_path.size() -1) == new_fs_g_map.end()) {
								new_fs_g_map[root_path.size() -1].set_empty_key(EMPTY_KEY);
							}
							new_fs_g_map[root_path.size() - 1][token.first] += token.second;
							token_map[root_path.back()][token.first] += token.second;
						}
					}
				}

				global_data.update_feature_sequence(edge.target().id(), new_fs_g_map);
				global_data.update_token_map(token_map);
			} 
			context.signal(edge.target(), parent_score + log((1 - GAMMA) / out_degree));
		}
	}

  void save(graphlab::oarchive& oarc) const
	{
		oarc << parent_score << out_degree;
	}

	void load(graphlab::iarchive& iarc)
	{
		iarc >> parent_score >> out_degree;
	}

};

class children_aggregator:
	public graphlab::ivertex_program<graph_type,
																	 graphlab::empty,
																	 uint32_set>
{
private:
	uint32_set msg_set;

public:

	void init(icontext_type& context,
					  const vertex_type& vertex,
						const message_type& message) {
		msg_set = message;
	}

	edge_dir_type gather_edges(icontext_type& context,
														 const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void apply(icontext_type& context,
						 vertex_type& vertex,
						 const gather_type& total)
	{
		/* First time visit */
		if (vertex.data().children.size() == 0) {
			BOOST_FOREACH(const uint32_t& target, vertex.data().root_path) {
				if (vertex.id() != target){
					uint32_set tmp_set;
					tmp_set.insert(vertex.id());
					context.signal_vid(target, tmp_set);
				}
			}
			vertex.data().children.insert(vertex.id());
		} else {
			/* Second time, just write down aggregrated children */
			vertex.data().children.insert(msg_set.begin(), msg_set.end());
		}
	}

	edge_dir_type scatter_edges(icontext_type& context,
															const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void save(graphlab::oarchive& oarc) const {}
	void load(graphlab::iarchive& iarc) {}
};

class parent_selector:
	public graphlab::ivertex_program<graph_type,
																	 vertex_level_rwr_map,
																	 graphlab::empty>
{

private:

public:
	edge_dir_type gather_edges(icontext_type& context,
														 const vertex_type& vertex) const
	{
		return graphlab::IN_EDGES;
	}

	vertex_level_rwr_map gather(icontext_type& context,
															const vertex_type& vertex,
															edge_type& edge) const
	{
		vertex_level_rwr_map map;
		map[edge.source().id()] = uint16_double_pair(
																edge.source().data().root_path.size() - 1,
																edge.source().data().rwr_score);
		return map;
	}

	void apply(icontext_type& context, 
						 vertex_type& vertex,
						 const gather_type& total)
	{
		if(vertex.id() == root_node)
			return;

		if(vertex.data().root_path.size() == 0)
			return;

		if(total.size() == 0) {
			vertex.data().new_parent = vertex.data().root_path[vertex.data().root_path.size() - 2];
			return;
		}
		score_map score;
		uint32_set level_set;

		feature_sequence_g_map fs_map;
		fs_map.set_empty_key(65535);

		BOOST_FOREACH(const uint32_t& child, vertex.data().children) {
			const feature_sequence_g_map& tmp = global_data.get_feature_sequence(child);
			BOOST_FOREACH(const feature_sequence_g_map::value_type& i, tmp) {
				if(fs_map.find(i.first) == fs_map.end()){
					fs_map[i.first] = uint32_g_map(MAX_TOKEN_CAP);
					fs_map[i.first].set_empty_key(EMPTY_KEY);
				}
				BOOST_FOREACH(const uint32_g_map::value_type& j, i.second) {
					fs_map[i.first][j.first] += j.second;
				}
			}
		}

		/* Get rwr score */
		BOOST_FOREACH(const vertex_level_rwr_map::value_type& i, total) {
			level_set.insert(i.second.first);
		}


		/* get prob */

		BOOST_FOREACH(const feature_sequence_g_map::value_type& i, fs_map) {
			double level_score = 0.0;
			uint32_t total_token = 0;
			/* This is a valid level */
			if(level_set.find(i.first) != level_set.end()) {
				if(i.first != 0) {
					BOOST_FOREACH(const uint32_g_map::value_type& elm, i.second) {
						for(uint32_t token_cnt = 0; token_cnt < elm.second; token_cnt++) {
							level_score += log((double)(ETA + token_cnt) / (double)(ETA_SUM + total_token));
							total_token++;
						}
					}
				}
				score[i.first] = level_score;
			}
		}


		/* Pick new parent */

		uint32_vector candidates;
		std::vector<double> probs;

		double weight_max = -DBL_MAX;
	
		BOOST_FOREACH(const score_map::value_type& i, score) {
			if (weight_max < i.second)
				weight_max = i.second;
		}
		
		uint32_double_map final_score;
		BOOST_FOREACH(const vertex_level_rwr_map::value_type& i, total) {
			//TODO Check inf problem
			double tmp_score = 
				exp(score[i.second.first] + i.second.second - weight_max);
			candidates.push_back(i.first);
			probs.push_back(tmp_score);
		}

		vertex.data().new_parent = candidates[graphlab::random::multinomial(probs)];
		// after burn-in
		if (current_iter >= burn_in) {
			// parent changed
			if (vertex.data().new_parent != 
					vertex.data().root_path[vertex.data().root_path.size() - 2]) {
					vertex.data().nchanged++;
			}
			// log parent count
			if(vertex.data().change_map.find(vertex.data().new_parent) 
					== vertex.data().change_map.end()) {
				vertex.data().change_map[vertex.data().new_parent] = 0;
			}
				vertex.data().change_map[vertex.data().new_parent]++;
		}
	}


	edge_dir_type scatter_edges(icontext_type& context,
														 const vertex_type& vertex) const
	{
		return graphlab::IN_EDGES;
	}

	void scatter(icontext_type& context,
							 const vertex_type& vertex,
							 edge_type& edge) const
	{
		if (edge.source().id() == vertex.data().new_parent) {
			edge.data().is_tree_edge = true;
		}else{
			edge.data().is_tree_edge = false;
		}
	}

	void save(graphlab::oarchive& oarc) const {}
	void load(graphlab::iarchive& iarc) {}
};

void clean_children_rwr(graph_type::vertex_type& vertex) {
	vertex.data().children.clear();
	vertex.data().rwr_score = 0.0;
}

void get_connected_vertices(const graph_type::vertex_type& vertex, uint32_t& acc) {
	acc += vertex.data().root_path.size() > 0 ? 1 : 0;
}

void get_tree_edges(const graph_type::edge_type& edge, uint32_t& acc) {
	acc += edge.data().is_tree_edge ? 1 : 0;
}

void get_vertex_set(const graph_type::vertex_type& vertex, uint32_set& total){
	if (vertex.data().root_path.size() > 0)
		total.insert(vertex.id());
}

void get_total_node_weight(const graph_type::vertex_type& vertex, double& total) {
	if(vertex.data().root_path.size() > 0)
		total += vertex.data().node_weight;
}

class loglikelihood_calculator :
  public graphlab::ivertex_program<graph_type, graphlab::empty, graphlab::empty>
{
public:
	edge_dir_type gather_edges(icontext_type& context,
														 const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void apply(icontext_type& context,
						 vertex_type& vertex,
						 const gather_type& total)
  {
		if(vertex.data().root_path.size() == 0) {
			vertex.data().node_weight = 0.0;
			return;
		}
		
		
		/* Token assigned to this level */
		uint32_t total_tokens = global_data.get_feature_sequence_size(vertex.id());

		/* Calculate log likelihood */
		const uint32_g_map& target_level_map = 
					global_data.get_level_map(vertex.data().root_path.size() - 1);

		uint32_t acc_total_tokens = 0;
		vertex.data().node_weight = 0.0;

		BOOST_FOREACH(const uint32_g_map::value_type& token, target_level_map) {
			uint32_t tcount = global_data.get_fs_token_size_by_id(vertex.id(), token.first);

			/*
			 * Stirling's approximation
			 * http://en.wikipedia.org/wiki/Stirling%27s_approximation
			*/
			acc_total_tokens += token.second;
			double p1 = ETA + tcount + token.second - 1;
			double p2 = ETA + tcount; 
			double p3 = ETA_SUM + total_tokens + acc_total_tokens - 1;
			double p4 = ETA_SUM + total_tokens;
			vertex.data().node_weight = 
				(p1) * log(p1) - (p2) * log(p2)
				- (p3) * log(p3) + (p4) * log(p4);
		}
	}

	edge_dir_type scatter_edges(icontext_type& context,
															const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void save(graphlab::oarchive& oarc) const {}
	void load(graphlab::iarchive& iarc) {}
};

void get_max_depth(const graph_type::vertex_type& vertex, uint32_t& total)
{
	if (vertex.data().root_path.size() > 0 && 
			(vertex.data().root_path.size() - 1 > total))
		total = vertex.data().root_path.size() - 1;
}

void get_sum_depth(const graph_type::vertex_type& vertex, uint32_t& total)
{
	if (vertex.data().root_path.size() > 0)
		total += vertex.data().root_path.size() - 1;
}

void get_sum_degree(const graph_type::edge_type& edge, uint32_t& total)
{
	if(is_tree_edge(edge.data()))
		total++;
}

void record_parent(const graph_type::vertex_type& vertex, graphlab::empty& total)
{
	if(vertex.data().root_path.size() > 0 &&
			 vertex.data().new_parent != vertex.id()) {
		//conn_pool.batch_record_parent(vertex.id(), vertex.data().new_parent);
	}
}

class topic_sampler :
	public graphlab::ivertex_program<graph_type, graphlab::empty, graphlab::empty>
{
public:
	edge_dir_type gather_edges(icontext_type& context,
															const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void apply(icontext_type& context,
						 vertex_type& vertex,
						 const gather_type& total)
	{
		/* Skip root node */
		if(vertex.id() == root_node) return;
		/* Skip un-connected nodes */
		if(vertex.data().root_path.size() == 0) return;

		ASSERT_EQ(vertex.data().root_path[0], root_node);

		const feature_sequence_g_map& fs = global_data.get_feature_sequence(vertex.id());

		uint32_map path_token_total;

		const uint32_vector& root_path = vertex.data().root_path;

		uint32_map level_count;

		BOOST_FOREACH(const uint32_t& id, vertex.data().root_path) {
			path_token_total[id] = 0;
			try{
				//path_token_total[id] = conn_pool.get_size_by_id(id);
				path_token_total[id] = global_data.get_token_map_size(id);
			}catch(std::out_of_range& err) {
			}
		}

		for(uint32_t pos = 0; pos < vertex.data().root_path.size(); pos++) {
			level_count[pos] = 0;
		}

		BOOST_FOREACH(const feature_sequence_g_map::value_type& i, fs) {
			BOOST_FOREACH(const uint32_g_map::value_type& j, i.second) {
				level_count[i.first] += j.second;
			}
		}

		vertex_token_map new_token_map;	
		feature_sequence_g_map new_fs;
		new_fs.set_empty_key(65535);
		//new_fs.reserve(fs.size());

		BOOST_FOREACH(const feature_sequence_g_map::value_type& i, fs) {
			BOOST_FOREACH(const uint32_g_map::value_type& j, i.second) {
				// Every token

				std::vector<double> level_probs;
				std::vector<uint32_t> level_candidates;
				level_count[i.first]--;

				for(uint32_t lev = 0; lev < vertex.data().root_path.size(); lev++) {
					uint32_t a = level_count[lev];
					const uint32_t b = global_data.get_token_count_by_id(root_path[lev], j.first);
					uint32_t c = path_token_total[root_path[lev]];
					level_probs.push_back((double)(ALPHA + a) * 
																	(double)(ETA + b) /
																	(double)(ETA_SUM + c));
					level_candidates.push_back(lev);
				}

				uint32_t new_level = level_candidates[graphlab::random::multinomial(level_probs)];

				level_count[new_level]++;
				/* construct new feature sequence */
				if(new_fs.find(new_level) == new_fs.end()) {
					new_fs[new_level].set_empty_key(MAX_TOKEN_CAP);
				}
				new_fs[new_level][j.first]++;
				/* construct modification of token map */
				new_token_map[root_path[i.first]][j.first]--;
				new_token_map[root_path[new_level]][j.first]++;
			}
		}

		global_data.update_feature_sequence(vertex.id(), new_fs);
		global_data.update_token_map(new_token_map);

		vertex.data().finished = true;
	}

	edge_dir_type scatter_edges(icontext_type& context,
															const vertex_type& vertex) const
	{
		return graphlab::NO_EDGES;
	}

	void save(graphlab::oarchive& oarc) const {}
	void load(graphlab::iarchive& iarc) {}

};

uint32_t job_finished(topic_sampler::icontext_type& context,
											const graph_type::vertex_type& vertex) {
	return vertex.data().finished ? 1 : 0;
}

void print_finished_job(topic_sampler::icontext_type& context,
												uint32_t total) {
	std::cout << "\n" << "Finished " << total << "\n";
}	

int main(int argc, char** argv)
{
  global_logger().set_log_level(LOG_EMPH);
  global_logger().set_log_to_console(true);

  /* Init graphlab */
  graphlab::mpi_tools::init(argc, argv);
  graphlab::distributed_control dc;

  std::string vertices_file;
  std::string edges_file;
  std::string exec_type;

  /* Parse cmdline options */
  if(!get_options(argc, argv, vertices_file,
                  edges_file, exec_type, binary_file_prefix,
									load_from_binary, root_node,
                  num_tokens, MAX_ITER, burn_in, sample, GAMMA, ALPHA, ETA)) {
    graphlab::mpi_tools::finalize();
    return EXIT_FAILURE;
  }

  /*! Pre-calculate log result for rwr_score */
  {

    LOG_GAMMA = log(GAMMA);
    LOG_1_GAMMA = log(1-GAMMA);
    ETA_SUM = static_cast<double>(num_tokens) * ETA;
  }

	{
		logstream(LOG_EMPH) << "gamma" << GAMMA << std::endl;
		logstream(LOG_EMPH) << "tokens " << num_tokens 
												<< " ETA_SUM " << ETA_SUM << std::endl;
		logstream(LOG_EMPH) << "log(GAMMA) " << LOG_GAMMA 
												<< " log(1-GAMMA) " << LOG_1_GAMMA<<std::endl;
	}

  graph_type graph(dc);
  /*! Load graph */

	graphlab::timer init_timer;
	init_timer.start();

  {
		if(load_from_binary){
				graph.load_binary(binary_file_prefix);
		}else{
				graphlab::timer timer;
				if(dc.procid() == 0) {
				  timer.start();
				}
				graph.load(vertices_file, vertex_parser);
				graph.load(edges_file, edge_parser);
				graph.finalize();
				
				if(dc.procid() == 0)
				  logstream(LOG_EMPH) << " Graph loaded, costs "
				                      << timer.current_time()
				                      << " seconds" << std::endl;
		}
  }

	ASSERT_EQ(graph.fold_vertices<uint8_t>(find_vertex), 1);


  /*! Initialize graph */
	/* Ignore this costly initialization step if we load from binary */
  if (!load_from_binary){
    /*! Here we use async because there is no race or message combination
     * during execution, once we access all the vertices, the procedure
     * is done.
		 * Since we need to communicate with redis, this must be a sync engine 
		 * otherwise there will be segment fault.
     */
    graphlab::omni_engine<tree_labeler> tl_engine(dc, graph, "sync");

    graphlab::timer timer;
    if(dc.procid() == 0)
      timer.start();

    tl_engine.signal(root_node, labeler_message());
    tl_engine.start();
	
    if(dc.procid() == 0) {
      logstream(LOG_EMPH) << " Tree labeler validation finish, costs "
                          << timer.current_time()
                          << " seconds " << std::endl;
    }
  }

	if(dc.procid() == 0) {
		logstream(LOG_EMPH) << " Initialization finished " 
												<< init_timer.current_time()
												<< " seconds " << std::endl;
	}

	ASSERT_EQ(graph.fold_vertices<uint32_t>(get_connected_vertices), 
						graph.fold_edges<uint32_t>(get_tree_edges) + 1);

	uint32_t removed = graph.fold_vertices<uint32_t>(remove_unreached_vertices);
	logstream(LOG_EMPH) << "remove " << removed << " unreached articles\n";

	/* Save only if we do not load from binary graph file */
	if (!load_from_binary && !boost::equal(binary_file_prefix, "")) {
		//graph.save_binary(binary_file_prefix);
	}

	/* Main logic */
	double best = -DBL_MAX;

	while(current_iter < MAX_ITER) {

		{
			/*! Clean up rwr score and children */
			graph.transform_vertices(clean_children_rwr);
		}

		{
			/*! Calculate rwr */
			/* This will update assignment map for last iteration */
			graphlab::omni_engine<rwr_calculator> rwr_engine(dc, graph, "sync");
			graphlab::timer timer;
			timer.start();

			rwr_engine.signal(root_node, 0.0);
			rwr_engine.start();

			if(dc.procid() == 0) {
				logstream(LOG_EMPH) << " rwr calculation finish, costs "
				                    << timer.current_time()
                            << " seconds " << std::endl;
//				logstream(LOG_EMPH) << " token map cached " << conn_pool.cache_size() << "\n";
			}
		}
		/* Now we should have a global token_map for entire graph */

		{
			//! Sample Topic 
			// This will reassign tokens to proper level 
			graphlab::omni_engine<topic_sampler> tc_engine(dc, graph, "sync");
			graphlab::timer timer;
			timer.start();

			tc_engine.signal_all();
			tc_engine.start();

			if(dc.procid() == 0) {
				logstream(LOG_EMPH) << " topic sampler calculation finish, costs "
				                    << timer.current_time()
                            << " seconds " << std::endl;
			}
		} 

		/* After sampling, we have an updated global token map */

		dc.cout() << current_iter << " " << burn_in << " " << current_iter << " " << sample << std::endl;
		if (current_iter >= burn_in && (current_iter % sample == 0)) {
			dc.cout() << "Graph evaluation starting, it may take a while...\n";
			graphlab::timer timer;
			timer.start();

			double ll =  0.0;

			if (current_iter + sample >= MAX_ITER) {
			
				if(dc.procid() == 0) {
					global_data.construct_level_map();
				}
				dc.barrier();
				dc.cout() << "Construct level_map costs " << timer.current_time() << std::endl;
				timer.start();
				graphlab::omni_engine<loglikelihood_calculator> lc_engine(dc, graph, "sync");
	
				timer.start();
				lc_engine.signal_all();
				lc_engine.start();
	
				ll = graph.fold_vertices<double>(get_total_node_weight);
			}

			uint32_t max_depth = graph.fold_vertices<uint32_t>(get_max_depth);
			uint32_t sum_depth = graph.fold_vertices<uint32_t>(get_sum_depth);
			uint32_t sum_degree = graph.fold_edges<uint32_t>(get_sum_degree);
			std::ostringstream strm;
			strm << "\n" << "iter: "<< current_iter << "\t ll: " << ll
					 << "\t max_depth: " << max_depth << "\t avg_depth: " 
					 << (static_cast<double>(sum_depth) / (graph.num_vertices() - removed))
					 << "\t avg_degree: "
					 << (static_cast<double>(sum_degree) / (graph.num_vertices() - removed))
					 << "\n";
			dc.cout() << strm.str();

			//if (ll > best) {
				/* This is better than before, save this graph */
				/* Save the graph anyway */
				best = ll;
				graphlab::timer save_timer;
				save_timer.start();
				std::ostringstream new_prefix;
				new_prefix << binary_file_prefix << current_iter << "_" << ll << "_";
				if(dc.procid() == 0)
					logstream(LOG_EMPH) << "saving best graph. This may take a while\n";
				graph.save_binary(new_prefix.str());
				if(dc.procid() == 0)
					logstream(LOG_EMPH) << "save best graph finished, costs "
															<< save_timer.current_time()
															<< " seconds."<<std::endl;
			//}

			/* Save parents */
			graph.fold_vertices<graphlab::empty>(record_parent);
		}

		{
			/*! Children aggregator */
			graphlab::omni_engine<children_aggregator> ca_engine(dc, graph, "sync");
			graphlab::timer timer;
			timer.start();

			ca_engine.signal_all();
			ca_engine.start();

			if(dc.procid() == 0) {
				logstream(LOG_EMPH) << " children aggregation finish, costs "
				                    << timer.current_time()
                            << " seconds " << std::endl;
			}
		}

		{
			/*! Pick parent */
			graphlab::omni_engine<parent_selector> ps_engine(dc, graph, "sync");
			graphlab::timer timer;

			ps_engine.signal_all();
			ps_engine.start();
			if(dc.procid() == 0) {
				logstream(LOG_EMPH) << " parent selection finish, costs "
				                    << timer.current_time()
                            << " seconds " << std::endl;
			}
		}

		current_iter++;
		dc.cout()<< "ITER "<< current_iter << " finish\n";
	}

  graphlab::mpi_tools::finalize();
  return EXIT_SUCCESS;
}
