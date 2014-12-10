#ifndef __HDTM_OPTS_HPP__
#define __HDTM_OPTS_HPP_

#include <string>
#include <graphlab.hpp>

bool get_options(int argc,
                 char** argv,
								 std::string& vertices_file,
                 std::string& edges_file,
                 std::string& exec_type,
								 std::string& binary_file_prefix,
								 bool& load_from_binary,
                 graphlab::vertex_id_type& root_node,
								 uint64_t& num_tokens,
								 uint32_t& iter,
								 uint32_t& burn_in,
								 uint32_t& sample,
                 double& gamma,
                 double& alpha,
                 double& eta);
#endif
