#include <opennui/opennui.h>
#include <memory>
#include <atomic>
//#include <thread>
#include <map>
#include <mutex>
#include <OpenNI.h>
#include <NiTE.h>

//TODO: DEF FILE
using namespace openni;
class xtion_pro_live: public _OPENNUI opennui_device
{
    DEFINE_OPENNUI_DEVICE(xtion_pro_live);
private:
    openni::Device device;
    openni::VideoStream colorStream;
    openni::VideoStream depthStream;
    openni::VideoFrameRef depthFrame;
    nite::UserTracker userTracker;
    openni::VideoStream* streams[2];
    _OPENNUI body::joint_type ConvertNiteJointToOpenNuiJoint(nite::JointType type);
    static const int COLOR_WIDTH = 1280;
    static const int COLOR_HEIGHT = 1024;
    ::std::atomic<_OPENNUI_DEVICE state_type> _state = _OPENNUI_DEVICE state_type::unloaded;
};

xtion_pro_live::~xtion_pro_live()
{
    nite::NiTE::shutdown();
    //openni::OpenNI::shutdown();
    printf("Xtion pro shutdown completed!\n");
}

_OPENNUI body::joint_type xtion_pro_live::ConvertNiteJointToOpenNuiJoint(nite::JointType type)
{
    switch (type)
    {
    case nite::JOINT_HEAD:
        return _OPENNUI body::joint_type::head;
    case nite::JOINT_NECK:
        return _OPENNUI body::joint_type::neck;
    case nite::JOINT_LEFT_SHOULDER:
        return _OPENNUI body::joint_type::shoulder_left;
    case nite::JOINT_RIGHT_SHOULDER:
        return _OPENNUI body::joint_type::shoulder_right;
    case nite::JOINT_LEFT_ELBOW:
        return _OPENNUI body::joint_type::elbow_left;
    case nite::JOINT_RIGHT_ELBOW:
        return _OPENNUI body::joint_type::elbow_right;
    case nite::JOINT_LEFT_HAND:
        return _OPENNUI body::joint_type::hand_left;
    case nite::JOINT_RIGHT_HAND:
        return _OPENNUI body::joint_type::hand_right;
    case nite::JOINT_TORSO:
        return _OPENNUI body::joint_type::spine_base;
    case nite::JOINT_LEFT_HIP:
        return _OPENNUI body::joint_type::hip_left;
    case nite::JOINT_RIGHT_HIP:
        return _OPENNUI body::joint_type::hip_right;
    case nite::JOINT_LEFT_KNEE:
        return _OPENNUI body::joint_type::knee_left;
    case nite::JOINT_RIGHT_KNEE:
        return _OPENNUI body::joint_type::knee_right;
    case nite::JOINT_LEFT_FOOT:
        return _OPENNUI body::joint_type::foot_left;
    case nite::JOINT_RIGHT_FOOT:
        return _OPENNUI body::joint_type::foot_right;
    default:
        return _OPENNUI body::joint_type::null;
    }
}

_OPENNUI_DEVICE state_type xtion_pro_live::open()
{
    this->_state = _OPENNUI_DEVICE state_type::unknown;
    openni::Status rc = OpenNI::initialize();
    if (rc != openni::STATUS_OK)
    {
        printf("Initialize failed\n%s\n", OpenNI::getExtendedError());
        return _OPENNUI_DEVICE state_type::error;
    }
    openni::Array<openni::DeviceInfo> deviceList;
    openni::OpenNI::enumerateDevices(&deviceList);
    char* xtionProUri = NULL;
    for (int i = 0; i < deviceList.getSize(); ++i)
    {
        if (std::string(deviceList[i].getName()) == "PS1080")
        {
            xtionProUri = (char*)deviceList[i].getUri();
            break;
        }
    }
    if (xtionProUri == NULL)
        return _OPENNUI_DEVICE state_type::error;

    rc = device.open(xtionProUri);
    if (rc != openni::STATUS_OK)
    {
        printf("Couldn't open device\n%s\n", OpenNI::getExtendedError());
        return _OPENNUI_DEVICE state_type::error;
    }


    nite::Status niteRc;
    nite::NiTE::initialize();
    niteRc = userTracker.create(&device);
    if (niteRc != nite::STATUS_OK)
    {
        printf("Couldn't create user tracker\n");
        return _OPENNUI_DEVICE state_type::error;
    }
    if (!userTracker.isValid())
        return _OPENNUI_DEVICE state_type::error;


    if (device.getSensorInfo(SENSOR_DEPTH) != NULL)
    {
        rc = depthStream.create(device, SENSOR_DEPTH);
        if (rc != openni::STATUS_OK)
        {
            printf("Couldn't create depth stream\n%s\n", OpenNI::getExtendedError());
            return _OPENNUI_DEVICE state_type::error;
        }
        depthStream.start();
    }
    if (!depthStream.isValid())
        return _OPENNUI_DEVICE state_type::error;

    if (device.getSensorInfo(SENSOR_COLOR) != NULL)
    {
        rc = colorStream.create(device, SENSOR_COLOR);
        if (rc != openni::STATUS_OK)
        {
            printf("Couldn't create color stream\n%s\n", OpenNI::getExtendedError());
            return _OPENNUI_DEVICE state_type::error;
        }
        colorStream.start();
    }
    if (!colorStream.isValid())
        return _OPENNUI_DEVICE state_type::error;

    streams[0] = &colorStream;
    streams[1] = &depthStream;

    this->_state = _OPENNUI_DEVICE state_type::opened;
    return _OPENNUI_DEVICE state_type::opened;
}

//TODO: Implementation
_OPENNUI_DEVICE state_type xtion_pro_live::suspend()
{
    return _OPENNUI_DEVICE state_type::error;
}

//TODO: Implementation
_OPENNUI_DEVICE state_type xtion_pro_live::release()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type xtion_pro_live::state() const
{
    return _state;
}

void xtion_pro_live::name(_OPENNUI_DEVICE string& dst) const
{
    dst = "Xtion Pro Live";
}

void xtion_pro_live::vendor(_OPENNUI_DEVICE string& dst) const
{
    dst = "ASUS";
}

//TODO: implementation
void xtion_pro_live::uuid(_OPENNUI_DEVICE string& dst) const
{
    dst = "Undefined";
}

void xtion_pro_live::rivision(_OPENNUI_DEVICE string& dst) const
{
    dst = "OpenNI2 + NiTE2";
}

_OPENNUI bitflag xtion_pro_live::databits() const
{
    return (~0) & (
        static_cast<int>(_OPENNUI_DEVICE data_bit::depth) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::color) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::body)  |
        static_cast<int>(_OPENNUI_DEVICE data_bit::hand)
        );
}

void xtion_pro_live::get_color_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 320;
    out.height = 240;
    out.channel = 4;
    out.byte_per_pixel = 4;
    return;
}

void xtion_pro_live::get_depth_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 320;
    out.height = 240;
    out.channel = 1;
    out.byte_per_pixel = 2;
    return;
}

void xtion_pro_live::get_body_tracking_info(_OPENNUI body_tracking_info& out) const
{
    out.maximum_tracking_bodies = 0;
    out.number_of_joints = 0;
    return;
}

bool xtion_pro_live::acquire_color_frame(const _OPENNUI byte* dst)
{
    if (!colorStream.isValid())
    {
        return false;
    }
    int changedStreamDummy;
    openni::Status rc = OpenNI::waitForAnyStream(streams, 2, &changedStreamDummy, 0);
    if (rc != openni::STATUS_OK)
    {
        return false;
    }
    if (changedStreamDummy == 0)
    {

        openni::VideoFrameRef colorFrame;
        openni::Status	rc = colorStream.readFrame(&colorFrame);

        if (colorFrame.isValid())
        {
            openni::RGB888Pixel* pColor = (openni::RGB888Pixel*)colorFrame.getData();

#ifdef ENVIRONMENT64
            BYTE* dest = (BYTE*)dst;
            for (int i = 0; i < colorFrame.getWidth()*colorFrame.getHeight(); i++)
            {
                dest[i * 4] = pColor[i].b;
                dest[i * 4 + 1] = pColor[i].g;
                dest[i * 4 + 2] = pColor[i].r;
                dest[i * 4 + 3] = 0xFF;
            }
#else
            void* source = const_cast<void*>(colorFrame.getData());
            void* dest = const_cast<unsigned char*>(dst);
            __asm                               // Assembler Code To Follow
            {
                mov ecx, 320 * 240 // Set Up A Counter (Dimensions Of Memory Block)
                    mov ebx, source               // Points ebx To Our Data
                    mov edx, dest
label :                          // Label Used For Looping

                mov al, [ebx + 2]
                    mov[edx + 0], al
                    mov al, [ebx + 1]
                    mov[edx + 1], al
                    mov al, [ebx + 0]
                    mov[edx + 2], al


                    add ebx, 3                   // Moves Through The Data By 3 Bytes
                    add edx, 4
                    dec ecx                     // Decreases Our Loop Counter
                    jnz label                   // If Not Zero Jump Back To Label
            }
#endif
            return true;
        }
    }
    return false;
}

bool xtion_pro_live::acquire_depth_frame(const _OPENNUI byte* dst)
{
    if (!depthStream.isValid())
    {
        return false;
    }
    int changedStreamDummy;
    openni::Status rc = OpenNI::waitForAnyStream(streams, 2, &changedStreamDummy, 0);
    if (rc != openni::STATUS_OK)
        return false;

    if (changedStreamDummy == 1)
    {

        depthStream.readFrame(&depthFrame);
        if (depthFrame.isValid())
        {
            memcpy((void*)dst, depthFrame.getData(), depthFrame.getDataSize());
            return true;
        }

    }

    return false;
}

bool xtion_pro_live::acquire_body_frame(const _OPENNUI byte* dst)
{
    if (!userTracker.isValid())
    {
        return false;
    }
    static std::mutex m;
    if (!m.try_lock())
    {
        return false;
    }
    nite::UserTrackerFrameRef userTrackerFrame;
    nite::Status niteRc = userTracker.readFrame(&userTrackerFrame);

    if (niteRc != nite::STATUS_OK)
    {
        m.unlock();
        return false;
    }

    const nite::Array<nite::UserData>& users = userTrackerFrame.getUsers();
    int index = 0;
    BYTE* dest = (BYTE*)dst;
    int count = 0;
    memcpy(dest, &count, sizeof(__int32));
    for (int i = 0; i < users.getSize(); ++i)
    {
        const nite::UserData& user = users[i];
        if (user.isNew())
        {
            userTracker.startSkeletonTracking(user.getId());
        }
        else if (user.getSkeleton().getState() == nite::SKELETON_TRACKED)
        {
            count++;
            memcpy(dest, &count, sizeof(__int32));

            __int32 trackingId = static_cast<__int32>(user.getId());
            memcpy(dest + 4 + index, &trackingId, sizeof(__int32)); index += sizeof(__int32);
            *(dest + 4 + index) = true; index++;
            static int jointCount = 25;
            memcpy(dest + 4 + index, &jointCount, sizeof(__int32)); index += sizeof(__int32);

            std::map<std::string, nite::SkeletonJoint> named_joints;

            for (int j = 0; j < 25; j++)
            {
                memset(dest + 4 + index +
                       _OPENNUI body::JOINT_BLOCK_LENGTH * j, 0, _OPENNUI body::JOINT_BLOCK_LENGTH);
                memcpy(dest + 4 + index +
                       _OPENNUI body::JOINT_BLOCK_LENGTH * j, &j, sizeof(__int32));
            }
            for (int j = 0; j < 15; j++)
            {

                __int32 jointType = (__int32)ConvertNiteJointToOpenNuiJoint((nite::JointType)j);
                if (jointType != -1)
                {
                    nite::SkeletonJoint joint = user.getSkeleton().getJoint((nite::JointType)j);

                    __int32 trackingState = (__int32)_OPENNUI body::tracking_state::tracked;
                    memcpy(dest + 4 + index + _OPENNUI body::JOINT_BLOCK_LENGTH * jointType + sizeof(__int32), &trackingState, sizeof(__int32));

                    double x = joint.getPosition().x / 100;
                    memcpy(dest + 4 + index +
                           _OPENNUI body::JOINT_BLOCK_LENGTH * jointType + sizeof(__int32) * 2, &x, sizeof(double));

                    double y = joint.getPosition().y / 100;
                    memcpy(dest + 4 + index +
                           _OPENNUI body::JOINT_BLOCK_LENGTH * jointType + sizeof(__int32) * 2 + sizeof(double), &y, sizeof(double));

                    double z = joint.getPosition().z / 100;
                    memcpy(dest + 4 + index +
                           _OPENNUI body::JOINT_BLOCK_LENGTH * jointType + sizeof(__int32) * 2 + sizeof(double) * 2, &z, sizeof(double));
                }
            }
            index += _OPENNUI body::JOINT_BLOCK_LENGTH * 25;

            __int16 unknownStatus = (__int16)_OPENNUI body::hand_state::unknown;
            memcpy(dest + 4 + index, &unknownStatus, sizeof(__int16));
            index += sizeof(__int16);
            memcpy(dest + 4 + index, &unknownStatus, sizeof(__int16));
            index += sizeof(__int16);

        }
    }
    //userTrackerFrame.release();
    m.unlock();
    return true;
}

__declspec(dllexport) ::std::shared_ptr<_OPENNUI opennui_device> __stdcall on_load()
{
    ::std::shared_ptr<_OPENNUI opennui_device> device = ::std::make_shared<xtion_pro_live>();
    return device;
}