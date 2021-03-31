
#include <savvy/writer.hpp>
#include <optional>
#include <savvy/reader.hpp>
#include "sav/stat.hpp"
#include "sav/utility.hpp"
#include "savvy/s1r.hpp"
#include <random>
#include <iostream>
#include "get_variants.hpp"
#include "range/v3/range/conversion.hpp"
#include "range/v3/view/transform.hpp"
#include <range/v3/view/iota.hpp>
#include "range/v3/numeric/accumulate.hpp"
#include "range/v3/algorithm/minmax.hpp"
#include "range/v3/view/sample.hpp"
#include "cxxopts.hpp"

int main(int argc, char** argv){

  cxxopts::Options options("iter_sav", "simple test of iterating sav file");


  options.add_options()
    ("i,input", "sav file to read from", cxxopts::value<std::string>())
    ("n,sample_num", "number of elements to sample from sav file (default is 10)", cxxopts::value<int>())
    ("s,seed", "seed for random number generator", cxxopts::value<int>())
    ("o,output", "file to write output to", cxxopts::value<std::string>())
    ("h,help", "Print usage");;
    std::mt19937 engine;

  int sample_num = 10;
  auto result = options.parse(argc, argv);
  if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
     if (result.count("seed"))
    {
     engine.seed(result["seed"].as<int>());
    }
  if (result.count("sample_num"))
    {
     sample_num = result["sample_num"].as<int>();
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
  const std::string output=result["output"].as<std::string>();

  savvy::reader f(input);
  auto header = f.headers();
  auto samples = f.samples();
  savvy::writer o_f(output,savvy::file::format::sav2,header,samples);
 std::vector<savvy::s1r::index_statistics> stats_v;

  stats_v = savvy::s1r::stat_index(input);
 auto tot_size_r = ranges::views::transform(stats_v,[](const savvy::s1r::index_statistics &sv) -> int{
     return static_cast<int>(sv.record_count);
 });
 int tot_size = ranges::accumulate(tot_size_r,0);
auto idxr = ranges::views::ints(0,tot_size) | ranges::views::sample(sample_num,engine);

  std::unordered_set<int> idx =  ranges::to<std::unordered_set>(idxr);
  const auto [min, max] = ranges::minmax(idx);
  //std::sort(idx.begin(),idx.end());
  f.reset_bounds({static_cast<std::uint64_t>(min),static_cast<std::uint64_t>(max)});

  auto getlv = ranges::getvariants(f);

  //int i=min;
  int i=0;
  for(savvy::variant & var: getlv){
      if(idx.count(i+min)>0)
        o_f.write(var);
    i++;
  }
  std::cout<<i<<std::endl;
  return 0;
}
