#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity )
  : capacity_( capacity ), error_( false ), read_count( 0 ), write_count( 0 )
{}

bool Writer::is_closed() const
{
  // Your code here.
  return is_close_;
}

void Writer::push( string data )
{
  // Your code here.
  if ( is_closed() )
    return;
  uint64_t space = writable_space();
  int s = int( space );
  printf( "%d", s );
  if ( space > data.size() ) {
    bStream.insert( bStream.end(), data.begin(), data.begin() + data.size() );
    write_count += data.size();
  } else {
    bStream.insert( bStream.end(), data.begin(), data.begin() + space );
    write_count += space;
  }

  return;
}

void Writer::close()
{
  // Your code here.
  is_close_ = true;
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  return writable_space();
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return write_count;
}

bool Reader::is_finished() const
{
  // Your code here.
  return ( is_close_  &&bStream.empty());
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return read_count;
}

string_view Reader::peek() const
{
  // Your code here.
  if ( bStream.empty() ) {
    return std::string_view(); // return null
  }
  std::string_view s_peek( &bStream.front(), 1 );
  return s_peek;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  if ( len <= bStream.size() ) {
    for ( size_t i = 0; i < len; i++ ) {
      bStream.pop_front();
      read_count++;
    }
  }
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return bStream.size();
}
