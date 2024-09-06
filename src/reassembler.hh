#pragma once

#include "byte_stream.hh"
#include <algorithm>
#include <list>
#include <map>
#include <numeric>
#include <set>

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output )
    : output_( std::move( output ) )
    , _capacity( output.bytes_cap() )
    , assembled_bytes( 0 )
    , stored_bytes( 0 )
    , _str_to_assemble()
    , _existed()
    , _eof( false )
    , eof_idx()
    , _used_byte { std::make_pair( 0, std::numeric_limits<size_t>::max() ) }
  {}

  /*
   * Insert a new substring to be reassembled into a ByteStream.
   *   `first_index`: the index of the first byte of the substring
   *   `data`: the substring itself
   *   `is_last_substring`: this substring represents the end of the stream
   *   `output`: a mutable reference to the Writer
   *
   * The Reassembler's job is to reassemble the indexed substrings (possibly out-of-order
   * and possibly overlapping) back into the original ByteStream. As soon as the Reassembler
   * learns the next byte in the stream, it should write it to the output.
   *
   * If the Reassembler learns about bytes that fit within the stream's available capacity
   * but can't yet be written (because earlier bytes remain unknown), it should store them
   * internally until the gaps are filled in.
   *
   * The Reassembler should discard any bytes that lie beyond the stream's available capacity
   * (i.e., bytes that couldn't be written even if earlier gaps get filled in).
   *
   * The Reassembler should close the stream after writing the last byte.
   */
  void insert( uint64_t first_index, std::string data, bool is_last_substring );

  // How many bytes are stored in the Reassembler itself?
  uint64_t bytes_pending() const;

  // Access output stream reader
  Reader& reader() { return output_.reader(); }
  const Reader& reader() const { return output_.reader(); }

  // Access output stream writer, but const-only (can't write from outside)
  const Writer& writer() const { return output_.writer(); }

private:
  using str_list = std::list<std::pair<std::string, size_t>>;
  using str_map = std::map<size_t, str_list::iterator>;
  ByteStream output_; // the Reassembler writes to this ByteStream
  size_t _capacity;
  size_t assembled_bytes;
  size_t stored_bytes;
  str_list _str_to_assemble;
  str_map _existed;
  bool _eof;
  size_t eof_idx;
  std::set<std::pair<size_t, size_t>> _used_byte;
  using Type1 = std::set<std::pair<size_t, size_t>>::iterator;
  using Type2 = std::vector<std::pair<size_t, size_t>>;
  void remove_segement( const Type1& it, size_t left_, size_t right_, Type2& erase, Type2& insert );
  bool empty() const;
};
