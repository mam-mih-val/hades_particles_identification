#include <iostream>
#include <chrono>
#include <boost/program_options.hpp>

#include <AnalysisTree/TaskManager.hpp>
#include "analysis_task.h"

int main(int n_args, char** args){
  namespace po=boost::program_options;
  if(n_args<2){
    throw std::runtime_error( "Please type \"./acceptance --help\" to get information" );
  }
  std::string file_list;
  std::string output_file{"output.root"};
  po::options_description options("Options");
  options.add_options()
      ("help,h", "Help screen")
      ("input,i", po::value<std::string>(&file_list),
       "Path to input file list")
      ("output,o", po::value<std::string>(&output_file),
       "output file name");
  po::variables_map vm;
  po::parsed_options parsed = po::command_line_parser(n_args, args).options(options).run();
  po::store(parsed, vm);
  po::notify(vm);
  if (vm.count("help")){
    std::cout << options << std::endl;
    return 0;
  }
  AnalysisTree::TaskManager manager({file_list}, {"hades_analysis_tree"});
  auto *analysis_task = new AnalysisTree::AnalysisTask;

  manager.AddTask(analysis_task);
  manager.SetOutFileName(output_file);
  manager.Init();
  manager.Run(-1);
  manager.Finish();
  return 0;
}