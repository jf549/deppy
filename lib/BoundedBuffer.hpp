#include <boost/call_traits.hpp>
#include <boost/circular_buffer.hpp>

#include <condition_variable>
#include <mutex>

namespace lib {

  template<typename T>
  class BoundedBuffer {
  public:
    using BufferT = boost::circular_buffer<T>;
    using SizeT = typename BufferT::size_type;
    using ValueT = typename BufferT::value_type;
    using ParamT = typename boost::call_traits<ValueT>::param_type;

    explicit BoundedBuffer(SizeT capacity = 10000) : unread(0), buf(capacity) {}

    BoundedBuffer(const BoundedBuffer&) = delete; // Prevent copying
    BoundedBuffer& operator=(const BoundedBuffer&) = delete; // Prevent assignment

    void produce(ParamT item) {
      std::unique_lock<std::mutex> lock(mutex);
      notFull.wait(lock, [this]{ return unread < buf.capacity(); });
      buf.push_front(item);
      ++unread;
      lock.unlock();
      notEmpty.notify_one();
    }

    ValueT consume() {
      std::unique_lock<std::mutex> lock(mutex);
      notEmpty.wait(lock, [this]{ return unread > 0; });
      ValueT res = buf[--unread];
      lock.unlock();
      notFull.notify_one();
      return res;
    }

  private:
    SizeT unread;
    BufferT buf;
    std::mutex mutex;
    std::condition_variable notFull, notEmpty;
  };

}
