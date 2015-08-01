#ifndef _OPENNUI_SDK_DATADEF_H_
#define _OPENNUI_SDK_DATADEF_H_
#include "sdkver.h"

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

_OPENNUI_END

#endif // _OPENNUI_SDK_DATADEF_H_
