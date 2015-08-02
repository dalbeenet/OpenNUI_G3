/******************************************************************* 
FileName: datadef.h
Author  : Seyeon Oh(vee)
Date    : 2015-08-02 11:45:13
Desc    : Data type definition of OpenNUI
*******************************************************************/ 

#ifndef _OPENNUI_SDK_DATADEF_H_
#define _OPENNUI_SDK_DATADEF_H_
#include "sdkver.h"
#include <string>

_OPENNUI_BEGIN

struct video_frame_info
{
    unsigned int width   = 0;
    unsigned int height  = 0;
    unsigned int channel = 0;
    unsigned int byte_per_pixel = 0;
    inline unsigned int buffer_pitch() const
    {
        return width * height;
    }
    inline unsigned int size() const
    {
        return width * height * byte_per_pixel * channel;
    }
    inline unsigned int bit_depth() const
    {
        return (byte_per_pixel * 8) / channel;
    }
};

struct body_tracking_info
{
    unsigned int number_of_joints = 0;
    unsigned int maximum_tracking_bodies = 0;
};

#define _OPENNUI_INTERFACE = 0
class opennui_device abstract
{
public:
    using string  = std::string;
    using bitflag  = int;
    enum class state: int
    {
        unloaded = 0,
        unknown,
        opened,
        suspended,
        error
    };
    enum class data_bit: int
    {
        depth = 0x01,
        color = depth << 1,
        body  = color << 1,
        hand  = body  << 1,
        audio = hand  << 1,
        etc   = audio << 1,
    };
    enum class frame_type: int
    {
        color = 0,
        depth,
        body,
        hand,
        audio,
        etc,
    };
    virtual ~opennui_device() = default;
    virtual void   on_load()  _OPENNUI_INTERFACE;
    virtual state  open()     _OPENNUI_INTERFACE;
    virtual state  suspend()  _OPENNUI_INTERFACE;
    virtual state  release()  _OPENNUI_INTERFACE;
    virtual state  state()     const _OPENNUI_INTERFACE;
    virtual string name()      const _OPENNUI_INTERFACE;
    virtual string vendor()    const _OPENNUI_INTERFACE;
    virtual string uuid()      const _OPENNUI_INTERFACE;
    virtual string rivision()  const _OPENNUI_INTERFACE;
    virtual bitflag databits() const _OPENNUI_INTERFACE;
    virtual void get_color_frame_info(video_frame_info& out)     const _OPENNUI_INTERFACE;
    virtual void get_depth_frame_info(video_frame_info& out)     const _OPENNUI_INTERFACE;
    virtual void get_body_tracking_info(body_tracking_info& out) const _OPENNUI_INTERFACE;
    virtual bool acquire_color_frame(const unsigned char* dst) _OPENNUI_INTERFACE;
    virtual bool acquire_depth_frame(const unsigned char* dst) _OPENNUI_INTERFACE;
    virtual bool acquire_body_frame(const unsigned char* dst)  _OPENNUI_INTERFACE;
};

#define _OVERRIDE_OPENNUI_INTERFACE override
#define DEFINE_OPENNUI_DEVICE(class_name) \
    public:\
        virtual ~##class_name()   _OVERRIDE_OPENNUI_INTERFACE;\
        virtual void   on_load()  _OVERRIDE_OPENNUI_INTERFACE;\
        virtual state  open()     _OVERRIDE_OPENNUI_INTERFACE;\
        virtual state  suspend()  _OVERRIDE_OPENNUI_INTERFACE;\
        virtual state  release()  _OVERRIDE_OPENNUI_INTERFACE;\
        virtual state  state()     const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual string name()      const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual string vendor()    const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual string uuid()      const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual string rivision()  const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual bitflag databits() const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual void get_color_frame_info(video_frame_info& out)     const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual void get_depth_frame_info(video_frame_info& out)     const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual void get_body_tracking_info(body_tracking_info& out) const _OVERRIDE_OPENNUI_INTERFACE;\
        virtual bool acquire_color_frame(const unsigned char* dst) _OVERRIDE_OPENNUI_INTERFACE;\
        virtual bool acquire_depth_frame(const unsigned char* dst) _OVERRIDE_OPENNUI_INTERFACE;\
        virtual bool acquire_body_frame(const unsigned char* dst)  _OVERRIDE_OPENNUI_INTERFACE;

_OPENNUI_END

#endif // _OPENNUI_SDK_DATADEF_H_
