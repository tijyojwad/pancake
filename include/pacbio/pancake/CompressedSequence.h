// Author: Ivan Sovic

#ifndef PANCAKE_COMPRESSED_SEQUENCE_H
#define PANCAKE_COMPRESSED_SEQUENCE_H

#include <pacbio/pancake/Range.h>
#include <pacbio/pancake/Twobit.h>
#include <cstdint>
#include <memory>
#include <vector>

namespace PacBio {
namespace Pancake {

class CompressedSequence
{
public:
    CompressedSequence();
    CompressedSequence(const std::string& bases);
    ~CompressedSequence();

    void SetFromBases(const std::string& bases);
    const std::vector<uint8_t>& GetTwobit() const { return twobit_; }
    const std::vector<PacBio::Pancake::Range>& GetRanges() const { return ranges_; }
    int64_t GetNumUncompressedBases() const { return numUncompressedBases_; }
    int64_t GetNumCompressedBases() const { return numCompressedBases_; }

private:
    std::vector<uint8_t> twobit_;
    std::vector<PacBio::Pancake::Range> ranges_;
    int64_t numUncompressedBases_ = 0;
    int64_t numCompressedBases_ = 0;
};

}  // namespace Pancake
}  // namespace PacBio

#endif  // PANCAKE_COMPRESSED_SEQUENCE_H