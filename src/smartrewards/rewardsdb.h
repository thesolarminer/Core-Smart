// Copyright (c) 2017 - 2019 - The SmartCash Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef REWARDSDB_H
#define REWARDSDB_H

#include "dbwrapper.h"
#include "amount.h"
#include "chain.h"
#include "coins.h"
#include "base58.h"
#include "smarthive/hive.h"

static constexpr uint8_t REWARDS_DB_VERSION = 0x09;

//! Compensate for extra memory peak (x1.5-x1.9) at flush time.
static constexpr int REWARDS_DB_PEAK_USAGE_FACTOR = 2;
//! -rewardsdbcache default (MiB)
static const int64_t nRewardsDefaultDbCache = 80;
//! max. -rewardsdbcache (MiB)
static const int64_t nRewardsMaxDbCache = sizeof(void*) > 4 ? 16384 : 1024;

class CSmartRewardBlock;
class CSmartRewardEntry;
class CSmartRewardRound;
class CSmartRewardRoundResult;
class CSmartRewardTransaction;

typedef std::vector<CSmartRewardBlock> CSmartRewardBlockList;
typedef std::vector<CSmartRewardEntry> CSmartRewardEntryList;
typedef std::vector<CSmartRewardRound> CSmartRewardRoundList;
typedef std::vector<CSmartRewardRoundResult> CSmartRewardRoundResultList;
typedef std::vector<CSmartRewardRoundResult*> CSmartRewardRoundResultPtrList;
typedef std::vector<CSmartRewardTransaction> CSmartRewardTransactionList;

typedef std::map<CSmartAddress, CSmartRewardEntry*> CSmartRewardEntryMap;

class CSmartRewardTransaction
{

public:
    int blockHeight;
    uint256 hash;
    CSmartRewardTransaction(){blockHeight = 0; hash = uint256();}
    CSmartRewardTransaction(int height, const uint256 &hash) : blockHeight(height), hash(hash) {}

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(blockHeight);
        READWRITE(hash);
    }

    friend bool operator==(const CSmartRewardTransaction& a, const CSmartRewardTransaction& b)
    {
        return (a.hash == b.hash);
    }

    friend bool operator!=(const CSmartRewardTransaction& a, const CSmartRewardTransaction& b)
    {
        return !(a == b);
    }

    std::string ToString() const;
};
class CSmartRewardBlock
{

public:
    int nHeight;
    uint256 blockHash;
    int64_t blockTime;
    CSmartRewardBlock(){nHeight = 0; blockHash = uint256(); blockTime = 0;}
    CSmartRewardBlock(int height, const uint256* pHash, int64_t time) : nHeight(height), blockHash(*pHash), blockTime(time) {}
    CSmartRewardBlock(int height, const uint256 nHash, int64_t time) : nHeight(height), blockHash(nHash), blockTime(time) {}

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(nHeight);
        READWRITE(blockHash);
        READWRITE(blockTime);
    }

    friend bool operator==(const CSmartRewardBlock& a, const CSmartRewardBlock& b)
    {
        return (a.nHeight == b.nHeight);
    }

    friend bool operator!=(const CSmartRewardBlock& a, const CSmartRewardBlock& b)
    {
        return !(a == b);
    }

    friend bool operator<(const CSmartRewardBlock& a, const CSmartRewardBlock& b)
    {
        return (a.nHeight < b.nHeight);
    }

    std::string ToString() const;

    bool IsValid() const { return !blockHash.IsNull(); }
};

class CSmartRewardRound
{
public:
    uint16_t number;
    int64_t startBlockHeight;
    int64_t startBlockTime;
    int64_t endBlockHeight;
    int64_t endBlockTime;

    int64_t eligibleEntries;
    CAmount eligibleSmart;
    int64_t disqualifiedEntries;
    CAmount disqualifiedSmart;

    int64_t nBlockPayees;
    int64_t nBlockInterval;

    CAmount rewards;
    double percent;

    CSmartRewardRound() {
        number = 0;
        startBlockHeight = INT_MAX;
        startBlockTime = INT_MAX;
        endBlockHeight = INT_MAX;
        endBlockTime = INT_MAX;
        eligibleEntries = 0;
        eligibleSmart = 0;
        disqualifiedEntries = 0;
        disqualifiedSmart = 0;
        nBlockPayees = 0;
        nBlockInterval = 0;
        rewards = 0;
        percent = 0;
    }

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(number);
        READWRITE(startBlockHeight);
        READWRITE(startBlockTime);
        READWRITE(endBlockHeight);
        READWRITE(endBlockTime);
        READWRITE(eligibleEntries);
        READWRITE(eligibleSmart);
        READWRITE(disqualifiedEntries);
        READWRITE(disqualifiedSmart);
        READWRITE(nBlockPayees);
        READWRITE(nBlockInterval);
        READWRITE(rewards);
        READWRITE(percent);
    }

    friend bool operator<(const CSmartRewardRound& a, const CSmartRewardRound& b)
    {
        return a.number < b.number;
    }

    std::string ToString() const;
};

class CSmartRewardEntry
{

public:

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(id);
        READWRITE(balance);
        READWRITE(balanceAtStart);
        READWRITE(balanceEligible);
        READWRITE(disqualifyingTx);
        READWRITE(voteProof);
        READWRITE(smartnodePaymentTx);
    }

    CSmartAddress id;
    CAmount balance;
    CAmount balanceAtStart;
    CAmount balanceEligible;
    CAmount reward;
    uint256 disqualifyingTx;
    uint256 voteProof;
    uint256 smartnodePaymentTx;

    CSmartRewardEntry() : id(CSmartAddress()),
                          balance(0), balanceAtStart(0), balanceEligible(0), reward(0),
                          disqualifyingTx(uint256()), voteProof(uint256()),
                          smartnodePaymentTx(uint256()) {}
    CSmartRewardEntry(const CSmartAddress &address) : id(address),
                          balance(0), balanceAtStart(0), balanceEligible(0), reward(0),
                          disqualifyingTx(uint256()), voteProof(uint256()),
                          smartnodePaymentTx(uint256()) {}

    friend bool operator==(const CSmartRewardEntry& a, const CSmartRewardEntry& b)
    {
        return (a.id == b.id);
    }

    friend bool operator!=(const CSmartRewardEntry& a, const CSmartRewardEntry& b)
    {
        return !(a == b);
    }

    std::string GetAddress() const;
    void setNull();
    std::string ToString() const;
    bool IsEligible();
};

class CSmartRewardRoundResult
{

public:

    CSmartRewardEntry entry;
    CAmount reward;

    ADD_SERIALIZE_METHODS

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(entry);
        READWRITE(reward);
    }

    CSmartRewardRoundResult(){}

    CSmartRewardRoundResult(CSmartRewardEntry &entry, CAmount nReward) :
        entry(entry), reward(nReward){}

    friend bool operator==(const CSmartRewardRoundResult& a, const CSmartRewardRoundResult& b)
    {
        return (a.entry.id == b.entry.id);
    }

    friend bool operator!=(const CSmartRewardRoundResult& a, const CSmartRewardRoundResult& b)
    {
        return !(a == b);
    }

    friend bool operator<(const CSmartRewardRoundResult& a, const CSmartRewardRoundResult& b)
    {
        // TBD, verify this sort is fast/unique
        int cmp = a.entry.id.Compare(b.entry.id);
        return cmp < 0 || (cmp == 0 && a.reward < b.reward);
    }

    std::string GetAddress() const;
    std::string ToString() const;

    arith_uint256 CalculateScore(const uint256& blockHash);
};


/** Access to the rewards database (rewards/) */
class CSmartRewardsDB : public CDBWrapper
{
public:
    CSmartRewardsDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);
    ~CSmartRewardsDB() { Unlock(); }
private:
    CSmartRewardsDB(const CSmartRewardsDB&);
    void operator=(const CSmartRewardsDB&);

    bool locked;
    void Unlock();
public:

    bool Verify(int& lastBlockHeight);

    void Lock();
    bool IsLocked();

    bool ReadBlock(const int nHeight, CSmartRewardBlock &block);
    bool ReadLastBlock(CSmartRewardBlock &block);

    bool ReadTransaction(const uint256 hash, CSmartRewardTransaction &transaction);

    bool ReadRound(const int16_t number, CSmartRewardRound &round);
    bool ReadRounds(CSmartRewardRoundList &vect);

    bool ReadCurrentRound(CSmartRewardRound &round);

    bool ReadRewardEntry(const CSmartAddress &id, CSmartRewardEntry &entry);
    bool ReadRewardEntries(CSmartRewardEntryList &vect);

    bool ReadRewardRoundResults(const int16_t round, CSmartRewardRoundResultList &results);
    bool ReadRewardPayouts(const int16_t round, CSmartRewardRoundResultList &payouts);
    bool ReadRewardPayouts(const int16_t round, CSmartRewardRoundResultPtrList &payouts);

    bool SyncCached(const CSmartRewardRound& current, const CSmartRewardEntryMap &rewards, const CSmartRewardTransactionList &transactions, bool fUndo = false);
    bool SyncCached(const CSmartRewardBlock &block, const CSmartRewardRound& current, const CSmartRewardEntryMap &rewards, const CSmartRewardTransactionList &transactions, bool fUndo = false);
    bool StartFirstRound(const CSmartRewardRound &start, const CSmartRewardEntryList &entries);
    bool FinalizeRound(const CSmartRewardRound &current, const CSmartRewardRound &next, const CSmartRewardEntryList &entries, const CSmartRewardRoundResultList &results);
    bool UndoFinalizeRound(const CSmartRewardRound &current, const CSmartRewardRoundResultList &results);
};



#endif // REWARDSDB_H
