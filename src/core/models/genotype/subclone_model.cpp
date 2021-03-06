// Copyright (c) 2015-2019 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#include "subclone_model.hpp"

#include <algorithm>
#include <numeric>
#include <iterator>
#include <functional>
#include <cstddef>
#include <cmath>
#include <cassert>

#include "logging/logging.hpp"
#include "utils/maths.hpp"
#include "utils/concat.hpp"
#include "constant_mixture_genotype_likelihood_model.hpp"
#include "individual_model.hpp"
#include "variable_mixture_genotype_likelihood_model.hpp"

namespace octopus { namespace model {

namespace detail {

LogProbabilityVector log_uniform_dist(const std::size_t n)
{
    return LogProbabilityVector(n, -std::log(static_cast<double>(n)));
}

auto make_point_seed(const std::size_t num_genotypes, const std::size_t idx, const double p = 0.9999)
{
    LogProbabilityVector result(num_genotypes, num_genotypes > 1 ? std::log((1 - p) / (num_genotypes - 1)) : 0);
    if (num_genotypes > 1) result[idx] = std::log(p);
    return result;
}

void make_point_seeds(const std::size_t num_genotypes, const std::vector<std::size_t>& indices,
                      std::vector<LogProbabilityVector>& result, const double p = 0.9999)
{
    result.reserve(result.size() + indices.size());
    std::transform(std::cbegin(indices), std::cend(indices), std::back_inserter(result),
                   [=] (auto idx) { return make_point_seed(num_genotypes, idx, p); });
}

auto make_multipoint_seed(const std::size_t num_genotypes, const std::vector<std::size_t>& indices,
                          double p = 0.9999999)
{
    assert(num_genotypes >= indices.size());
    LogProbabilityVector result(num_genotypes, num_genotypes > 1 ? std::log((1 - p) / (num_genotypes - 1)) : 0);
    if (num_genotypes > 1) {
        p /= indices.size();
        for (auto idx : indices) result[idx] = std::log(p);
    }
    return result;
}

auto make_range_seed(const std::size_t num_genotypes, const std::size_t begin, const std::size_t n,
                     const double p = 0.9999999)
{
    LogProbabilityVector result(num_genotypes, std::log((1 - p) / (num_genotypes - n)));
    std::fill_n(std::next(std::begin(result), begin), n, std::log(p / n));
    return result;
}

auto make_range_seed(const std::vector<CancerGenotype<Haplotype>>& genotypes, const Genotype<Haplotype>& germline,
                     const double p = 0.9999999)
{
    auto itr1 = std::find_if(std::cbegin(genotypes), std::cend(genotypes), [&] (const auto& g) { return g.germline() == germline; });
    auto itr2 = std::find_if_not(std::next(itr1), std::cend(genotypes), [&] (const auto& g) { return g.germline() == germline; });
    return make_range_seed(genotypes.size(), std::distance(std::cbegin(genotypes), itr1), std::distance(itr1, itr2));
}

namespace debug {

template <typename S>
void print_top(S&& stream, const std::vector<CancerGenotype<Haplotype>>& genotypes,
               const LogProbabilityVector& probs, std::size_t n)
{
    assert(probs.size() == genotypes.size());
    n = std::min(n, genotypes.size());
    std::vector<std::pair<CancerGenotype<Haplotype>, double> > pairs {};
    pairs.reserve(genotypes.size());
    std::transform(std::cbegin(genotypes), std::cend(genotypes), std::cbegin(probs), std::back_inserter(pairs),
                   [] (const auto& g, auto p) { return std::make_pair(g, p); });
    const auto mth = std::next(std::begin(pairs), n);
    std::partial_sort(std::begin(pairs), mth, std::end(pairs),
                      [] (const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });
    std::for_each(std::begin(pairs), mth, [&] (const auto& p) {
        octopus::debug::print_variant_alleles(stream, p.first);
        stream << " " << p.second << '\n';
    });
}

void print_top(const std::vector<CancerGenotype<Haplotype>>& genotypes,
               const LogProbabilityVector& probs, std::size_t n = 10)
{
    print_top(std::cout, genotypes, probs, n);
}

} // namespace debug

void add_to(const LogProbabilityVector& other, LogProbabilityVector& result)
{
    std::transform(std::cbegin(other), std::cend(other), std::cbegin(result), std::begin(result),
                   [] (auto a, auto b) { return a + b; });
}

auto generate_exhaustive_seeds(const std::size_t n)
{
    std::vector<LogProbabilityVector> result {};
    result.reserve(n);
    for (unsigned i {0}; i < n; ++i) {
        result.push_back(make_point_seed(n, i));
    }
    return result;
}

std::vector<LogProbabilityVector>
compute_genotype_likelihoods_with_fixed_mixture_model(const std::vector<SampleName>& samples,
                                                      const std::vector<CancerGenotype<Haplotype>>& genotypes,
                                                      const HaplotypeLikelihoodArray& haplotype_log_likelihoods,
                                                      const SomaticSubcloneModel::Priors::GenotypeMixturesDirichletAlphaMap& priors,
                                                      boost::optional<IndexData<CancerGenotypeIndex>> index_data)
{
    VariableMixtureGenotypeLikelihoodModel model {haplotype_log_likelihoods};
    const bool use_genotype_indices {index_data && index_data->haplotypes};
    std::vector<LogProbabilityVector> result {};
    result.reserve(samples.size());
    for (const auto& sample : samples) {
        const auto& sample_priors = priors.at(sample);
        model.set_mixtures(maths::dirichlet_expectation(sample_priors));
        model.cache().prime(sample);
        if (use_genotype_indices) {
            model.prime(*index_data->haplotypes);
            result.push_back(evaluate(index_data->genotype_indices, model));
            model.unprime();
        } else {
            result.push_back(evaluate(genotypes, model));
        }
    }
    return result;
}

auto evaluate(const CancerGenotype<Haplotype>& genotype, const ConstantMixtureGenotypeLikelihoodModel& model)
{
    return model.evaluate(demote(genotype));
}

auto evaluate(const CancerGenotypeIndex& genotype, const ConstantMixtureGenotypeLikelihoodModel& model)
{
    return model.evaluate(concat(genotype.germline, genotype.somatic));
}

template <typename G>
LogProbabilityVector evaluate(const std::vector<G>& genotypes, const ConstantMixtureGenotypeLikelihoodModel& model)
{
    LogProbabilityVector result(genotypes.size());
    std::transform(std::cbegin(genotypes), std::cend(genotypes), std::begin(result),
                   [&] (const auto& genotype) { return evaluate(genotype, model); });
    return result;
}

std::vector<LogProbabilityVector>
compute_genotype_likelihoods_with_germline_model(const std::vector<SampleName>& samples,
                                                 const std::vector<CancerGenotype<Haplotype>>& genotypes,
                                                 const HaplotypeLikelihoodArray& haplotype_log_likelihoods,
                                                 boost::optional<IndexData<CancerGenotypeIndex>> index_data)
{
    ConstantMixtureGenotypeLikelihoodModel model {haplotype_log_likelihoods};
    const bool use_genotype_indices {index_data && index_data->haplotypes};
    std::vector<LogProbabilityVector> result {};
    result.reserve(samples.size());
    for (const auto& sample : samples) {
        model.cache().prime(sample);
        if (use_genotype_indices) {
            model.prime(*index_data->haplotypes);
            result.push_back(evaluate(index_data->genotype_indices, model));
            model.unprime();
        } else {
            result.push_back(evaluate(genotypes, model));
        }
    }
    return result;
}

LogProbabilityVector evaluate_germlines(const std::vector<CancerGenotype<Haplotype>>& genotypes, const ConstantMixtureGenotypeLikelihoodModel& model)
{
    std::unordered_map<Genotype<Haplotype>, ConstantMixtureGenotypeLikelihoodModel::LogProbability> cache {};
    cache.reserve(genotypes.size());
    LogProbabilityVector result(genotypes.size());
    std::transform(std::cbegin(genotypes), std::cend(genotypes), std::begin(result),
                   [&] (const auto& genotype) {
        const auto cache_itr = cache.find(genotype.germline());
        if (cache_itr != std::cend(cache)) return cache_itr->second;
        const auto result = model.evaluate(genotype.germline());
        cache.emplace(genotype.germline(), result);
        return result;
    });
    return result;
}

struct GenotypeIndexHash
{
    std::size_t operator()(const GenotypeIndex genotype) const
    {
        return boost::hash_range(std::cbegin(genotype), std::cend(genotype));
    }
};

LogProbabilityVector evaluate_germlines(const std::vector<CancerGenotypeIndex>& genotypes, const ConstantMixtureGenotypeLikelihoodModel& model)
{
    std::unordered_map<GenotypeIndex, ConstantMixtureGenotypeLikelihoodModel::LogProbability, GenotypeIndexHash> cache {};
    cache.reserve(genotypes.size());
    LogProbabilityVector result(genotypes.size());
    std::transform(std::cbegin(genotypes), std::cend(genotypes), std::begin(result),
                   [&] (const auto& genotype) {
                       const auto cache_itr = cache.find(genotype.germline);
                       if (cache_itr != std::cend(cache)) return cache_itr->second;
                       const auto result = model.evaluate(genotype.germline);
                       cache.emplace(genotype.germline, result);
                       return result;
                   });
    return result;
}

std::vector<LogProbabilityVector>
compute_germline_genotype_likelihoods_with_germline_model(const std::vector<SampleName>& samples,
                                                          const std::vector<CancerGenotype<Haplotype>>& genotypes,
                                                          const HaplotypeLikelihoodArray& haplotype_log_likelihoods,
                                                          boost::optional<IndexData<CancerGenotypeIndex>> index_data)
{
    ConstantMixtureGenotypeLikelihoodModel model {haplotype_log_likelihoods};
    const bool use_genotype_indices {index_data && index_data->haplotypes};
    std::vector<LogProbabilityVector> result {};
    result.reserve(samples.size());
    for (const auto& sample : samples) {
        model.cache().prime(sample);
        if (use_genotype_indices) {
            model.prime(*index_data->haplotypes);
            result.push_back(evaluate_germlines(index_data->genotype_indices, model));
            model.unprime();
        } else {
            result.push_back(evaluate_germlines(genotypes, model));
        }
    }
    return result;
}

auto add_all(const std::vector<ProbabilityVector>& likelihoods)
{
    assert(!likelihoods.empty());
    ProbabilityVector result(likelihoods.front().size());
    for (const auto& probabilities : likelihoods) {
        add_to(probabilities, result);
    }
    return result;
}

auto add_all_and_normalise(const std::vector<LogProbabilityVector>& log_likelihoods)
{
    auto result = add_all(log_likelihoods);
    maths::normalise_logs(result);
    return result;
}

auto add(const LogProbabilityVector& lhs, const LogProbabilityVector& rhs)
{
    auto result = lhs;
    add_to(rhs, result);
    return result;
}

auto add_and_normalise(const LogProbabilityVector& lhs, const LogProbabilityVector& rhs)
{
    auto result = add(lhs, rhs);
    maths::normalise_logs(result);
    return result;
}

std::vector<LogProbabilityVector>
generate_seeds(const std::vector<SampleName>& samples,
               const std::vector<Genotype<Haplotype>>& genotypes,
               const LogProbabilityVector& genotype_log_priors,
               const HaplotypeLikelihoodArray& haplotype_log_likelihoods,
               const SubcloneModel::Priors& priors,
               std::size_t max_seeds,
               std::vector<LogProbabilityVector> hints,
               boost::optional<IndexData<GenotypeIndex>> index_data)
{
    if (genotypes.size() <= max_seeds) {
        return generate_exhaustive_seeds(genotypes.size());
    }
    auto result = std::move(hints);
    if (result.size() > max_seeds) return result;
    max_seeds -= result.size();
    result.reserve(max_seeds);
    result.push_back(genotype_log_priors);
    IndividualModel germline_model {priors.genotype_prior_model};
    for (const auto& sample : samples) {
        haplotype_log_likelihoods.prime(sample);
        IndividualModel::InferredLatents latents;
        if (index_data) {
            latents = germline_model.evaluate(genotypes, index_data->genotype_indices, haplotype_log_likelihoods);
        } else {
            latents = germline_model.evaluate(genotypes, haplotype_log_likelihoods);
        }
        result.push_back(latents.posteriors.genotype_log_probabilities);
    }
    return result;
}

std::vector<LogProbabilityVector>
generate_seeds(const std::vector<SampleName>& samples,
               const std::vector<CancerGenotype<Haplotype>>& genotypes,
               const LogProbabilityVector& genotype_log_priors,
               const HaplotypeLikelihoodArray& haplotype_log_likelihoods,
               const SomaticSubcloneModel::Priors& priors,
               std::size_t max_seeds,
               std::vector<LogProbabilityVector> hints,
               boost::optional<IndexData<CancerGenotypeIndex>> index_data)
{
    if (genotypes.size() <= max_seeds) {
        return generate_exhaustive_seeds(genotypes.size());
    }
    auto result = std::move(hints);
    if (result.size() > max_seeds) return result;
    max_seeds -= result.size();
    result.reserve(max_seeds);
    if (max_seeds == 0) return result;
    auto sample_prior_mixture_likelihoods = compute_genotype_likelihoods_with_fixed_mixture_model(samples, genotypes, haplotype_log_likelihoods, priors.alphas, index_data);
    auto prior_mixture_likelihoods = add_all_and_normalise(sample_prior_mixture_likelihoods);
    auto prior_mixture_posteriors = add_and_normalise(genotype_log_priors, prior_mixture_likelihoods);
    result.push_back(prior_mixture_posteriors); // 1
    --max_seeds;
    if (max_seeds == 0) return result;
    auto sample_normal_likelihoods = compute_genotype_likelihoods_with_germline_model(samples, genotypes, haplotype_log_likelihoods, index_data);
    auto normal_likelihoods = add_all_and_normalise(sample_prior_mixture_likelihoods);
    auto normal_posteriors = add_and_normalise(genotype_log_priors, normal_likelihoods);
    result.push_back(normal_posteriors); // 2
    --max_seeds;
    if (max_seeds == 0) return result;
    result.push_back(prior_mixture_likelihoods); // 3
    if (max_seeds == 0) return result;
    result.push_back(normal_likelihoods); // 4
    if (max_seeds == 0) return result;
    auto combined_model_likelihoods = add_and_normalise(prior_mixture_likelihoods, normal_likelihoods);
    auto combined_model_posteriors = add_and_normalise(genotype_log_priors, combined_model_likelihoods);
    result.push_back(combined_model_posteriors); // 5
    if (max_seeds == 0) return result;
    result.push_back(combined_model_likelihoods); // 6
    if (max_seeds == 0) return result;
    auto sample_germline_likelihoods = compute_germline_genotype_likelihoods_with_germline_model(samples, genotypes, haplotype_log_likelihoods, index_data);
    result.push_back(add_all_and_normalise(sample_germline_likelihoods)); // 7
    --max_seeds;
    if (max_seeds == 0) return result;
//    if (samples.size() > 1) {
//        for (auto& log_likelihoods : sample_prior_mixture_likelihoods) {
//            result.push_back(add_and_normalise(genotype_log_priors, log_likelihoods));
//            result.push_back(log_likelihoods);
//            if (max_seeds == 0) return result;
//            maths::normalise_logs(log_likelihoods);
//            result.push_back(std::move(log_likelihoods));
//            if (max_seeds == 0) return result;
//        }
//        for (auto& log_likelihoods : sample_normal_likelihoods) {
//            result.push_back(add_and_normalise(genotype_log_priors, log_likelihoods));
//            result.push_back(log_likelihoods);
//            if (max_seeds == 0) return result;
//            maths::normalise_logs(log_likelihoods);
//            result.push_back(std::move(log_likelihoods));
//            if (max_seeds == 0) return result;
//        }
//    }
    result.push_back(genotype_log_priors); // 8
    maths::normalise_logs(result.back());
    if (max_seeds == 0) return result;
    std::vector<std::pair<double, std::size_t>> ranked_prior_mixture_posteriors {};
    ranked_prior_mixture_posteriors.reserve(genotypes.size());
    for (std::size_t i {0}; i < genotypes.size(); ++i) {
        ranked_prior_mixture_posteriors.push_back({prior_mixture_posteriors[i], i});
    }
    const auto nth_ranked_prior_mixture_posteriors_itr = std::next(std::begin(ranked_prior_mixture_posteriors), max_seeds);
    std::partial_sort(std::begin(ranked_prior_mixture_posteriors),
                      nth_ranked_prior_mixture_posteriors_itr,
                      std::end(ranked_prior_mixture_posteriors), std::greater<> {});
    std::transform(std::begin(ranked_prior_mixture_posteriors), nth_ranked_prior_mixture_posteriors_itr,
                   std::back_inserter(result), [&] (const auto& p) { return make_point_seed(genotypes.size(), p.second); });
    return result;
}

} // namespace

} // namespace model
} // namespace octopus
