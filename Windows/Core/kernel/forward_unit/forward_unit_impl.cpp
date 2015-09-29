#include <kernel/protocol.h>
#include <kernel/forward_unit.h>
#include <kernel/scheduler.h>
#include <boost/pool/singleton_pool.hpp>

namespace kernel {

namespace {

struct tag_color_frame_buffer_pool
{
};
struct tag_depth_frame_buffer_pool
{
};
struct tag_body_frame_buffer_pool
{
};

typedef boost::singleton_pool < tag_color_frame_buffer_pool, protocol::stream_constant::temp_g3_color_max_frame_size > color_frame_pool;
typedef boost::singleton_pool < tag_depth_frame_buffer_pool, protocol::stream_constant::temp_g3_depth_max_frame_size > depth_frame_pool;
typedef boost::singleton_pool < tag_body_frame_buffer_pool,  protocol::stream_constant::temp_g3_body_frame_size >      body_frame_pool;

template <class PoolTy>
struct buffer_deleter
{
    void operator () (unsigned char* p)
    {
        PoolTy::free(p);
    }
};

} // unnamed namespace

::std::shared_ptr<forward_unit> forward_unit::get_instance()
{
    static ::std::shared_ptr<forward_unit> instance(new forward_unit);
    return instance;
}

forward_unit::~forward_unit()
{

}

forward_unit::forward_unit()
{
    _timer = ::vee::voost::timer::create_timer();
    _callback += _on_timer;
    _timer->run(16, _callback);
    puts(__FUNCTION__);
}

void forward_unit::_on_timer(timer_tick tick)
{
    auto device_mgr = device_manager::get_instance();
    ::std::lock_guard<::vee::spin_lock> _guard(device_mgr->_mtx);

    for (auto& it : device_mgr->_modules)
    {
        device_manager::device_module_ptr module = it.second;
        // Acllocates buffers
        ::std::shared_ptr<unsigned char> color_frame_buffer((unsigned char*)color_frame_pool::malloc(), buffer_deleter<color_frame_pool>());
        ::std::shared_ptr<unsigned char> depth_frame_buffer((unsigned char*)depth_frame_pool::malloc(), buffer_deleter<depth_frame_pool>());
        ::std::shared_ptr<unsigned char> body_frame_buffer((unsigned char*)body_frame_pool::malloc(), buffer_deleter<body_frame_pool>());
        std::function<void()> bind = ::std::bind(_poll_and_forward_once, module, color_frame_buffer, depth_frame_buffer, body_frame_buffer);
        scheduler.request(::vee::make_delegate(bind));
    }
}

void forward_unit::_poll_and_forward_once(device_manager::device_module_ptr module, 
                                          ::std::shared_ptr<unsigned char> color_frame_buffer,
                                          ::std::shared_ptr<unsigned char> depth_frame_buffer,
                                          ::std::shared_ptr<unsigned char> body_frame_buffer)
{
    static std::atomic<int64_t> time_stamp = 0;

    // Poll
    bool color_ok = module->opennui_device_instance()->acquire_color_frame(color_frame_buffer.get());
    bool depth_ok = module->opennui_device_instance()->acquire_depth_frame(depth_frame_buffer.get());
    bool body_ok  = module->opennui_device_instance()->acquire_body_frame(body_frame_buffer.get());
    int64_t current_stamp = time_stamp++;
    
    // Forward color frame
    if (color_ok)
    {
        auto destinations = module->color_buffer_table.get_all_value_copies(); // copies pointer value
        for (auto& it : destinations)
        {
            for (int i = 0; i < protocol::stream_constant::shm_buffering_count; i++)
            {
                volatile unsigned int* lock_offset = (volatile unsigned int*)((BYTE*)it->addr() + (protocol::stream_constant::shm_lock_block_size * i));
                if (InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_lock_state, protocol::stream_constant::shm_idle_state) == protocol::stream_constant::shm_idle_state)
                {
                    unsigned char* target = (BYTE*)it->addr() + protocol::stream_constant::shm_lock_section_size + ((module->color_frame_info().size() + protocol::stream_constant::shm_time_stamp_block_size) * i);
                    memmove(target, &current_stamp, protocol::stream_constant::shm_time_stamp_block_size);
                    memmove(target + protocol::stream_constant::shm_time_stamp_block_size, color_frame_buffer.get(), module->color_frame_info().size());
                    InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_idle_state, protocol::stream_constant::shm_lock_state);
                    break;
                }
            }
        }
    }

    // Forward depth frame
    if (depth_ok)
    {
        auto destinations = module->depth_buffer_table.get_all_value_copies(); // copies pointer value
        for (auto& it : destinations)
        {
            for (int i = 0; i < protocol::stream_constant::shm_buffering_count; i++)
            {
                volatile unsigned int* lock_offset = (volatile unsigned int*)((BYTE*)it->addr() + protocol::stream_constant::shm_lock_block_size * i);
                if (InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_lock_state, protocol::stream_constant::shm_idle_state) == protocol::stream_constant::shm_idle_state)
                {
                    unsigned char* target = (BYTE*)it->addr() + protocol::stream_constant::shm_lock_section_size + ((module->depth_frame_info().size() + protocol::stream_constant::shm_time_stamp_block_size) * i);
                    memmove(target, &current_stamp, protocol::stream_constant::shm_time_stamp_block_size);
                    memmove(target + protocol::stream_constant::shm_time_stamp_block_size, depth_frame_buffer.get(), module->depth_frame_info().size());
                    InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_idle_state, protocol::stream_constant::shm_lock_state);
                    break;
                }
            }
        }
    }

    // Forward body frame
    if (body_ok)
    {
        auto destinations = module->body_buffer_table.get_all_value_copies(); // copies pointer value
        for (auto& it : destinations)
        {
            for (int i = 0; i < protocol::stream_constant::shm_buffering_count; i++)
            {
                volatile unsigned int* lock_offset = (volatile unsigned int*)((BYTE*)it->addr() + protocol::stream_constant::shm_lock_block_size * i);
                if (InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_lock_state, protocol::stream_constant::shm_idle_state) == protocol::stream_constant::shm_idle_state)
                {
                    unsigned char* target = (BYTE*)it->addr() + protocol::stream_constant::shm_lock_section_size + ((protocol::stream_constant::temp_g3_body_frame_size + protocol::stream_constant::shm_time_stamp_block_size) * i);
                    memmove(target, &current_stamp, protocol::stream_constant::shm_time_stamp_block_size);
                    memmove(target + protocol::stream_constant::shm_time_stamp_block_size, body_frame_buffer.get(), protocol::stream_constant::temp_g3_body_frame_size);
                    InterlockedCompareExchange(lock_offset, protocol::stream_constant::shm_idle_state, protocol::stream_constant::shm_lock_state);
                    break;
                }
            }
        }
    }
}

} // namespace kernel