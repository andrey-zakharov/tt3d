#ifndef TIMER_HPP_
#define TIMER_HPP_
class Timer {
 public:
  virtual bool  Period() const      = 0;
  virtual int   Timeout() const     = 0;
  virtual void  Expired()           = 0;
};
#endif  // TIMER_HPP_
