
#include <savvy/writer.hpp>
#include <optional>
#include <savvy/reader.hpp>
#include <iostream>
#include "get_variants.hpp"
#include "range/v3/view/transform.hpp"
#include "cxxopts.hpp"

int main(int argc, char** argv){

  cxxopts::Options options("iter_sav", "simple test of iterating sav file");


  options.add_options()
    ("i,input", "sav file to read from", cxxopts::value<std::string>())
    ("o,output", "file to write output to", cxxopts::value<std::string>())
    ("h,help", "Print usage");;

  auto result = options.parse(argc, argv);
  if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
  if (!result.count("output"))
    {
      std::cerr<<"must specify 'output'"<<std::endl;
      std::cout << options.help() << std::endl;
      exit(1);
    }
  if (!result.count("input"))
    {
      std::cerr<<"must specify 'output'"<<std::endl;
      std::cout << options.help() << std::endl;
      exit(1);
    }
  //  const std::string output=result["output"].as<std::string>();
  const std::string input=result["input"].as<std::string>();

  savvy::reader f(input);
  auto getlv = ranges::getvariants(f);
  


  int i=0;
  for(savvy::variant & var: getlv){
    i++;
  }
  std::cout<<i<<std::endl;
  return 0;
}
