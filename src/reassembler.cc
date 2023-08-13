#include "reassembler.hh"

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring, Writer& output )
{
  update_indexes( output );
  if ( is_last_substring ) {
    is_last_substring_ = is_last_substring;
  }
  if ( first_index + data.size() <= first_unassembled_index_ || first_index >= first_unacceptable_index_ ) {
    if ( is_last_substring_ && unassembled_container_.empty() ) {
      output.close();
    }
    return;
  }
  judge_cut_data( first_index, data );
  if ( unassembled_container_.find( first_index ) != unassembled_container_.end() ) {
    if ( data.size() > unassembled_container_[first_index].size() ) {
      unassembled_bytes_ += data.size() - unassembled_container_[first_index].size();
      unassembled_container_[first_index] = move( data );
    }
  } else {
    unassembled_bytes_ += data.size();
    unassembled_container_.insert( { first_index, move( data ) } );
  }
  merge( first_index );
  while ( unassembled_container_.begin()->first == first_unassembled_index_ ) {
    unassembled_bytes_ -= unassembled_container_.begin()->second.size();
    output.push( unassembled_container_.begin()->second );
    first_unassembled_index_ += unassembled_container_.begin()->second.size();
    unassembled_container_.erase( unassembled_container_.begin() );
  }
  if ( is_last_substring_ && unassembled_container_.empty() ) {
    output.close();
    is_last_substring_ = false;
  }
}

uint64_t Reassembler::bytes_pending() const
{
  return unassembled_bytes_;
}

void Reassembler::update_indexes( const Writer& write )
{
  first_unpopped_index_ = write.reader().bytes_popped();
  first_unassembled_index_ = first_unpopped_index_ + write.reader().peek().size();
  auto total_capacity = write.available_capacity() + write.reader().peek().size();
  first_unacceptable_index_ = first_unpopped_index_ + total_capacity;
}

void Reassembler::judge_cut_data( uint64_t& first_index, string& data ) const
{
  if ( first_index < first_unassembled_index_ ) {
    data.erase( 0, first_unassembled_index_ - first_index );
    first_index = first_unassembled_index_;
  }
  if ( first_index + data.size() > first_unacceptable_index_ ) {
    data.erase( first_unacceptable_index_ - first_index, data.size() + first_index - first_unacceptable_index_ );
  }
}

void Reassembler::merge( uint64_t first_index )
{
  auto it = unassembled_container_.find( first_index );
  if ( it != unassembled_container_.begin() ) {
    auto pre = prev( it );
    if ( pre->first + pre->second.size() >= it->first + it->second.size() ) {
      unassembled_bytes_ -= it->second.size();
      unassembled_container_.erase( it );
      return;
    }
    if ( pre->first + pre->second.size() > it->first ) {
      unassembled_bytes_ -= pre->first + pre->second.size() - it->first;
      auto substr_start = pre->first + pre->second.size() - it->first;
      auto substr_length = it->first + it->second.size() - pre->first - pre->second.size();
      pre->second += it->second.substr( substr_start, substr_length );
      unassembled_container_.erase( it );
      it = pre;
    }
  }
  for ( auto next_iterator = next( it ); next_iterator != unassembled_container_.end(); ) {
    if ( it->first + it->second.size() < next_iterator->first + next_iterator->second.size() ) {
      if ( it->first + it->second.size() > next_iterator->first ) {
        unassembled_bytes_ -= it->first + it->second.size() - next_iterator->first;
        auto substr_start = it->first + it->second.size() - next_iterator->first;
        auto substr_length = next_iterator->first + next_iterator->second.size() - it->first - it->second.size();
        it->second += next_iterator->second.substr( substr_start, substr_length );
        unassembled_container_.erase( next_iterator );
      }
      break;
    }
    unassembled_bytes_ -= next_iterator->second.size();
    next_iterator = unassembled_container_.erase( next_iterator );
  }
}