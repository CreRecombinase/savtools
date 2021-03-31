#include <savvy/writer.hpp>
#include <optional>
#include <savvy/reader.hpp>
#include "cxxopts.hpp"
#include "vcf_header.hpp"
#include <iostream>
#include <algorithm>
#include <string_view>
#include <array>
#include <charconv>

constexpr std::array<char,255> encoding_map(){
  std::array<char,255> retmap{};
  retmap[':']=1;
  retmap[';']=2;
  retmap['=']=3;
  retmap['%']=4;
  retmap[',']=5;
  retmap[13]=6;
  retmap[10]=7;
  retmap[9]=8;
  return retmap;
}


int encode_string(const std::string& input,std::string& output){
  constexpr std::string_view reps[8]={"%3A","%3B","%3D","%25","%2C","%0D","%0A","%09"};

  output.clear();
  output.reserve(input.size());
  auto ob = std::back_inserter(output);
  constexpr auto em{encoding_map()};
  for(const char & c: input){
    if(em[c]==0)
      output.push_back(c);
    else{
      const std::string_view &sv = reps[em[c]];
      std::copy_n(sv.begin(),sv.size(),ob);
    }
  }
  return output.size()-input.size();
}




// template<int N>
// std::array<std::string_view,N> splitSV(std::string_view strv, const char delim = '\t')
// {
//   std::array<std::string_view,N> output;
//   auto beg = strv.begin();

//     for(int i=0; i<N; i++)
//     {
//       auto second = std::find(beg,strv.end(),delim);

//       if (strv.end() != second)
//         output[i]=std::string_view(beg,second);
//         beg = second + 1;
//     }

//     return output;
// }


int main(int argc, char** argv){

  cxxopts::Options options("make_header", "Make a header for a VCF");

  options.add_options()
    ("a,assembly", "Human genome assemby to use (currently only 'grch37' and 'grch38' are supported (default is grch38)", cxxopts::value<std::string>()->default_value("grch38"))
    ("n,ncbi", "whether to use NCBI style sequence names (no 'chr' prefix))", cxxopts::value<bool>()->default_value("false"))
    ("o,output", "prefix of file to write output to", cxxopts::value<std::string>())
    ("i,input", "file to read from", cxxopts::value<std::string>())
    ("b,bcf", "whether to create a bcf header", cxxopts::value<bool>()->default_value("false"))
    ("s,split_chrom", "split output by chromosome", cxxopts::value<bool>()->default_value("false"))
    ("c,chunk_size", "number of rows per chunk (can be combined with split_chrom)", cxxopts::value<std::int64_t>())
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
   if (!result.count("input"))
    {
      std::cerr<<"must specify 'output'"<<std::endl;
      std::cout << options.help() << std::endl;
      exit(1);
    }
   const bool is_bcf = result.count("bcf")>0;
   const bool is_split = result.count("split_chrom")>0;

   std::optional<std::int64_t>chunk_size = result.count("chunk_size")>0 ? std::optional<std::int64_t>(result["chunk_size"].as<std::int64_t>()) : std::optional<std::int64_t>(std::nullopt);


   const std::string output=result["output"].as<std::string>();
   const std::string input=result["input"].as<std::string>();
   const bool is_hg38=result["assembly"].as<std::string>()=="grch38";
   auto fmto = is_bcf ? savvy::file::format::bcf : savvy::file::format::vcf;

   const bool is_ncbi=result.count("ncbi")>0;

   static_assert(GRCh38::seqname(0,true)[0]!='c');
   static_assert(GRCh38::seqname(0,true)[0]=='1');
   static_assert(GRCh38::seqname(0,false)[0]=='c');
   static_assert(GRCh38::seqname(0,false)[0]!='1');

   static_assert(GRCh37::seqname(0,true)[0]!='c');
   static_assert(GRCh37::seqname(0,true)[0]=='1');
   static_assert(GRCh37::seqname(0,false)[0]=='c');
   static_assert(GRCh37::seqname(0,false)[0]!='1');

   std::vector<std::string> sample_ids = {};
   std::vector<std::pair<std::string, std::string>> headers = is_hg38 ?      headers=make_header<GRCh38>(is_ncbi) :      headers=make_header<GRCh37>(is_ncbi);

   headers.push_back(    {"INFO",
       "<ID=ANNO,Number=1,Type=String,Description=\"Annotation at site\">"});

   savvy::writer out(output, fmto, headers, sample_ids);
   shrinkwrap::bgzf::istream is(input);
   std::string chr_field;
   std::string pos_field;
   std::string ref_field;
   std::string alt_field;
   std::string annotation_field;
   std::string out_annotation_field;
   int pos;
   std::getline(is,out_annotation_field);
   while(is){
     std::getline(is,chr_field,'\t');
     std::getline(is,pos_field,'\t');
     std::getline(is,ref_field,'\t');
     std::getline(is,alt_field,'\t');
     std::getline(is,annotation_field,'\n');
     std::from_chars(pos_field.data(),pos_field.data()+pos_field.size(),pos);
     savvy::variant var(chr_field, pos, ref_field, {alt_field}); // chrom, pos, ref, alts
     encode_string(annotation_field,out_annotation_field);
     var.set_info("ANNO",out_annotation_field);
     out.write(var);
   }
   return 0;
};
