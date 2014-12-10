#include <graphlab.hpp>

/**
 * \brief Get options from cmd line, give default value if not defined
 */
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
                 double& eta)
{
  static std::string const description =
    "\n======================================================\n"
    "HDTM\n\n"
    "Usage:\n"
    "hdtm --vertices vertices.txt --edges edges.txt --root 2000 --token 30000 --prefix [--gamma --alpha --eta --iter ]\n"
    "gamma default is 0.25\n"
    "alpha defulat is 10\n"
    "eta default is 0.1\n"
    "iter is max iteration we will perform\n";
  graphlab::command_line_options clopts(description);

  // Default values
  exec_type = "synchronous";
  gamma = 0.25;
  alpha = 10;
  eta = 0.1;
  iter = 5;
	burn_in = 100;
	sample = 10;
	num_tokens = 0;


  clopts.attach_option("engine", exec_type, "The engine type synchronous or asynchronous");
  clopts.attach_option("vertices", vertices_file, "The file contains article vertices");
  clopts.attach_option("edges", edges_file, "The file contains article edges");
  clopts.attach_option("root", root_node, "Id of root article");
  clopts.attach_option("gamma", gamma, "Gamma for RWR calculation, default 0.25");
  clopts.attach_option("alpha", alpha, "Alpha, default 10");
  clopts.attach_option("eta", eta, "Eta, default 0.1");
  clopts.attach_option("iter", iter, "Iter, default 5");
	clopts.attach_option("burn", burn_in, "Burn-in, default 100");
	clopts.attach_option("sample", sample, "Sample interval, default 10");
	clopts.attach_option("token", num_tokens, "Number of tokens, run wc -l on dictionary file");
	clopts.attach_option("prefix", binary_file_prefix, "Prefix of binary graph files");
	clopts.attach_option("load", load_from_binary, "Load graph from binary files");

  if(!clopts.parse(argc, argv)) {
    return false;
  }
	
	if(binary_file_prefix.empty()) {
		if(vertices_file.empty()) {
		  logstream(LOG_ERROR) << "No vertices_file was provided.(--vertices)\n";
		  return false;
		}
	
		if(edges_file.empty()) {
		  logstream(LOG_ERROR) << "No edges_file was provided.(--edges)\n";
		  return false;
		}
	}

	if(root_node == 0) {
		logstream(LOG_ERROR) << "No root_node was provided.(--root)\n";
		return false;
	}

	if(num_tokens == 0) {
		logstream(LOG_ERROR) << "No number of tokens was provided(--token)\n";
		return false;
	}

  return true;
}
