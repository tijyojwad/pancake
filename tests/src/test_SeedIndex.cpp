// Authors: Ivan Sovic

#include <PancakeTestData.h>
#include <gtest/gtest.h>
#include <pacbio/overlaphifi/SeedIndex.h>
#include <pacbio/seeddb/Seed.h>
#include <sstream>
#include <tuple>

TEST(SeedIndex, GetSeeds1)
{
    /*
     * Fetch the byte span of a single small block of 1 sequence.
     * It doesn't span more than 1 file or sequences.
    */

    int32_t seqId = 123;
    std::vector<__int128> inSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 5, false),
    };
    uint64_t inKey = 0;

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Expected results.
    std::vector<__int128> expected = inSeeds;

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    std::vector<__int128> results;
    int64_t n = si.GetSeeds(inKey, results);

    EXPECT_EQ(expected, results);
    EXPECT_EQ(static_cast<int64_t>(expected.size()), n);
}

TEST(SeedIndex, GetSeeds2)
{
    /*
     * Fetch the byte span of a single small block of 1 sequence.
     * It doesn't span more than 1 file or sequences.
    */

    int32_t seqId = 123;
    std::vector<__int128> inSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, seqId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 8, false),
    };
    uint64_t inKey = 5;

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Expected results.
    std::vector<__int128> expected = {
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 7, false),
    };

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    std::vector<__int128> results;
    int64_t n = si.GetSeeds(inKey, results);

    EXPECT_EQ(expected, results);
    EXPECT_EQ(static_cast<int64_t>(expected.size()), n);
}

TEST(SeedIndex, GetSeeds3NonexistentKey)
{
    /*
     * Fetch the byte span of a single small block of 1 sequence.
     * It doesn't span more than 1 file or sequences.
    */

    int32_t seqId = 123;
    std::vector<__int128> inSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, seqId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 8, false),
    };
    uint64_t inKey = 1024;

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Expected results.
    std::vector<__int128> expected = {};

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    std::vector<__int128> results;
    int64_t n = si.GetSeeds(inKey, results);

    EXPECT_EQ(expected, results);
    EXPECT_EQ(static_cast<int64_t>(expected.size()), n);
}

TEST(SeedIndex, CollectHitsEmptyQueryNonemptyTarget)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * This particular test builds the SeedIndex with a set of seeds from
     * an imaginary target.
     * It uses the same set of seeds for the input to CollectHits.
     * For every input seed, we should get all other positions in the target.
     *
     * Here, the target seeds contains actual seeds.
     * Query seed set is empty.
     * There should be no hits.
    */
    const int32_t targetId = 123;
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, targetId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 8, false),
    };
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = {};

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {};

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, 100);

    // Evaluate.
    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, CollectHitsNonemptyQueryEmptyTarget)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * This particular test builds the SeedIndex with a set of seeds from
     * an imaginary target.
     * It uses the same set of seeds for the input to CollectHits.
     * For every input seed, we should get all other positions in the target.
     *
     * Target seed set is empty.
     * Query seed set contains seeds which should not match anything.
     * There should be no hits.
    */
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {};
    const int32_t queryId = 123;
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, queryId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, queryId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, queryId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 8, false),
    };

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {};

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, 100);

    // Evaluate.
    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, CollectHitsNonemptyQueryNonemptyTargetNoHits)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * This particular test builds the SeedIndex with a set of seeds from
     * an imaginary target.
     * It uses the same set of seeds for the input to CollectHits.
     * For every input seed, we should get all other positions in the target.
     *
     * Target seed set not empty.
     * Query seed set contains seeds which should not match anything in the target set.
     * There should be no hits.
    */
    const int32_t targetId = 123;
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, targetId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 5, false),
    };
    const int32_t queryId = 0;
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 7, false),
    };

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {};

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, 100);

    // Evaluate.
    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, CollectHitsPerfectMatch)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * This particular test builds the SeedIndex with a set of seeds from
     * an imaginary target.
     * It uses the same set of seeds for the input to CollectHits.
     * For every input seed, we should get all other positions in the target.
    */
    const int32_t targetId = 123;
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, targetId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 8, false),
    };
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = targetSeeds;

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {
        // 0
        {targetId, 0, 0, 0, 0},
        {targetId, 0, 5, 0, 0},
        // 123
        {targetId, 0, 1, 0, 1},
        {targetId, 0, 6, 0, 1},
        {targetId, 0, 8, 0, 1},
        /// 5
        {targetId, 0, 2, 0, 2},
        {targetId, 0, 4, 0, 2},
        {targetId, 0, 7, 0, 2},
        /// 7
        {targetId, 0, 3, 0, 3},
        // 5
        {targetId, 0, 2, 0, 4},
        {targetId, 0, 4, 0, 4},
        {targetId, 0, 7, 0, 4},
        // 0
        {targetId, 0, 0, 0, 5},
        {targetId, 0, 5, 0, 5},
        // 123
        {targetId, 0, 1, 0, 6},
        {targetId, 0, 6, 0, 6},
        {targetId, 0, 8, 0, 6},
        // 5
        {targetId, 0, 2, 0, 7},
        {targetId, 0, 4, 0, 7},
        {targetId, 0, 7, 0, 7},
        // 123
        {targetId, 0, 1, 0, 8},
        {targetId, 0, 6, 0, 8},
        {targetId, 0, 8, 0, 8},
    };

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, 100);

    // Evaluate.
    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, CollectHitsFrequencyThreshold)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * Same as before, but test frequency cutoff.
     * Do not fetch hith with frequency > 2.
    */
    const int32_t targetId = 123;
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, targetId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 8, false),
    };
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = targetSeeds;
    int32_t freqCutoff = 2;

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {
        // 0
        {targetId, 0, 0, 0, 0},
        {targetId, 0, 5, 0, 0},
        /// 7
        {targetId, 0, 3, 0, 3},
        // 0
        {targetId, 0, 0, 0, 5},
        {targetId, 0, 5, 0, 5},
    };

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, freqCutoff);

    // Evaluate.
    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, CollectHitsReverseStrand)
{
    /*
     * Tests collecting hits for a given vector of query keys and a maximum
     * allowed seed frequency.
     *
     * Some of the hits are on the reverse strand of the query.
    */
    const int32_t targetId = 123;
    std::vector<PacBio::Pancake::SeedDB::SeedRaw> targetSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, targetId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 4, true),
        PacBio::Pancake::SeedDB::Seed::Encode(0, targetId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, targetId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, targetId, 8, false),
    };
    const int32_t queryId = 123;
    const std::vector<PacBio::Pancake::SeedDB::SeedRaw> querySeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, queryId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 1, true),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 2, true),
        PacBio::Pancake::SeedDB::Seed::Encode(7, queryId, 3, true),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, queryId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, queryId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, queryId, 8, false),
    };
    int32_t freqCutoff = 0;

    // Load the SeedDB cache.
    // Needed here because of the target sequence lengths.
    const std::string targetSeedDBString =
        R"(V	0.1.0
P	k=30,w=80,hpc=0,hpc_len=10,rc=1
F	0	dummy.seeddb.0.seeds	1	96
S	123	targetSeq0	0	0	96	38	6
B	0	0	1	96
    )";
    std::istringstream is(targetSeedDBString);
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> targetSeedDBCache =
        PacBio::Pancake::LoadSeedDBIndexCache(is, "filename.seeddb");

    // Expected results.
    const std::vector<PacBio::Pancake::SeedHit> expected = {
        // 0
        {targetId, 0, 0, 0, 0},
        {targetId, 0, 5, 0, 0},
        // 123
        {targetId, 1, 8, 0, 1},
        {targetId, 1, 3, 0, 1},
        {targetId, 1, 1, 0, 1},
        /// 5
        {targetId, 1, 7, 0, 2},
        {targetId, 1, 2, 0, 2},
        {targetId, 0, 4, 0, 2},
        /// 7
        {targetId, 1, 6, 0, 3},
        // 5
        {targetId, 0, 2, 0, 4},
        {targetId, 0, 7, 0, 4},
        {targetId, 1, 5, 0, 4},
        // 0
        {targetId, 0, 0, 0, 5},
        {targetId, 0, 5, 0, 5},
        // 123
        {targetId, 0, 1, 0, 6},
        {targetId, 0, 6, 0, 6},
        {targetId, 0, 8, 0, 6},
        // 5
        {targetId, 0, 2, 0, 7},
        {targetId, 0, 7, 0, 7},
        {targetId, 1, 5, 0, 7},
        // 123
        {targetId, 0, 1, 0, 8},
        {targetId, 0, 6, 0, 8},
        {targetId, 0, 8, 0, 8},
    };

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(targetSeedDBCache, std::move(targetSeeds));
    std::vector<PacBio::Pancake::SeedHit> results;
    si.CollectHits(querySeeds, results, freqCutoff);

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& hit = results[i];
        // std::cerr << "[i = " << i << "] targetId = " << hit.targetId
        //           << ", targetRev = " << hit.targetRev << ", targetPos = " << hit.targetPos
        //           << ", flags = " << hit.flags << ", queryPos = " << hit.queryPos << "\n";
        std::cerr << "{targetId"
                  << ", " << hit.targetRev << ", " << hit.targetPos << ", " << hit.flags << ", "
                  << hit.queryPos << "},\n";
    }

    EXPECT_EQ(expected, results);
}

TEST(SeedIndex, ComputeFrequencyStatsEmptyIndex)
{
    /*
     * Initialize a SeedIndex, and compute the seed statistics.
    */
    // Input values.
    std::vector<__int128> inSeeds = {};
    double freqPercentile = 0.60;  // Large percentile, just because this is a small test.

    // Expected results.
    int64_t expectedFreqMax = 0;
    double expectedFreqAvg = 0;
    double expectedFreqMedian = 0;
    int64_t expectedFreqCutoff = 0;

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    int64_t resultFreqMax = 0;
    double resultFreqAvg = 0.0;
    double resultFreqMedian = 0.0;
    int64_t resultFreqCutoff = 0;
    si.ComputeFrequencyStats(freqPercentile, resultFreqMax, resultFreqAvg, resultFreqMedian,
                             resultFreqCutoff);

    // Evaluate.
    EXPECT_EQ(
        std::make_tuple(expectedFreqMax, expectedFreqAvg, expectedFreqMedian, expectedFreqCutoff),
        std::make_tuple(resultFreqMax, resultFreqAvg, resultFreqMedian, resultFreqCutoff));
}

TEST(SeedIndex, ComputeFrequencyStatsNormal)
{
    /*
     * Initialize a SeedIndex, and compute the seed statistics.
    */
    // Input values.
    int32_t seqId = 123;
    std::vector<__int128> inSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, seqId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 8, false),
    };
    double freqPercentile = 0.60;  // Large percentile, just because this is a small test.

    // Expected results.
    int64_t expectedFreqMax = 3;
    double expectedFreqAvg = 2.25;
    double expectedFreqMedian = 2.5;
    int64_t expectedFreqCutoff = 2;

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    int64_t resultFreqMax = 0;
    double resultFreqAvg = 0.0;
    double resultFreqMedian = 0.0;
    int64_t resultFreqCutoff = 0;
    si.ComputeFrequencyStats(freqPercentile, resultFreqMax, resultFreqAvg, resultFreqMedian,
                             resultFreqCutoff);

    // Evaluate.
    EXPECT_EQ(
        std::make_tuple(expectedFreqMax, expectedFreqAvg, expectedFreqMedian, expectedFreqCutoff),
        std::make_tuple(resultFreqMax, resultFreqAvg, resultFreqMedian, resultFreqCutoff));
}

TEST(SeedIndex, ComputeFrequencyThresholdOutOfBounds)
{
    /*
     * This should throw, because freqPercentile should be in [0.0, 1.0].
    */
    // Input values.
    int32_t seqId = 123;
    std::vector<__int128> inSeeds = {
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 0, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 1, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 2, false),
        PacBio::Pancake::SeedDB::Seed::Encode(7, seqId, 3, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 4, false),
        PacBio::Pancake::SeedDB::Seed::Encode(0, seqId, 5, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 6, false),
        PacBio::Pancake::SeedDB::Seed::Encode(5, seqId, 7, false),
        PacBio::Pancake::SeedDB::Seed::Encode(123, seqId, 8, false),
    };

    // Dummy SeedDB cache.
    // No cache information is needed for this test.
    std::shared_ptr<PacBio::Pancake::SeedDBIndexCache> dummySeedDBCache(
        new PacBio::Pancake::SeedDBIndexCache);

    // Run the unit under test.
    PacBio::Pancake::SeedIndex si(dummySeedDBCache, std::move(inSeeds));
    int64_t resultFreqMax = 0;
    double resultFreqAvg = 0.0;
    double resultFreqMedian = 0.0;
    int64_t resultFreqCutoff = 0;

    // Evaluate.
    EXPECT_THROW(
        {
            si.ComputeFrequencyStats(1.5, resultFreqMax, resultFreqAvg, resultFreqMedian,
                                     resultFreqCutoff);
        },
        std::runtime_error);
    EXPECT_THROW(
        {
            si.ComputeFrequencyStats(-1.0, resultFreqMax, resultFreqAvg, resultFreqMedian,
                                     resultFreqCutoff);
        },
        std::runtime_error);
}