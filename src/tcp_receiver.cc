#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  // Your code here.
  if ( message.RST ) {
    RST = message.RST;
    return;
  }

  if ( isn_ == std::nullopt && !message.SYN ) // have not start
  {
    return;
  }
  if ( message.SYN && isn_ == nullopt ) // avoid initialize twice
  {
    isn_ = message.seqno;
  }
  uint64_t abs_SeqNo = message.seqno.unwrap( isn_.value(), checkpoint_ );
  reassembler_.insert( abs_SeqNo, message.payload, message.FIN );
  checkpoint_ += message.sequence_length();
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  uint16_t window_size = static_cast<uint16_t>(reassembler_.writer().bytes_cap() - static_cast<uint16_t>( reader().bytes_buffered()));
  struct TCPReceiverMessage receiver = { std::nullopt, window_size, false };
  if ( isn_ == std::nullopt ) {
    return receiver;
  }
  uint64_t written = writer().bytes_pushed();
  if (reassembler_.writer().bytes_cap() > static_cast<uint16_t>( reader().bytes_buffered()))
  {
    receiver = { Wrap32::wrap( written, isn_.value() ),window_size,RST};
  }
  return receiver;
}
