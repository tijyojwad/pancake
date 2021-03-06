// Authors: Ivan Sovic

#include <pacbio/alignment/AlignmentTools.h>
#include <pacbio/pancake/AlignerSES2.h>
#include <pbcopper/third-party/edlib.h>
#include <pacbio/alignment/Ses2AlignBanded.hpp>

namespace PacBio {
namespace Pancake {

std::shared_ptr<AlignerBase> CreateAlignerSES2(const AlignmentParameters& opt)
{
    return std::shared_ptr<AlignerBase>(new AlignerSES2(opt));
}

AlignerSES2::AlignerSES2(const AlignmentParameters& opt)
    : opt_(opt), sesScratch_{std::make_shared<Pancake::Alignment::SESScratchSpace>()}
{
}

AlignerSES2::~AlignerSES2() {}

AlignmentResult AlignerSES2::Global(const char* qseq, int64_t qlen, const char* tseq, int64_t tlen)
{
    const double alignMaxDiff = 1.0;  // 0.30;
    // const double alignBandwidth = 0.30;
    const int32_t maxDiffs = std::max(10, static_cast<int32_t>(qlen * alignMaxDiff));
    // const int32_t bandwidth =
    //     std::max(10, static_cast<int32_t>(std::min(tlen, qlen) * alignBandwidth));

    // Compute the actual bandwidth. If this was a long join, we need to allow more room.
    // const int32_t bw = opt_.alignBandwidth;  // * 1.5 + 1.;
    // const int32_t longestSpan = std::max(qlen, tlen);
    // const int32_t spanDiff = std::abs(tlen - qlen);
    // const int32_t actualBandwidth = ((static_cast<double>(spanDiff) / static_cast<double>(longestSpan)) > 0.05) ? longestSpan : bw;
    const int32_t actualBandwidth = qlen + tlen;

    auto aln = Alignment::SES2AlignBanded<Alignment::SESAlignMode::Global,
                                          Alignment::SESTrimmingMode::Disabled,
                                          Alignment::SESTracebackMode::Enabled>(
        qseq, qlen, tseq, tlen, maxDiffs, actualBandwidth, sesScratch_);

    Data::Cigar cigar = NormalizeCigar(qseq, qlen, tseq, tlen, aln.cigar);

    AlignmentResult ret;
    ret.cigar = std::move(cigar);
    ret.score = ScoreCigarAlignment(ret.cigar, opt_.matchScore, opt_.mismatchPenalty, opt_.gapOpen1,
                                    opt_.gapExtend1);
    ret.valid = aln.valid;
    ret.maxScore = ret.score;
    ret.zdropped = false;
    ret.lastQueryPos = qlen;
    ret.lastTargetPos = tlen;
    ret.maxQueryPos = qlen;
    ret.maxTargetPos = tlen;

    return ret;
}

AlignmentResult AlignerSES2::Extend(const char* /*qseq*/, int64_t /*qlen*/, const char* /*tseq*/,
                                    int64_t /*tlen*/)
{
    AlignmentResult ret;
    ret.valid = false;
    ret.lastQueryPos = 0;
    ret.lastTargetPos = 0;
    return ret;
}

}  // namespace Pancake
}  // namespace PacBio
