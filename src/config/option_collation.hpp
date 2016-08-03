//
//  option_collation.hpp
//  Octopus
//
//  Created by Daniel Cooke on 13/07/2016.
//  Copyright © 2016 Oxford University. All rights reserved.
//

#ifndef option_collation_hpp
#define option_collation_hpp

#include <vector>
#include <cstddef>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

#include "common.hpp"
#include "option_parser.hpp"
#include <io/reference/reference_genome.hpp>
#include <io/read/read_manager.hpp>
#include <readpipe/read_pipe.hpp>
#include <core/callers/caller_factory.hpp>
#include <io/variant/vcf_writer.hpp>
//#include "variant_call_filter.hpp"

namespace fs = boost::filesystem;

namespace octopus { namespace options {

bool is_run_command(const OptionMap& options);

bool is_debug_mode(const OptionMap& options);
bool is_trace_mode(const OptionMap& options);

boost::optional<fs::path> get_debug_log_file_name(const OptionMap& options);
boost::optional<fs::path> get_trace_log_file_name(const OptionMap& options);

boost::optional<unsigned> get_num_threads(const OptionMap& options);

std::size_t get_target_read_buffer_size(const OptionMap& options);

ReferenceGenome make_reference(const OptionMap& options);

InputRegionMap get_search_regions(const OptionMap& options, const ReferenceGenome& reference);

ContigOutputOrder get_contig_output_order(const OptionMap& options);

boost::optional<std::vector<SampleName>> get_user_samples(const OptionMap& options);

ReadManager make_read_manager(const OptionMap& options);

ReadPipe make_read_pipe(ReadManager& read_manager, std::vector<SampleName> samples,
                        const OptionMap& options);

bool call_sites_only(const OptionMap& options);

CallerFactory make_caller_factory(const ReferenceGenome& reference, ReadPipe& read_pipe,
                                  const InputRegionMap& regions, const OptionMap& options);

VcfWriter make_output_vcf_writer(const OptionMap& options);

boost::optional<fs::path> create_temp_file_directory(const OptionMap& options);

} // namespace options
} // namespace octopus

#endif /* option_collation_hpp */