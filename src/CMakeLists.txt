set(CONFIG_SOURCES
    config/common.hpp
    config/common.cpp
    config/option_parser.hpp
    config/option_parser.cpp
    config/option_collation.hpp
    config/option_collation.cpp
    config/octopus_vcf.hpp
    config/octopus_vcf.cpp
)

set(CONCEPTS_SOURCES
    concepts/equitable.hpp
    concepts/comparable.hpp
    concepts/mappable.hpp
    concepts/mappable_range.hpp
)

set(BASICS_SOURCES
    basics/contig_region.hpp
    basics/genomic_region.hpp

    basics/phred.hpp
    basics/cigar_string.hpp
    basics/cigar_string.cpp
    basics/aligned_read.hpp
    basics/aligned_read.cpp
    basics/mappable_reference_wrapper.hpp
)

set(CONTAINERS_SOURCES
    containers/mappable_flat_multi_set.hpp
    containers/mappable_flat_set.hpp
    containers/matrix_map.hpp
    containers/probability_matrix.hpp
)

set(LOGGING_SOURCES
    logging/logging.hpp
    logging/logging.cpp
    logging/progress_meter.hpp
    logging/progress_meter.cpp
)

set(IO_SOURCES
    io/read/htslib_sam_facade.hpp
    io/read/htslib_sam_facade.cpp
    io/read/read_manager.hpp
    io/read/read_manager.cpp
    io/read/read_reader_impl.hpp
    io/read/read_reader.hpp
    io/read/read_reader.cpp
    
    io/reference/caching_fasta.hpp
    io/reference/caching_fasta.cpp
    io/reference/fasta.hpp
    io/reference/fasta.cpp
    io/reference/reference_genome.hpp
    io/reference/reference_genome.cpp
    io/reference/reference_reader.hpp
    io/reference/threadsafe_fasta.hpp
    io/reference/threadsafe_fasta.cpp
    
    io/variant/htslib_bcf_facade.hpp
    io/variant/htslib_bcf_facade.cpp
    io/variant/vcf_header.hpp
    io/variant/vcf_header.cpp
    io/variant/vcf_parser.hpp
    io/variant/vcf_parser.cpp
    io/variant/vcf_reader_impl.hpp
    io/variant/vcf_reader.hpp
    io/variant/vcf_reader.cpp
    io/variant/vcf_record.hpp
    io/variant/vcf_record.cpp
    io/variant/vcf_type.hpp
    io/variant/vcf_type.cpp
    io/variant/vcf_utils.hpp
    io/variant/vcf_utils.cpp
    io/variant/vcf_writer.hpp
    io/variant/vcf_writer.cpp
    io/variant/vcf.hpp
)

set(READPIPE_SOURCES
    readpipe/read_pipe_fwd.hpp
    readpipe/read_pipe.hpp
    readpipe/read_pipe.cpp
    
    readpipe/downsampling/downsampler.hpp
    readpipe/downsampling/downsampler.cpp
    
    readpipe/filtering/read_filter.hpp
    readpipe/filtering/read_filter.cpp
    readpipe/filtering/read_filterer.hpp
    
    readpipe/transformers/read_transform.hpp
    readpipe/transformers/read_transform.cpp
    readpipe/transformers/read_transformer.hpp
    readpipe/transformers/read_transformer.cpp
)

set(UTILS_SOURCES
    utils/append.hpp
    utils/compression.hpp
    utils/compression.cpp
    utils/hash_functions.hpp
    utils/map_utils.hpp
    utils/mappable_algorithms.hpp
    utils/maths.hpp
    utils/merge_transform.hpp
    utils/read_algorithms.hpp
    utils/read_algorithms.cpp
    utils/read_stats.hpp
    utils/read_stats.cpp
    utils/sequence_complexity.hpp
    utils/sequence_utils.hpp
    utils/string_utils.hpp
    utils/timing.hpp
    utils/type_tricks.hpp
)

set(CORE_SOURCES
    core/callers/caller_builder.hpp
    core/callers/caller_builder.cpp
    core/callers/caller_factory.hpp
    core/callers/caller_factory.cpp
    core/callers/caller.hpp
    core/callers/caller.cpp
    core/callers/cancer_caller.hpp
    core/callers/cancer_caller.cpp
    core/callers/individual_caller.hpp
    core/callers/individual_caller.cpp
    core/callers/population_caller.hpp
    core/callers/population_caller.cpp
    core/callers/trio_caller.hpp
    core/callers/trio_caller.cpp
    
    core/callers/utils/call_types.hpp
    core/callers/utils/call.hpp
    core/callers/utils/call.cpp
    core/callers/utils/germline_variant_call.hpp
    core/callers/utils/germline_variant_call.cpp
    core/callers/utils/haplotype_filter.hpp
    core/callers/utils/haplotype_filter.cpp
    core/callers/utils/reference_call.hpp
    core/callers/utils/reference_call.cpp
    core/callers/utils/somatic_call.hpp
    core/callers/utils/somatic_call.cpp
    core/callers/utils/variant_call.hpp
    core/callers/utils/variant_call.cpp
    core/callers/utils/vcf_header_factory.hpp
    core/callers/utils/vcf_header_factory.cpp
    core/callers/utils/vcf_record_factory.hpp
    core/callers/utils/vcf_record_factory.cpp
    
    core/csr/facets/read_assignment.hpp
    core/csr/facets/read_assignment.cpp
    
    core/csr/filters/supervised_variant_call_filter.hpp
    core/csr/filters/supervised_variant_call_filter.cpp
    core/csr/filters/threshold_filter.hpp
    core/csr/filters/threshold_filter.cpp
    core/csr/filters/variant_call_filter.hpp
    core/csr/filters/variant_call_filter.cpp
    
    core/csr/measures/measure.hpp
    core/csr/measures/quality_by_depth.hpp
    core/csr/measures/quality_by_depth.cpp
    
    core/csr/utils/genotype_reader.hpp
    core/csr/utils/genotype_reader.cpp
    core/csr/utils/variant_call_filter_factory.hpp
    core/csr/utils/variant_call_filter_factory.cpp
    
    core/models/haplotype_likelihood_cache.hpp
    core/models/haplotype_likelihood_cache.cpp
    core/models/haplotype_likelihood_model.hpp
    core/models/haplotype_likelihood_model.cpp
    
    core/models/genotype/cnv_model.hpp
    core/models/genotype/cnv_model.cpp
    core/models/genotype/coalescent_model.hpp
    core/models/genotype/coalescent_model.cpp
    core/models/genotype/germline_likelihood_model.hpp
    core/models/genotype/germline_likelihood_model.cpp
    core/models/genotype/individual_model.hpp
    core/models/genotype/individual_model.cpp
    core/models/genotype/population_model.hpp
    core/models/genotype/population_model.cpp
    core/models/genotype/somatic_mutation_model.hpp
    core/models/genotype/somatic_mutation_model.cpp
    core/models/genotype/tumour_model.hpp
    core/models/genotype/tumour_model.cpp
    core/models/genotype/variational_bayes_mixture_model.hpp
    
    core/models/pairhmm/pair_hmm.hpp
    core/models/pairhmm/pair_hmm.cpp
    core/models/pairhmm/simd_pair_hmm.hpp
    core/models/pairhmm/simd_pair_hmm.cpp
    
    core/models/readerror/hiseq_indel_error_model.hpp
    core/models/readerror/hiseq_indel_error_model.cpp
    core/models/readerror/indel_error_model.hpp
    core/models/readerror/indel_error_model.cpp
    core/models/readerror/snv_error_model.hpp
    core/models/readerror/snv_error_model.cpp
    
    core/models/utils/kmer_mapper.hpp
    core/models/utils/kmer_mapper.cpp
    
    core/tools/coretools.hpp
    
    core/tools/haplotypegenerator/genome_walker.hpp
    core/tools/haplotypegenerator/genome_walker.cpp
    core/tools/haplotypegenerator/haplotype_generator.hpp
    core/tools/haplotypegenerator/haplotype_generator.cpp
    core/tools/haplotypegenerator/haplotype_tree.hpp
    core/tools/haplotypegenerator/haplotype_tree.cpp
    
    core/tools/phaser/phaser.hpp
    core/tools/phaser/phaser.cpp
    
    core/tools/variantgenerator/cigar_scanner.hpp
    core/tools/variantgenerator/cigar_scanner.cpp
    core/tools/variantgenerator/downloader.hpp
    core/tools/variantgenerator/downloader.cpp
    core/tools/variantgenerator/local_reassembler.hpp
    core/tools/variantgenerator/local_reassembler.cpp
    core/tools/variantgenerator/randomiser.hpp
    core/tools/variantgenerator/randomiser.cpp
    core/tools/variantgenerator/variant_generator.hpp
    core/tools/variantgenerator/variant_generator.cpp
    core/tools/variantgenerator/vcf_extractor.hpp
    core/tools/variantgenerator/vcf_extractor.cpp
    
    core/tools/variantgenerator/utils/assembler.hpp
    core/tools/variantgenerator/utils/assembler.cpp
    core/tools/variantgenerator/utils/global_aligner.hpp
    core/tools/variantgenerator/utils/global_aligner.cpp
    
    core/types/allele.hpp
    core/types/allele.cpp
    core/types/cancer_genotype.hpp
    core/types/cancer_genotype.cpp
    core/types/genotype.hpp
    core/types/genotype.cpp
    core/types/haplotype.hpp
    core/types/haplotype.cpp
    core/types/pedigree.hpp
    core/types/pedigree.cpp
    core/types/trio.hpp
    core/types/trio.cpp
    core/types/variant.hpp
    core/types/variant.cpp
    
    core/octopus.hpp
    core/octopus.cpp
)

set(OCTOPUS_SOURCES
    ${CONFIG_SOURCES}
    ${CONCEPTS_SOURCES}
    ${BASICS_SOURCES}
    ${CONTAINERS_SOURCES}
    ${LOGGING_SOURCES}
    ${IO_SOURCES}
    ${READPIPE_SOURCES}
    ${UTILS_SOURCES}
    ${CORE_SOURCES}
)

set(INCLUDE_SOURCES
    ${octopus_SOURCE_DIR}/lib/bioio.hpp
    ${octopus_SOURCE_DIR}/lib/tandem/tandem.hpp
)

add_executable(octopus main.cpp ${OCTOPUS_SOURCES} ${INCLUDE_SOURCES})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

target_include_directories(octopus PUBLIC ${octopus_SOURCE_DIR}/lib ${octopus_SOURCE_DIR}/src ${octopus_SOURCE_DIR}/test/benchmarks)

target_link_libraries(octopus tandem)

# Required for log
set(REQUIRED_BOOST_LIBRARIES
    system
    system-mt
    filesystem
    filesystem-mt
    program_options
    date_time
    date_time-mt
    log_setup-mt
    log
    log-mt
    iostreams
    timer
    thread-mt
)
add_definitions(-DBOOST_LOG_DYN_LINK)
find_package (Boost 1.61 REQUIRED COMPONENTS ${REQUIRED_BOOST_LIBRARIES} REQUIRED)
if (Boost_FOUND)
    target_include_directories (octopus PUBLIC ${Boost_INCLUDE_DIR})
    target_link_libraries (octopus ${Boost_LIBRARIES})
endif (Boost_FOUND)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/build/cmake/modules/")

find_package (HTSlib 1.3.1 REQUIRED)
if (HTSlib_FOUND)
    target_include_directories (octopus PUBLIC ${HTSlib_INCLUDE_DIRS})
    target_link_libraries (octopus ${HTSlib_LIBRARIES})
endif (HTSlib_FOUND)

set_target_properties(octopus PROPERTIES INTERPROCEDURAL_OPTIMIZATION True)

install(TARGETS octopus DESTINATION ${octopus_SOURCE_DIR}/bin)