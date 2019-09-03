#include <string>
#include <vector>

#include <seqan3/alphabet/quality/phred42.hpp>
#include <seqan3/range/view/trim.hpp>
#include <seqan3/range/view/to_char.hpp>

int main()
{
    std::vector<seqan3::phred42> vec{seqan3::phred42{40}, seqan3::phred42{40}, seqan3::phred42{30},
                                     seqan3::phred42{20}, seqan3::phred42{10}};

    // trim by phred_value
    auto v1 = vec | seqan3::view::trim(20u);                                // == ['I','I','?','5']

    // trim by quality character
    auto v2 = vec | seqan3::view::trim(seqan3::phred42{40});                // == ['I','I']

    // function syntax
    auto v3 = seqan3::view::trim(vec, 20u);                                 // == ['I','I','?','5']

    // combinability
    auto v4 = seqan3::view::trim(vec, 20u) | seqan3::view::to_char;         // == "II?5"
}
