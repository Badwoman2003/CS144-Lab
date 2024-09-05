#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  unassemble_str str( std::move( data ), data.size(), first_index, is_last_substring );
  if ( first_index <= next_idx && first_index + data.size() > next_idx ) {
    uint64_t break_start = next_idx - first_index;
    uint64_t break_end = data.size();
    for ( auto it = str_buffer.begin(); it != str_buffer.end(); ) {
      const auto& existing_str = *it;
      uint64_t existing_start = existing_str.idx;
      uint64_t existing_end = existing_start + existing_str.len;

      if ( existing_start >= first_index && existing_end <= first_index + data.size() ) {
        it = str_buffer.erase( it );
      } else {
        ++it;
      }
    }
    std::string sub_str = data.substr( break_start, break_end );
    output_.writer().push( sub_str );
    next_idx += sub_str.size();
    unassemble_str s = *( str_buffer.upper_bound( str ) );
    std::string overlap_str;
    if ( s.idx <= ( str.idx + str.len ) ) {
      overlap_str = s.data.substr( str.idx - s.idx, s.len );
      output_.writer().push( overlap_str );
      next_idx += overlap_str.size();
    }
  } else {
    if ( first_index > next_idx ) {
      push_to_buffer( str, str_buffer );
    }
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  uint64_t total_size = 0;
  for ( const auto& unassembled : str_buffer ) {
    total_size += unassembled.len;
  }
  return total_size;
}
