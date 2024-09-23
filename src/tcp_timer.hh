#pragma once
#include <cstdint>

class TCPTimer
{
private:
  uint64_t curr_ms;
  uint64_t RTO_ms;
  bool checkExpired();
  bool is_closed;
  bool is_alarm;

public:
  TCPTimer( /* args */ );
  ~TCPTimer();
  void TimePass( uint64_t pass_time );
  bool IsClosed() { return is_closed; };
  bool IsAlarm() { return is_alarm; };
  void Close() { is_closed = true; };
  void Open( uint64_t RTO )
  {
    is_closed = false;
    RTO_ms = RTO;
    curr_ms = 0;
  }
};
