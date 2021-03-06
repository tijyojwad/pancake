// Authors: Ivan Sovic

#include <pacbio/pancake/SeedDBReader.h>
#include <pacbio/pancake/SeedDBReaderCached.h>
#include <sstream>

namespace PacBio {
namespace Pancake {

SeedDBReaderCached::SeedDBReaderCached(
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache>& seedDBCache, int32_t blockId)
    : seedDBIndexCache_(seedDBCache), blockId_(blockId)
{
    // Sanity check.
    if (seedDBIndexCache_->fileLines.empty())
        throw std::runtime_error("There are no file specifications in the input index file.");
    if (seedDBIndexCache_->seedLines.empty())
        throw std::runtime_error("There are no sequences in the input index file.");
    if (seedDBIndexCache_->blockLines.empty())
        throw std::runtime_error("There are no blocks in the input index file.");

    // Create a SeedDB reader.
    PacBio::Pancake::SeedDBReader reader(seedDBCache);

    // Fetch the block of data.
    reader.GetBlock(records_, blockId);

    for (int32_t i = 0; i < static_cast<int32_t>(records_.size()); ++i) {
        headerToOrdinalId_[records_[i].Name()] = i;
        seqIdToOrdinalId_[records_[i].Id()] = i;
    }
}

SeedDBReaderCached::~SeedDBReaderCached() = default;

const SequenceSeeds& SeedDBReaderCached::GetSeedsForSequence(int32_t seqId) const
{
    auto it = seqIdToOrdinalId_.find(seqId);
    if (it == seqIdToOrdinalId_.end()) {
        std::ostringstream oss;
        oss << "(SeedDBReaderCached) Invalid seqId, not found in block " << blockId_
            << ". seqId = " << seqId << ", records_.size() = " << records_.size();
        throw std::runtime_error(oss.str());
    }
    int32_t ordinalId = it->second;
    return records_[ordinalId];
}

const SequenceSeeds& SeedDBReaderCached::GetSeedsForSequence(const std::string& seqName) const
{
    auto it = headerToOrdinalId_.find(seqName);
    if (it == headerToOrdinalId_.end()) {
        std::ostringstream oss;
        oss << "(SeedDBReaderCached) Invalid seqName, not found in block " << blockId_
            << ". seqName = '" << seqName << ".";
        throw std::runtime_error(oss.str());
    }
    int32_t ordinalId = it->second;
    return records_[ordinalId];
}

}  // namespace Pancake
}  // namespace PacBio
