#include "hdtm_ana.hpp"

#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <boost/thread.hpp>

std::ofstream output_file;
boost::mutex output_lock;

bool line_parser(cate_graph_type& graph,
									const std::string& filename,
									const std::string& textline,
									const bool& is_cate) {
	std::stringstream strm(textline);
	graphlab::vertex_id_type src, dest;

	strm >> src >> dest;

	graph.add_vertex(src, cate_vertex_data(is_cate));
	graph.add_vertex(dest, cate_vertex_data());
	graph.add_edge(src, dest);	
	return true;
}

bool page_parser(cate_graph_type& graph,
									const std::string& filename,
									const std::string& textline)
{
	return line_parser(graph, filename, textline, false);
}

bool cate_parser(cate_graph_type& graph,
									const std::string& filename,
									const std::string& textline)
{
	return line_parser(graph, filename, textline, true);
}

struct sp_message_type : public graphlab::IS_POD_TYPE {
	bool is_src_type;
	bool is_src;
	bool is_dest;
	sp_message_type(){
		is_src_type = false;
		is_src = false;
		is_dest = false;
	}
	sp_message_type(bool type) {
		is_src_type = type;
		if (is_src_type) {
			is_src = true;
			is_dest = false;
		} else {
			is_dest = true;
			is_src = false;
		}
	}
	sp_message_type& operator+=(const sp_message_type& other) {
		if (other.is_src_type) {
			is_src = true;
		} else {
			is_dest = true;
		}
		return *this;
	}
};

class shortest_path:
	public graphlab::ivertex_program<cate_graph_type, 
																	 graphlab::empty,
																	 sp_message_type>,
	public graphlab::IS_POD_TYPE {
	private:
		bool is_src;
		bool is_dest;
	public:
		void init(icontext_type& context, const vertex_type& vertex,
							const message_type& msg)
		{
			is_src = vertex.data().src || msg.is_src;
			is_dest = vertex.data().dest || msg.is_dest;
		}
		edge_dir_type gather_edges(icontext_type& context,
															 const vertex_type& vertex) const
		{
			return graphlab::NO_EDGES;
		}
		
		void apply(icontext_type& context, vertex_type& vertex,
							 const gather_type& total) 
		{
			vertex.data().src = vertex.data().src || is_src;
			vertex.data().dest = vertex.data().dest || is_dest;

			if((is_src) && (is_dest)) {
				std::cout << "bingo\n";
				context.stop();
			}
		}

		edge_dir_type scatter_edges(icontext_type& context,
																const vertex_type& vertex) const
		{
			return graphlab::OUT_EDGES;
		}

		void scatter(icontext_type& context, 
								 const vertex_type& vertex,
								 edge_type& edge) const
		{
			if(is_src){
				if(edge.target().id() != vertex.id() && !edge.target().data().src) {
					context.signal(edge.target(), sp_message_type(true));
				} else if(!edge.source().data().src) {
					context.signal(edge.source(), sp_message_type(true));
				}
			}
			if(is_dest){
				if(edge.target().id() != vertex.id() && !edge.target().data().dest) {
					context.signal(edge.target(), sp_message_type(false));
				} else if(!edge.source().data().dest){
					context.signal(edge.source(), sp_message_type(false));
				}
			}
		}
};

class children_random_aggregator:
	public graphlab::ivertex_program<page_graph_type,
																	 std::vector<uint32_t>,
																	 graphlab::empty>,
	public graphlab::IS_POD_TYPE

{
	public:
		edge_dir_type gather_edges(icontext_type& context,
																const vertex_type& vertex) const
		{
			// Get all in-coming edges 
			//if(vertex.data().root_path.size() > 0) {
				return graphlab::IN_EDGES;
			//}
		}

		std::vector<uint32_t> gather(icontext_type& context, 
																 const vertex_type& vertex,
																 edge_type& edge) const 
		{
			/* Get all parent candidates */
			std::vector<uint32_t> s;
			//if(edge.source().data().root_path.size() > 0 && 
			//		edge.source().data().root_path.back() == edge.source().id())
				s.push_back(edge.source().id());
			return s;
		}

		void apply(icontext_type& context, 
							 vertex_type& vertex,
							 const gather_type& total)
		{
			if(vertex.data().root_path.size() > 0 &&
				 vertex.data().root_path.back() == vertex.id()) {
				std::ostringstream ostrm;
				/* randomly select a parent and output */
				for(size_t pos = 0; pos < total.size(); pos++) {
					ostrm << total[pos] << "\t" << vertex.id() << "\n";
					//ostrm << vertex.id() << "\t" << total[pos] << "\n";
				}
				output_lock.lock();
				output_file << ostrm.str();
				output_lock.unlock();
			}
		}

		edge_dir_type scatter_edges(icontext_type& context,
																const vertex_type& vertex) const
		{
			return graphlab::NO_EDGES;
		}
};

class children_aggregator:
	public graphlab::ivertex_program<page_graph_type,
																	 uint32_set,
																	 graphlab::empty>,
	public graphlab::IS_POD_TYPE

{
	public:
		edge_dir_type gather_edges(icontext_type& context,
																const vertex_type& vertex) const
		{
			// if this vertex is changable, get its in-coming edges
			//if(vertex.data().nchanged != 0)
				return graphlab::IN_EDGES;
			//return graphlab::NO_EDGES;
		}

		uint32_set gather(icontext_type& context, const vertex_type& vertex, edge_type& edge) const 
		{
			// if source is in the sub-graph and the it is the true parent
			if(edge.source().data().root_path.size() > 0 
					&& edge.target().data().root_path[edge.target().data().root_path.size() - 2] == edge.source().id()) {
				uint32_set s;
				s.insert(edge.source().id());
				return s;
			}
				return uint32_set();
		}

		void apply(icontext_type& context, vertex_type& vertex, const gather_type& total)
		{
			if(vertex.data().root_path.size() > 1 && 
				 vertex.data().root_path.back() == vertex.id() &&
				 total.size() > 0 &&
				 vertex.num_in_edges() > 1) {
				vertex.data().children = total;
				std::ostringstream ostrm;
				assert(total.size() == 1);

				BOOST_FOREACH(const uint32_t& src, total) {
					ostrm << src << "\t" 
								<< (vertex.data().root_path.size() - 1) << "\t"
								<< vertex.id() << "\n";
				}
				output_lock.lock();
				output_file << ostrm.str();
				output_lock.unlock();
			} else {
				vertex.data().children.clear();
			}
		}

		edge_dir_type scatter_edges(icontext_type& context,
																 const vertex_type& vertex) const
		{
			return graphlab::NO_EDGES;
		}
};

void multi_parent_nodes(const page_graph_type::vertex_type& vertex, uint32_t& total) {
	if (vertex.data().root_path.size() > 0 && vertex.num_in_edges() > 1)
		total++;
}

void num_changed_nodes(const page_graph_type::vertex_type& vertex, uint32_t& total) {
	if(vertex.data().nchanged != 0){
		total++;
	}
}

void changed_nodes(const page_graph_type::vertex_type& vertex, uint32_t& total) {
	if(vertex.data().root_path.size() > 0) {
		output_lock.lock();
		double max_iter = 0;
		double selection = 0;
		uint32_t parent = vertex.data().root_path[vertex.data().root_path.size() - 2];
		std::cout << vertex.data().change_map.size() << "\n";
		BOOST_FOREACH(const uint32_map::value_type& v, vertex.data().change_map) {
			if(v.first == parent) {
				selection = v.second;
			}
				max_iter += v.second;
		}
		output_file << vertex.id() << "\t" << (selection/max_iter) << "\t"
															 << vertex.num_in_edges()<< "\n";
		output_lock.unlock();
		total++;
	}
}

int main(int argc, char** argv)
{
	std::srand(std::time(0));
	output_file.open(argv[2]);

	global_logger().set_log_level(LOG_EMPH);
	global_logger().set_log_to_console(true);

	graphlab::mpi_tools::init(argc, argv);
	graphlab::distributed_control dc;

	page_graph_type page_graph(dc);
	cate_graph_type cate_graph(dc);

	page_graph.load_binary(std::string(argv[1]));
	page_graph.finalize();


	graphlab::omni_engine<children_random_aggregator> ca_engine(dc, page_graph, "sync");
	ca_engine.signal_all();
	ca_engine.start();

	output_file.close();

	output_file.open(argv[3]);
	graphlab::omni_engine<children_aggregator> ca_engine2(dc, page_graph, "sync");
	ca_engine2.signal_all();
	ca_engine2.start();

	output_file.close();
	
	output_file.open(argv[4]);
	uint32_t node_nchange = page_graph.fold_vertices<uint32_t>(changed_nodes);
	output_file.close();

	uint32_t nu = page_graph.fold_vertices<uint32_t>(multi_parent_nodes);
	std::cout << "nodes that have multiple parent candidates " << nu << "\n";

	graphlab::mpi_tools::finalize();

	return 0;
}
