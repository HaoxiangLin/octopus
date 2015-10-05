//
//  variant_caller_factory.hpp
//  Octopus
//
//  Created by Daniel Cooke on 03/10/2015.
//  Copyright © 2015 Oxford University. All rights reserved.
//

#ifndef variant_caller_factory_hpp
#define variant_caller_factory_hpp

#include <unordered_map>
#include <string>
#include <memory> // std::unique_ptr
#include <stdexcept>

#include "variant_caller.hpp"
#include "population_caller.hpp"
#include "cancer_caller.hpp"

namespace Octopus {

inline std::unique_ptr<VariantCaller>
make_variant_caller(const std::string& model, ReferenceGenome& reference,
                    CandidateVariantGenerator& candidate_generator,
                    VariantCaller::RefCall refcalls, double min_posterior, unsigned ploidy)
{
    std::unordered_map<std::string, std::function<std::unique_ptr<VariantCaller>()>> model_map {
        {"population", [&] () {
            return std::make_unique<PopulationVariantCaller>(reference, candidate_generator,
                                                             refcalls, min_posterior, ploidy);
        }},
        {"cancer",     [&] () {
            return std::make_unique<CancerVariantCaller>(reference, candidate_generator,
                                                         refcalls, min_posterior);
        }}
        };
    
    if (model_map.count(model) == 0) {
        throw std::runtime_error {"unknown model " + model};
    }
    
    return model_map[model]();
}

} // namespace Octopus

#endif /* variant_caller_factory_hpp */
