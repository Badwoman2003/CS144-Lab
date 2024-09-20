#include "tcp_receiver.hh"
#include <iostream>

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  checkpoint_ = reassembler_.writer().bytes_pushed() + isn_.has_value();
  // Your code here.
  if ( message.RST ) {
    RST = message.RST;
    reassembler_.reader().set_error();
  }
  else if (checkpoint_ > 0 && checkpoint_ <= UINT32_MAX && message.seqno == isn_)
  {
    return;
  }
  if ( isn_ == std::nullopt ) // have not start
  {
    if (!message.SYN)
    {
      return;
    }
    isn_ = message.seqno;
  }

  uint64_t abs_SeqNo = ( message.seqno + static_cast<int>( message.SYN ) ).unwrap( isn_.value(), checkpoint_ );
  reassembler_.insert( abs_SeqNo - 1, message.payload, message.FIN );
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  uint64_t checkpoint = reassembler_.writer().bytes_pushed() + isn_.has_value();
  uint64_t capacity = reassembler_.writer().available_capacity();
  const uint16_t window_size = capacity > UINT16_MAX ? UINT16_MAX : capacity;
  if ( !isn_.has_value() ) {
    return { {}, window_size, reassembler_.writer().has_error() };
  }
  return { Wrap32::wrap( checkpoint + reassembler_.writer().is_closed(), isn_.value() ),
           window_size,
           reassembler_.writer().has_error() };
}
