#include <savvy/writer.hpp>
#include <savvy/reader.hpp>
#include "cxxopts.hpp"
#include "vcf_header.hpp"
#include <iostream>
//savvy::writer out("out.bcf", savvy::fmt::bcf, in.headers(), in.samples());


int main(int argc, char** argv){

  cxxopts::Options options("make_header", "Make a header for a VCF");

  options.add_options()
    ("a,assembly", "Human genome assemby to use (currently only 'grch37' and 'grch38' are supported (default is grch38)", cxxopts::value<std::string>()->default_value("grch38"))
    ("o,output", "file to write output to", cxxopts::value<std::string>())
    ("b,bcf", "whether to create a bcf header", cxxopts::value<bool>()->default_value("false"))
    ("h,help", "Print usage");
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
   const std::string output=result["output"].as<std::string>();
   const bool is_hg38=result["assembly"].as<std::string>()=="grch38";

   std::vector<std::string> sample_ids = {};
   std::vector<std::pair<std::string, std::string>> headers;
   if(is_hg38){
     headers.reserve(GRCh38::num_seqs+3);
     headers.push_back({"fileformat", "VCFv4.2"});
     headers.push_back({"FILTER", "<ID=PASS,Description=\"All filters passed\">"});
     headers.push_back(    {"INFO",
         "<ID=ANNO,Number=1,Type=String,Description=\"Annotation at site\">"});
     for(int i=0; i<GRCh38::num_seqs; i++){
       headers.emplace_back(contig_as_header<GRCh38>(i));
     }
   }else{
     headers.reserve(GRCh37::num_seqs+3);
     headers.push_back({"fileformat", "BCFv4.2"});
     headers.push_back({"FILTER", "<ID=PASS,Description=\"All filters passed\">"});
     headers.push_back(    {"INFO",
         "<ID=ANNO,Number=1,Type=String,Description=\"Annotation at site\">"});
     for(int i=0; i<GRCh37::num_seqs; i++){
       headers.emplace_back(contig_as_header<GRCh37>(i));
     }
   }
   savvy::writer out(output, savvy::file::format::vcf, headers, sample_ids);
   return 0;
};
