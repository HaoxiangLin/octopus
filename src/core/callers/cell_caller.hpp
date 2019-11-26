// Copyright (c) 2015-2019 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#ifndef cell_caller_hpp
#define cell_caller_hpp

#include <vector>
#include <string>
#include <memory>
#include <functional>

#include <boost/optional.hpp>

#include "config/common.hpp"
#include "basics/phred.hpp"
#include "core/types/haplotype.hpp"
#include "core/types/genotype.hpp"
#include "core/models/mutation/coalescent_model.hpp"
#include "core/models/genotype/genotype_prior_model.hpp"
#include "core/models/genotype/single_cell_model.hpp"
#include "caller.hpp"

namespace octopus {

class GenomicRegion;
class ReadPipe;
class Variant;
class HaplotypeLikelihoodArray;
class VariantCall;

class CellCaller : public Caller
{
public:
    using Caller::CallTypeSet;
    
    struct Parameters
    {
        unsigned ploidy;
        boost::optional<CoalescentModel::Parameters> prior_model_params;
        Phred<double> min_variant_posterior, min_refcall_posterior;
        bool deduplicate_haplotypes_with_prior_model = false;
        unsigned max_clones;
        boost::optional<std::size_t> max_genotypes, max_joint_genotypes;
        double dropout_concentration;
        DeNovoModel::Parameters mutation_model_parameters;
        boost::optional<unsigned> max_vb_seeds = boost::none; // Use default if none
        std::vector<SampleName> normal_samples = {};
    };
    
    CellCaller() = delete;
    
    CellCaller(Caller::Components&& components,
               Caller::Parameters general_parameters,
               Parameters specific_parameters);
    
    CellCaller(const CellCaller&)            = delete;
    CellCaller& operator=(const CellCaller&) = delete;
    CellCaller(CellCaller&&)                 = delete;
    CellCaller& operator=(CellCaller&&)      = delete;
    
    ~CellCaller() = default;

private:
    class Latents;
    friend Latents;
    
    Parameters parameters_;
    
    std::string do_name() const override;
    CallTypeSet do_call_types() const override;
    unsigned do_min_callable_ploidy() const override;
    unsigned do_max_callable_ploidy() const override;
    
    std::size_t do_remove_duplicates(HaplotypeBlock& haplotypes) const override;
    
    std::unique_ptr<Caller::Latents>
    infer_latents(const HaplotypeBlock& haplotypes,
                  const HaplotypeLikelihoodArray& haplotype_likelihoods) const override;
    
    boost::optional<double>
    calculate_model_posterior(const HaplotypeBlock& haplotypes,
                              const HaplotypeLikelihoodArray& haplotype_likelihoods,
                              const Caller::Latents& latents) const override;
    
    boost::optional<double>
    calculate_model_posterior(const HaplotypeBlock& haplotypes,
                              const HaplotypeLikelihoodArray& haplotype_likelihoods,
                              const Latents& latents) const;
    
    std::vector<std::unique_ptr<VariantCall>>
    call_variants(const std::vector<Variant>& candidates, const Caller::Latents& latents) const override;
    
    std::vector<std::unique_ptr<VariantCall>>
    call_variants(const std::vector<Variant>& candidates, const Latents& latents) const;
    
    std::vector<std::unique_ptr<ReferenceCall>>
    call_reference(const std::vector<Allele>& alleles, const Caller::Latents& latents,
                   const ReadPileupMap& pileup) const override;
    
    std::vector<std::unique_ptr<ReferenceCall>>
    call_reference(const std::vector<Allele>& alleles, const Latents& latents,
                   const ReadPileupMap& pileup) const;
    
    std::unique_ptr<GenotypePriorModel> make_prior_model(const HaplotypeBlock& haplotypes) const;
};

class CellCaller::Latents : public Caller::Latents
{
public:
    using Caller::Latents::HaplotypeProbabilityMap;
    using Caller::Latents::GenotypeProbabilityMap;
    
    Latents() = delete;
    
    Latents(const CellCaller& caller,
            HaplotypeBlock haplotypes,
            std::vector<Genotype<Haplotype>> genotypes,
            std::vector<model::SingleCellModel::Inferences> inferences);
    
    std::shared_ptr<HaplotypeProbabilityMap> haplotype_posteriors() const noexcept override;
    std::shared_ptr<GenotypeProbabilityMap> genotype_posteriors() const noexcept override;

private:
    mutable std::shared_ptr<GenotypeProbabilityMap> genotype_posteriors_;
    mutable std::shared_ptr<HaplotypeProbabilityMap> haplotype_posteriors_;
    
    const CellCaller& caller_;
    HaplotypeBlock haplotypes_;
    std::vector<Genotype<Haplotype>> genotypes_;
    std::vector<model::SingleCellModel::Inferences> phylogeny_inferences_;
    std::vector<double> phylogeny_posteriors_;
    
    friend CellCaller;
};

} // namespace octopus

#endif
