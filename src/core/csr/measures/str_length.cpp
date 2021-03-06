// Copyright (c) 2015-2019 Daniel Cooke
// Use of this source code is governed by the MIT license that can be found in the LICENSE file.

#include "str_length.hpp"

#include <iterator>
#include <algorithm>

#include <boost/variant.hpp>

#include "basics/tandem_repeat.hpp"
#include "io/variant/vcf_record.hpp"
#include "utils/repeat_finder.hpp"
#include "../facets/reference_context.hpp"
#include "../facets/samples.hpp"
#include "../facets/alleles.hpp"

namespace octopus { namespace csr {

const std::string STRLength::name_ = "STRL";

std::unique_ptr<Measure> STRLength::do_clone() const
{
    return std::make_unique<STRLength>(*this);
}

namespace {

template <typename Region>
Region safe_expand_lhs(const Region& region, typename Region::Distance n) noexcept
{
    return expand_lhs(region, std::min(n, static_cast<decltype(n)>(region.begin())));
}

struct RepeatContextLess
{
    bool operator()(const TandemRepeat& lhs, const TandemRepeat& rhs) const noexcept
    {
        // Expand to discount possible reference pad
        const auto expanded_lhs_region = safe_expand_lhs(mapped_region(lhs), 1);
        const auto expanded_rhs_region = safe_expand_lhs(mapped_region(rhs), 1);
        if (overlap_size(expanded_lhs_region, call_) != overlap_size(expanded_rhs_region, call_)) {
            return overlap_size(expanded_lhs_region, call_) < overlap_size(expanded_rhs_region, call_);
        }
        return ends_equal(lhs, rhs) ? begins_before(rhs, lhs) : ends_before(lhs, rhs);
    }
    RepeatContextLess(const VcfRecord& call) : call_ {call} {};
private:
    const VcfRecord& call_;
};

bool could_contain(const TandemRepeat& repeat, const VcfRecord& call)
{
    return contains(expand(mapped_region(repeat), 1), call);
}

boost::optional<TandemRepeat> find_repeat_context(const VcfRecord& call, const std::vector<Allele>& alleles, const Haplotype& reference)
{
    const auto repeats = find_exact_tandem_repeats(reference.sequence(), reference.mapped_region(), 1, 20);
    const auto overlapping_repeats = overlap_range(repeats, expand(mapped_region(call), 1));
    boost::optional<TandemRepeat> result {};
    if (!empty(overlapping_repeats)) {
        for (const auto& repeat : repeats) {
            if (could_contain(repeat, call)  && has_overlapped(alleles, repeat)) {
                if (result) {
                    result = std::max(repeat, *result, RepeatContextLess {call});
                } else {
                    result = repeat;
                }
            }
        }
        if (!result) {
            result = *max_overlapped(repeats, call);
            if (!has_overlapped(alleles, *result)) {
                result = boost::none;
            }
        }
    }
    return result;
}

} // namespace

Measure::ResultType STRLength::do_evaluate(const VcfRecord& call, const FacetMap& facets) const
{
    int result {0};
    const auto& reference = get_value<ReferenceContext>(facets.at("ReferenceContext"));
    const auto& samples = get_value<Samples>(facets.at("Samples"));
    const auto alleles = copy_unique_overlapped(get_value<Alleles>(facets.at("Alleles")), call, samples);
    const auto repeat_context = find_repeat_context(call, alleles, reference);
    if (repeat_context) result = region_size(*repeat_context);
    return result;
}

Measure::ResultCardinality STRLength::do_cardinality() const noexcept
{
    return ResultCardinality::one;
}

const std::string& STRLength::do_name() const
{
    return name_;
}

std::string STRLength::do_describe() const
{
    return "Length of overlapping STR";
}

std::vector<std::string> STRLength::do_requirements() const
{
    return {"ReferenceContext", "Samples", "Alleles"};
}

} // namespace csr
} // namespace octopus
