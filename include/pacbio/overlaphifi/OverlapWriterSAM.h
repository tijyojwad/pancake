// Author: Ivan Sovic

#ifndef PANCAKE_OVERLAPHIFI_OVERLAP_WRITER_SAM_H
#define PANCAKE_OVERLAPHIFI_OVERLAP_WRITER_SAM_H

#include <pacbio/overlaphifi/OverlapWriterBase.h>
#include <pacbio/pancake/FastaSequenceId.h>
#include <pacbio/pancake/Overlap.h>
#include <pacbio/seqdb/SeqDBReaderCached.h>
#include <pacbio/seqdb/SeqDBReaderCachedBlock.h>
#include <pacbio/util/Util.h>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace PacBio {
namespace Pancake {

class OverlapWriterSAM : public OverlapWriterBase
{
public:
    OverlapWriterSAM(FILE* fpOut, bool writeIds, bool writeCigar);
    ~OverlapWriterSAM();

    void WriteHeader(const PacBio::Pancake::SeqDBReaderCached& targetSeqs) override;

    void WriteHeader(const PacBio::Pancake::SeqDBReaderCachedBlock& targetSeqs) override;

    void Write(const OverlapPtr& ovl, const PacBio::Pancake::SeqDBReaderCached& targetSeqs,
               const PacBio::Pancake::FastaSequenceId& querySeq) override;

    void Write(const OverlapPtr& ovl, const PacBio::Pancake::SeqDBReaderCachedBlock& targetSeqs,
               const PacBio::Pancake::FastaSequenceCached& querySeq) override;

private:
    std::string outFile_;
    FILE* fpOut_ = NULL;
    bool shouldClose_ = false;
    bool writeIds_ = false;
    bool writeCigar_ = false;
};

}  // namespace Pancake
}  // namespace PacBio

#endif  // PANCAKE_OVERLAPHIFI_OVERLAP_WRITER_IPA_OVL_H
