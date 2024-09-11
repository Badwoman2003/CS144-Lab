#include "wrapping_integers.hh"
#include <iostream>

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  // Your code here.
  uint32_t trans = static_cast<uint32_t>( n & 0xFFFFFFFF );
  return Wrap32 { zero_point + trans };
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  // Your code here.
  Wrap32 check_seq = wrap( checkpoint, zero_point );
  if ( *this == check_seq ) {
    return checkpoint;
  } else {
    if ( this->raw_value_ < check_seq.raw_value_ ) {
      uint32_t direct_distance = check_seq.raw_value_ - this->raw_value_;
      uint32_t wrapped_distance = this->raw_value_ - check_seq.raw_value_;

      if ( (direct_distance < wrapped_distance)&&(checkpoint>=direct_distance) ) {
        return checkpoint - direct_distance;
      } else {
        return checkpoint + wrapped_distance;
      }
    } else {
      uint32_t direct_distance = this->raw_value_ - check_seq.raw_value_;
      uint32_t wrapped_distance = check_seq.raw_value_ - this->raw_value_;

      if ( ( wrapped_distance < direct_distance )
           && (checkpoint>=wrapped_distance) ) {
        return checkpoint - wrapped_distance;
      } else {
        return checkpoint + direct_distance;
      }
    }
  }
}
