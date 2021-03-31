#pragma once
#include <string>
#include "seqnames.hpp"
#include <fmt/format.h>





// template <typename Assembly,typename FileFormat>
// class Header{
//   int format_major_version;
template<typename Assembly>
inline std::pair<std::string,std::string> contig_as_header(int chrom_id,bool is_ncbi=false){
  const auto name=Assembly::seqname(chrom_id,is_ncbi);
  const auto length=Assembly::seqsize(chrom_id);
  auto sstr=fmt::format("<ID={},length={}>",name,length);
  return {"contig",sstr};
}

// inline std::pair<std::string,std::string> vcf_header_version(int major=4,int minor=2){
//   auto sstr=fmt::format("<ID={},length={}>",name,length);
//   return {"fileformat",sstr};

template<typename Assembly>
inline std::vector<std::pair<std::string,std::string> > make_header(const bool is_ncbi) {
  std::vector<std::pair<std::string, std::string>> headers;
  headers.reserve(Assembly::num_seqs+3);
  headers.push_back({"fileformat", "VCFv4.2"});
  headers.push_back({"FILTER", "<ID=PASS,Description=\"All filters passed\">"});
  for(int i=0; i<Assembly::num_seqs; i++){
    headers.emplace_back(contig_as_header<Assembly>(i,is_ncbi));
  }
  return headers;
}


// class ContigHeader{
//   int name_id;
//   std::int64_t length;
// public:
//   ContigHeader(const std::string &name_, const std::int64_t length_):name(name_),length(length_){}



//   }
// };
