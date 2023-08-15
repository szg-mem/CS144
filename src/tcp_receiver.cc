#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream )
{
  if ( message.SYN ) {
    ISN_ = message.seqno;
  }
  if ( not ISN_.has_value() ) {
    return;
  }
  reassembler.insert( get_first_index( inbound_stream, message ), message.payload, message.FIN, inbound_stream );
  const bool is_closed = inbound_stream.is_closed();
  ackno_ = get_wrap_raw_value( Wrap32::wrap( inbound_stream.bytes_pushed() + 1 + is_closed, *ISN_ ) );
}

TCPReceiverMessage TCPReceiver::send( const Writer& inbound_stream ) const
{
  auto available_size = inbound_stream.available_capacity();
  const uint16_t window_size = available_size > UINT16_MAX ? UINT16_MAX : available_size;
  if ( not ISN_.has_value() ) {
    return { nullopt, window_size };
  }
  if ( not ackno_.has_value() ) {
    return {};
  }
  return { Wrap32( *ackno_ ), window_size };
}

uint64_t TCPReceiver::get_first_index( const Writer& write, const TCPSenderMessage& message )
{
  const Wrap32 first_abs_seq { message.seqno + message.SYN };
  uint64_t result {};
  if ( ISN_.has_value() ) {
    result = first_abs_seq.unwrap( *ISN_, write.bytes_pushed() ) - 1;
  }
  return result;
}

uint64_t TCPReceiver::get_wrap_raw_value( const Wrap32& value )
{
  ackno_.has_value();
  return value.unwrap( Wrap32( 0 ), 0 );
}
