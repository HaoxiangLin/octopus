//
//  trio_caller.cpp
//  Octopus
//
//  Created by Daniel Cooke on 30/10/2015.
//  Copyright © 2015 Oxford University. All rights reserved.
//

#include "trio_caller.hpp"

#include <utility> // std::move

#include "vcf_record.hpp"

namespace Octopus
{
    // public methods
    
    TrioVariantCaller::TrioVariantCaller(ReferenceGenome& reference, CandidateVariantGenerator& candidate_generator,
                                         unsigned ploidy, SampleIdType mother, SampleIdType father,
                                         double min_variant_posterior)
    :
    VariantCaller {reference, candidate_generator, RefCallType::None},
    phaser_ {reference, 1000, 0},
    ploidy_ {ploidy},
    mother_ {std::move(mother)},
    father_ {std::move(father)},
    min_variant_posterior_ {min_variant_posterior}
    {}
    
    // private methods
    
    std::string TrioVariantCaller::do_get_details() const
    {
        return "trio caller. mother = " + mother_ + ", father = " + father_;
    }
    
    std::vector<VcfRecord>
    TrioVariantCaller::call_variants(const GenomicRegion& region,
                                     const std::vector<Variant>& candidates,
                                     const ReadMap& reads)
    {
        std::vector<VcfRecord> result {};
        return result;
    }

} // namespace Octopus