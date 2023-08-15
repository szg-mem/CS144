#include "wrapping_integers.hh"

using namespace std;

Wrap32 Wrap32::wrap( uint64_t n, Wrap32 zero_point )
{
  return Wrap32( ( n + zero_point.raw_value_ ) % ( 1LL << 32 ) );
}

uint64_t Wrap32::unwrap( Wrap32 zero_point, uint64_t checkpoint ) const
{
  const uint64_t result = ( raw_value_ + ( 1LL << 32 ) - zero_point.raw_value_ ) % ( 1LL << 32 );
  const uint64_t bound = checkpoint / ( 1LL << 32 );
  if ( bound * ( 1LL << 32 ) + result <= checkpoint ) {
    if ( checkpoint - ( bound * ( 1LL << 32 ) + result ) < ( bound + 1 ) * ( 1LL << 32 ) + result - checkpoint ) {
      return bound * ( 1LL << 32 ) + result;
    }
    return ( bound + 1 ) * ( 1LL << 32 ) + result;
  }
  if ( bound == 0 ) {
    return result;
  }
  if ( checkpoint - ( ( bound - 1 ) * ( 1LL << 32 ) + result ) < bound * ( 1LL << 32 ) + result - checkpoint ) {
    return ( bound - 1 ) * ( 1LL << 32 ) + result;
  }
  return bound * ( 1LL << 32 ) + result;
}
