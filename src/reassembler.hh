#pragma once

#include "byte_stream.hh"
#include <algorithm>
#include <set>

struct unassemble_str
{
  std::string data;
  uint64_t len;
  uint64_t idx;
  bool eof;
  unassemble_str( std::string&& data_, uint64_t len_, uint64_t idx_, bool eof_ ):data(std::move( data_ )),len(0),idx(0),eof(false)
  {
    data = std::move( data_ );
    len = len_;
    idx = idx_;
    eof = eof_;
  };
  bool operator<( const unassemble_str& other ) const { return idx > other.idx; }
};

unassemble_str merge( const unassemble_str& s1, const unassemble_str& s2 )
{
  uint64_t new_idx = std::min( s1.idx, s2.idx );
  uint64_t new_end = std::max( s1.idx + s1.len, s2.idx + s2.len );
  uint64_t new_len = new_end - new_idx;

  std::string merged_data( new_len, '\0' );

  std::copy( s1.data.begin(), s1.data.end(), merged_data.begin() + ( s1.idx - new_idx ) );
  std::copy( s2.data.begin(), s2.data.end(), merged_data.begin() + ( s2.idx - new_idx ) );

  bool new_eof = s1.eof || s2.eof;

  return unassemble_str( std::move( merged_data ), new_len, new_idx, new_eof );
}

void push_to_buffer( unassemble_str str, std::set<unassemble_str>& buffer )
{
  auto it = buffer.begin();

  while ( it != buffer.end() ) {
    unassemble_str existing_str = *it;

    if ( std::max( str.idx, existing_str.idx )
         < std::min( str.idx + str.len, existing_str.idx + existing_str.len ) ) {
      str = merge( str, existing_str );
      buffer.erase( *it );
    }
    ++it;
  }

  buffer.insert( str );
}

class Reassembler
{
public:
  // Construct Reassembler to write into given ByteStream.
  explicit Reassembler( ByteStream&& output ) : output_( std::move( output ) ) {}

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
  ByteStream output_; // the Reassembler writes to this ByteStream
  std::set<unassemble_str> str_buffer = {};
  uint64_t next_idx = 0;
};
