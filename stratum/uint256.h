// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef UINT256_H
#define UINT256_H

#include <assert.h>
#include <cstring>
#include <stdexcept>
#include <stdint.h>
#include <string>
#include <vector>

uint32_t static inline ReadLE32(const uint8_t *ptr) {
    uint32_t x;
    memcpy((char *)&x, ptr, 4);
    return le32toh(x);
}

uint64_t static inline ReadLE64(const unsigned char* ptr)
{
    uint64_t x;
    memcpy((char*)&x, ptr, 8);
    return le64toh(x);
}

void static inline WriteLE32(uint8_t *ptr, uint32_t x) {
    uint32_t v = htole32(x);
    memcpy(ptr, (char *)&v, 4);
}

void static inline WriteLE64(uint8_t *ptr, uint64_t x) {
    uint64_t v = htole64(x);
    memcpy(ptr, (char *)&v, 8);
}

/** Template base class for fixed-sized opaque blobs. */
template<unsigned int BITS>
class base_blob
{
protected:
    enum { WIDTH=BITS/8 };
    uint8_t data[WIDTH];
public:
    base_blob()
    {
        memset(data, 0, sizeof(data));
    }

    explicit base_blob(const std::vector<unsigned char>& vch);

    bool IsNull() const
    {
        for (int i = 0; i < WIDTH; i++)
            if (data[i] != 0)
                return false;
        return true;
    }

    void SetNull()
    {
        memset(data, 0, sizeof(data));
    }

    int CompareTo(const base_blob& b) const;
    inline int Compare(const base_blob& other) const { return memcmp(data, other.data, sizeof(data)); }

    friend inline bool operator==(const base_blob& a, const base_blob& b) { return a.Compare(b) == 0; }
    friend inline bool operator!=(const base_blob& a, const base_blob& b) { return a.Compare(b) != 0; }
    friend inline bool operator>(const base_blob& a, const base_blob& b) { return a.CompareTo(b) > 0; }
    friend inline bool operator<(const base_blob& a, const base_blob& b) { return a.CompareTo(b) < 0; }
    friend inline bool operator>=(const base_blob& a, const base_blob& b) { return a.CompareTo(b) >= 0; }
    friend inline bool operator<=(const base_blob& a, const base_blob& b) { return a.CompareTo(b) <= 0; }

    std::string GetHex() const;
    void SetHex(const char* psz);
    void SetHex(const std::string& str);
    std::string ToString() const;

    unsigned char* begin()
    {
        return &data[0];
    }

    unsigned char* end()
    {
        return &data[WIDTH];
    }

    const unsigned char* begin() const
    {
        return &data[0];
    }

    const unsigned char* end() const
    {
        return &data[WIDTH];
    }

    unsigned int size() const
    {
        return sizeof(data);
    }

    uint64_t GetUint64(int pos) const
    {
        const uint8_t* ptr = data + pos * 8;
        return ((uint64_t)ptr[0]) | \
               ((uint64_t)ptr[1]) << 8 | \
               ((uint64_t)ptr[2]) << 16 | \
               ((uint64_t)ptr[3]) << 24 | \
               ((uint64_t)ptr[4]) << 32 | \
               ((uint64_t)ptr[5]) << 40 | \
               ((uint64_t)ptr[6]) << 48 | \
               ((uint64_t)ptr[7]) << 56;
    }

    template<typename Stream>
    void Serialize(Stream& s) const
    {
        s.write((char*)data, sizeof(data));
    }

    template<typename Stream>
    void Unserialize(Stream& s)
    {
        s.read((char*)data, sizeof(data));
    }
};

/** 160-bit opaque blob.
 * @note This type is called uint160 for historical reasons only. It is an opaque
 * blob of 160 bits and has no integer operations.
 */
class uint160 : public base_blob<160> {
public:
    uint160() {}
    explicit uint160(const std::vector<unsigned char>& vch) : base_blob<160>(vch) {}
};

/** 256-bit opaque blob.
 * @note This type is called uint256 for historical reasons only. It is an
 * opaque blob of 256 bits and has no integer operations. Use arith_uint256 if
 * those are required.
 */
class uint256 : public base_blob<256> {
public:
    uint256() {}
    explicit uint256(const std::vector<unsigned char>& vch) : base_blob<256>(vch) {}

    int GetNibble(int index) const 
    {
        index = 63 - index;
        if (index % 2 == 1)
            return(data[index / 2] >> 4);
        return(data[index / 2] & 0x0F); 
    }
    /** A cheap hash function that just returns 64 bits from the result, it can be
     * used when the contents are considered uniformly random. It is not appropriate
     * when the value can easily be influenced from outside as e.g. a network adversary could
     * provide values to trigger worst-case behavior.
     */
    uint64_t GetCheapHash() const
    {
        return ReadLE64(data);
    }
};

/* uint256 from const char *.
 * This is a separate function because the constructor uint256(const char*) can result
 * in dangerously catching uint256(0).
 */
inline uint256 uint256S(const char *str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

/* uint256 from std::string.
 * This is a separate function because the constructor uint256(const std::string &str) can result
 * in dangerously catching uint256(0) via std::string(const char*).
 */
inline uint256 uint256S(const std::string& str)
{
    uint256 rv;
    rv.SetHex(str);
    return rv;
}

class uint512 : public base_blob<512> {
public:
    uint512() {}
    uint512(const base_blob<512>& b) : base_blob<512>(b) {}
    explicit uint512(const std::vector<unsigned char>& vch) : base_blob<512>(vch) {}
    uint256 trim256() const
    {
        uint256 result;
        memcpy((void*)&result, (void*)data, 32);
        return result;
    }
};

#endif // UINT256_H
