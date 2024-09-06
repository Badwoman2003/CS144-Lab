#include "reassembler.hh"

using namespace std;

template<typename... Targs>
void DUMMY_CODE( Targs&&... /* unused */ )
{}

void Reassembler::remove_segement( const Type1& it, size_t left_, size_t right_, Type2& erase, Type2& insert )
{
  DUMMY_CODE( it, left_, right_ );
  if ( left_ >= it->second || right_ <= it->first )
    return;
  erase.push_back( *it );
  if ( left_ >= it->first ) {
    if ( left_ > it->first ) {
      auto node = *it;
      node.second = left_;
      insert.push_back( node );
    }
    if ( it->second > right_ ) {
      auto node = make_pair( right_, it->second );
      insert.push_back( node );
    }
    stored_bytes += std::min( right_, it->second ) - left_;
  } else {
    stored_bytes += std::min( right_, it->second ) - it->first;
    auto node = *it;
    node.second = right_;
    if ( node.second > node.first )
      insert.push_back( node );
  }
}

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  DUMMY_CODE( data, first_index, is_last_substring );

  if (is_last_substring)
  {
    eof_idx = first_index+data.size();
  }
  
  if ( first_index >= _capacity + output_.reader().bytes_popped() )
    return;
  std::string temp( data );
  if ( first_index + data.size() > _capacity + output_.reader().bytes_popped() ) {
    temp.resize( _capacity + output_.reader().bytes_popped() - first_index );
  } else {
    _eof |= is_last_substring;
  }

  if ( first_index + temp.size() < assembled_bytes ) {
    if ( empty() ) {
      output_.writer().close();
    }
    return;
  }
  decltype( _used_byte )::iterator it_l = _used_byte.lower_bound( make_pair( first_index, first_index ) );
  decltype( _used_byte )::iterator it_r
    = _used_byte.lower_bound( make_pair( first_index + temp.size(), first_index + temp.size() ) );
  vector<pair<size_t, size_t>> _need_to_erase {}, _need_to_insert {};

  if ( it_l != _used_byte.begin() ) {
    it_l--;
  }
  while ( it_l != _used_byte.end() ) {
    remove_segement( it_l, first_index, first_index + temp.size(), _need_to_erase, _need_to_insert );
    if ( it_l == it_r ) {
      break;
    }
    it_l++;
  }
  for ( auto v : _need_to_erase )
    _used_byte.erase( v );
  for ( auto v : _need_to_insert )
    _used_byte.insert( v );
  decltype( _existed )::iterator it = _existed.lower_bound( first_index );
  if ( it == _existed.end() ) {
    _str_to_assemble.push_back( make_pair( temp, first_index ) );
    _existed[first_index] = ( --_str_to_assemble.end() );
  } else {
    if ( it->first == first_index ) {
      if ( temp.size() > ( it->second->first ).size() ) {
        *( it->second ) = make_pair( temp, first_index );
      }

    } else {
      _existed[first_index] = _str_to_assemble.insert( it->second, make_pair( temp, first_index ) );
    }
  }
  while ( !_str_to_assemble.empty() && _str_to_assemble.front().second <= assembled_bytes ) {
    size_t idx = _str_to_assemble.front().second;
    std::string _data = _str_to_assemble.front().first;
    if ( idx + _data.size() > assembled_bytes ) {
      output_.writer().push( _data.substr( assembled_bytes - idx, idx + _data.size() - assembled_bytes ) );
      assembled_bytes = idx + _data.size();
    }
    _existed.erase( idx );
    _str_to_assemble.pop_front();
  }
  if ( empty() ) {
    output_.writer().close();
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return stored_bytes - assembled_bytes;
}

bool Reassembler::empty() const
{
  return (assembled_bytes==eof_idx)&&_eof;
}