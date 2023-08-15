#pragma once

#include "reassembler.hh"
#include "tcp_receiver_message.hh"
#include "tcp_sender_message.hh"

class TCPReceiver
{
public:
  /*
   * The TCPReceiver receives TCPSenderMessages, inserting their payload into the Reassembler
   * at the correct stream index.
   */
  void receive( TCPSenderMessage message, Reassembler& reassembler, Writer& inbound_stream );

  /* The TCPReceiver sends TCPReceiverMessages back to the TCPSender. */
  TCPReceiverMessage send( const Writer& inbound_stream ) const;

private:
  std::optional<Wrap32> ISN_ { std::nullopt };
  std::optional<uint32_t> ackno_ { std::nullopt };

  uint64_t get_first_index( const Writer& write, const TCPSenderMessage& message );
  uint64_t get_wrap_raw_value( const Wrap32& value );
};
