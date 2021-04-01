
#include <savvy/writer.hpp>
#include <optional>
#include <savvy/reader.hpp>
#include <iostream>
#include "get_variants.hpp"
#include "range/v3/view/transform.hpp"
#include <tuple>
#include "zip_intersection_pair.hpp"
#include "cxxopts.hpp"

int main(int argc, char** argv){

  cxxopts::Options options("iter_sav", "simple test of iterating sav file");


  options.add_options()
    ("a,input1", "sav file to read from", cxxopts::value<std::string>())
    ("b,input2", "sav file to read from", cxxopts::value<std::string>())
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
  if (!result.count("input1"))
    {
      std::cerr<<"must specify 'input1'"<<std::endl;
      std::cout << options.help() << std::endl;
      exit(1);
    }
      if (!result.count("input2"))
    {
      std::cerr<<"must specify 'input2'"<<std::endl;
      std::cout << options.help() << std::endl;
      exit(1);
    }
  //  const std::string output=result["output"].as<std::string>();
  const std::string input1=result["input1"].as<std::string>();
  const std::string input2=result["input2"].as<std::string>();

  savvy::reader f1(input1);
  savvy::reader f2(input1);
  auto getlv1 = ranges::getvariants(f1);
  auto getlv2 = ranges::getvariants(f2);

 std::vector<int> a{{1,2,3,4,5,6,7}};
 std::vector<int> b{{2,4,6}};



  // auto comp_vars = [](const savvy::variant& a, const savvy::variant& b){
  //   auto const t_a = std::make_tuple<const std::string&,uint32_t>(a.chrom(),a.pos());
  //   auto const t_b = std::make_tuple<const std::string&,uint32_t>(b.chrom(),b.pos());
  //   return t_a<t_b;
  //   //return a.chrom < b.site_info;
  // };
  
  auto both_v = ranges::views::zip_intersection(a,b);

  static_assert(ranges::same_as<ranges::range_reference_t<decltype(both_v)>,std::pair<const int &,const int &>>);

  int i=0;
  for(std::pair<const int &,const int &> pair_v: both_v){
    i++;
  }
  std::cout<<i<<std::endl;
  return 0;
}
