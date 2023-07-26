#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity )
{
  container_.reserve( capacity );
}

void Writer::push( const string& data )
{
  const uint64_t current_capacity = available_capacity();
  const uint64_t push_size = min( current_capacity, data.size() );
  container_ += data.substr( 0, push_size );
  push_count_ += push_size;
}

void Writer::close()
{
  close_ = true;
}

void Writer::set_error()
{
  error_ = true;
}

bool Writer::is_closed() const
{
  return close_;
}

uint64_t Writer::available_capacity() const
{
  return capacity_ - container_.size();
}

uint64_t Writer::bytes_pushed() const
{
  return push_count_;
}

string_view Reader::peek() const
{
  return container_;
}

bool Reader::is_finished() const
{
  return close_ and container_.empty();
}

bool Reader::has_error() const
{
  return error_;
}

void Reader::pop( uint64_t len )
{
  const auto pop_size = min( len, container_.size() );
  container_.erase( container_.begin(), container_.begin() + static_cast<int>( pop_size ) );
  pop_count_ += pop_size;
}

uint64_t Reader::bytes_buffered() const
{
  return container_.size();
}

uint64_t Reader::bytes_popped() const
{
  return pop_count_;
}
