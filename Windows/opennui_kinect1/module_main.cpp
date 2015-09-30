#include <opennui/opennui.h>
#include <memory>
#include <atomic>
#include <Windows.h>
#include <NuiApi.h> // Kinect 1 header

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
    if (pInterfaceToRelease != NULL)
    {
        pInterfaceToRelease->Release();
        pInterfaceToRelease = NULL;
    }
}

class ms_kinect1: public _OPENNUI opennui_device
{
    DEFINE_OPENNUI_DEVICE(ms_kinect1);
private:
    INuiSensor* pSensor;
    HANDLE colorStream;
    HANDLE depthStream;
    ::std::atomic<_OPENNUI_DEVICE state_type> _state = _OPENNUI_DEVICE state_type::unloaded;
    _OPENNUI body::joint_type ConvertKinect1JointToOpenNuiJoint(NUI_SKELETON_POSITION_INDEX jointType);
};

ms_kinect1::~ms_kinect1()
{
    if (pSensor)
    {
        pSensor->NuiShutdown();
    }
    SafeRelease(pSensor);
}

_OPENNUI body::joint_type ms_kinect1::ConvertKinect1JointToOpenNuiJoint(NUI_SKELETON_POSITION_INDEX jointType)
{
    switch (jointType)
    {
    case  NUI_SKELETON_POSITION_HIP_CENTER:
        return _OPENNUI body::joint_type::spine_base;
    case NUI_SKELETON_POSITION_SPINE:
        return _OPENNUI body::joint_type::spine_mid;
    case NUI_SKELETON_POSITION_SHOULDER_CENTER:
        return _OPENNUI body::joint_type::spine_shoulder;
    case NUI_SKELETON_POSITION_HEAD:
        return _OPENNUI body::joint_type::head;
    case NUI_SKELETON_POSITION_SHOULDER_LEFT:
        return _OPENNUI body::joint_type::shoulder_left;
    case NUI_SKELETON_POSITION_ELBOW_LEFT:
        return _OPENNUI body::joint_type::elbow_left;
    case NUI_SKELETON_POSITION_WRIST_LEFT:
        return _OPENNUI body::joint_type::wrist_left;
    case NUI_SKELETON_POSITION_HAND_LEFT:
        return _OPENNUI body::joint_type::hand_left;
    case NUI_SKELETON_POSITION_SHOULDER_RIGHT:
        return _OPENNUI body::joint_type::shoulder_right;
    case NUI_SKELETON_POSITION_ELBOW_RIGHT:
        return _OPENNUI body::joint_type::elbow_right;
    case NUI_SKELETON_POSITION_WRIST_RIGHT:
        return _OPENNUI body::joint_type::wrist_right;
    case NUI_SKELETON_POSITION_HAND_RIGHT:
        return _OPENNUI body::joint_type::hand_right;
    case NUI_SKELETON_POSITION_HIP_LEFT:
        return _OPENNUI body::joint_type::hip_left;
    case NUI_SKELETON_POSITION_KNEE_LEFT:
        return _OPENNUI body::joint_type::knee_left;
    case NUI_SKELETON_POSITION_ANKLE_LEFT:
        return _OPENNUI body::joint_type::ankle_left;
    case NUI_SKELETON_POSITION_FOOT_LEFT:
        return _OPENNUI body::joint_type::foot_left;
    case NUI_SKELETON_POSITION_HIP_RIGHT:
        return _OPENNUI body::joint_type::hip_right;
    case NUI_SKELETON_POSITION_KNEE_RIGHT:
        return _OPENNUI body::joint_type::knee_right;
    case NUI_SKELETON_POSITION_ANKLE_RIGHT:
        return _OPENNUI body::joint_type::ankle_right;
    case NUI_SKELETON_POSITION_FOOT_RIGHT:
        return _OPENNUI body::joint_type::foot_right;
    default:
        return _OPENNUI body::joint_type::null;
    }
}

_OPENNUI_DEVICE state_type ms_kinect1::open()
{
    INuiSensor* pNuiSensor = NULL;
    this->pSensor = NULL;
    int sensorCount = 0;
    this->_state = _OPENNUI_DEVICE state_type::unknown;
    HRESULT hr = NuiGetSensorCount(&sensorCount);
    if (FAILED(hr))
        return _OPENNUI_DEVICE state_type::error;

    for (int i = 0; i < sensorCount; ++i)
    {
        hr = NuiCreateSensorByIndex(i, &pNuiSensor);
        if (FAILED(hr))
            continue;
        hr = pNuiSensor->NuiStatus();
        if (SUCCEEDED(hr))
        {
            this->pSensor = pNuiSensor;
            break;
        }
        pNuiSensor->Release();
    }

    if (NULL != pSensor)
    {
        hr = pSensor->NuiInitialize(
            NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX
            | NUI_INITIALIZE_FLAG_USES_SKELETON
            | NUI_INITIALIZE_FLAG_USES_COLOR
            | NUI_INITIALIZE_FLAG_USES_AUDIO);

        if (FAILED(hr))
            return _OPENNUI_DEVICE state_type::error;

        hr = pSensor->NuiImageStreamOpen(
            NUI_IMAGE_TYPE_COLOR,
            NUI_IMAGE_RESOLUTION_640x480,
            0,
            2,
            NULL,
            &colorStream);

        if (FAILED(hr))
            return _OPENNUI_DEVICE state_type::error;

        NUI_IMAGE_TYPE type = HasSkeletalEngine(pSensor) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH;

        hr = pSensor->NuiImageStreamOpen(
            type,
            NUI_IMAGE_RESOLUTION_320x240,
            0,
            2,
            NULL,
            &depthStream);

        if (FAILED(hr))
            return _OPENNUI_DEVICE state_type::error;


        hr = pSensor->NuiSkeletonTrackingEnable(NULL, 0);

        if (FAILED(hr))
            return _OPENNUI_DEVICE state_type::error;

    }

    if (NULL == pSensor || FAILED(hr))
    {
        printf("No ready Kinect found!\n");
        return _OPENNUI_DEVICE state_type::error;
    }
    this->_state = _OPENNUI_DEVICE state_type::opened;
    return _OPENNUI_DEVICE state_type::opened;
}

//TODO: implementation
_OPENNUI_DEVICE state_type ms_kinect1::suspend()
{
    return _OPENNUI_DEVICE state_type::error;
}

//TODO: implementation
_OPENNUI_DEVICE state_type ms_kinect1::release()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type ms_kinect1::state() const
{
    return _state;
}

void ms_kinect1::name(_OPENNUI_DEVICE string& dst) const
{
    dst = "Kinect1";
}

void ms_kinect1::vendor(_OPENNUI_DEVICE string& dst) const
{
    dst = "Microsoft";
}

//TODO: implementation
void ms_kinect1::uuid(_OPENNUI_DEVICE string& dst) const
{
    dst = "Undefined";
}

void ms_kinect1::rivision(_OPENNUI_DEVICE string& dst) const
{
    dst = "1.8.0";
}

_OPENNUI bitflag ms_kinect1::databits() const
{
    return (~0) & (
        static_cast<int>(_OPENNUI_DEVICE data_bit::depth) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::color) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::body)  |
        static_cast<int>(_OPENNUI_DEVICE data_bit::hand)
        );
}

void ms_kinect1::get_color_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 640;
    out.height = 480;
    out.channel = 4;
    out.byte_per_pixel = 4;
    return;
}

void ms_kinect1::get_depth_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 320;
    out.height = 240;
    out.channel = 1;
    out.byte_per_pixel = 4;
    return;
}

void ms_kinect1::get_body_tracking_info(_OPENNUI body_tracking_info& out) const
{
    out.maximum_tracking_bodies = NUI_SKELETON_COUNT;
    out.number_of_joints = 20;
    return;
}

bool ms_kinect1::acquire_color_frame(const _OPENNUI byte* dst)
{
    if (pSensor == NULL)
        return false;

    if (this->_state != _OPENNUI_DEVICE state_type::opened)
        return false;

    NUI_IMAGE_FRAME imageFrame;
    bool result = false;
    HRESULT hr = pSensor->NuiImageStreamGetNextFrame(colorStream, 0, &imageFrame);
    if (FAILED(hr))
    {
        return result;
    }
    INuiFrameTexture* pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect(0, &LockedRect, NULL, 0);
#ifdef ENVIRONMENT64
    for (int i = 3; i < LockedRect.size; i += 4)
    {
        *(LockedRect.pBits + i) = 255;
    }
#else
    void* source = LockedRect.pBits;
    __asm                               // Assembler Code To Follow
    {
        mov ecx, 640 * 480 // Set Up A Counter (Dimensions Of Memory Block)
            mov ebx, source               // Points ebx To Our Data
            //mov edx, dest
label :                          // Label Used For Looping

        mov[ebx + 3], 255
            add ebx, 4                   // Moves Through The Data By 3 Bytes
            //add edx, 4
            dec ecx                     // Decreases Our Loop Counter
            jnz label                   // If Not Zero Jump Back To Label
    }
#endif
    if (LockedRect.Pitch != 0)
    {
        memcpy((BYTE*)dst, static_cast<BYTE*>(LockedRect.pBits), LockedRect.size);
        result = true;
    }
    pTexture->UnlockRect(0);
    pSensor->NuiImageStreamReleaseFrame(colorStream, &imageFrame);

    return result;
}

bool ms_kinect1::acquire_depth_frame(const _OPENNUI byte* dst)
{
    if (pSensor == NULL)
        return false;

    if (this->_state != _OPENNUI_DEVICE state_type::opened)
        return false;

    NUI_IMAGE_FRAME imageFrame;
    bool result = false;
    HRESULT hr = pSensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame);
    if (FAILED(hr))
    {
        return result;
    }
    INuiFrameTexture* pTexture = imageFrame.pFrameTexture;
    NUI_LOCKED_RECT LockedRect;
    pTexture->LockRect(0, &LockedRect, NULL, 0);
    if (LockedRect.Pitch != 0)
    {
        memcpy((BYTE*)dst, static_cast<BYTE*>(LockedRect.pBits), LockedRect.size);
        result = true;
    }

    pTexture->UnlockRect(0);
    pSensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);

    return result;
}

bool ms_kinect1::acquire_body_frame(const _OPENNUI byte* dst)
{
    if (pSensor == NULL)
        return false;

    if (this->_state != _OPENNUI_DEVICE state_type::opened)
        return false;

    NUI_SKELETON_FRAME skeletonFrame = { 0 };
    if (FAILED(pSensor->NuiSkeletonGetNextFrame(1, &skeletonFrame)))
    {
        return false;
    }
    pSensor->NuiTransformSmooth(&skeletonFrame, NULL);
    __int32 count = 0;
    int index = 0;
    byte* dest = (byte*)dst;
    memcpy(dest, &count, sizeof(__int32));

    for (int i = 0; i < NUI_SKELETON_COUNT; i++)
    {
        NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;
        if (NUI_SKELETON_TRACKED == trackingState)
        {
            count++;
            memcpy(dest, &count, sizeof(__int32));

            __int32 trackingId = static_cast<__int32>(skeletonFrame.SkeletonData[i].dwTrackingID);
            memcpy(dest + 4 + index, &trackingId, sizeof(__int32)); index += sizeof(__int32);

            *(dest + 4 + index) = true; index++;

            int jointCount = 25;
            memcpy(dest + 4 + index, &jointCount, sizeof(__int32)); index += sizeof(__int32);

            static int jointBlockSize = sizeof(_OPENNUI body::joint);
            // JT
            // TS
            // D * 3
            // F * 4 

            for (int j = 0; j < 25; j++)
            {

                memset(dest + 4 + index +
                       jointBlockSize * j, 0, jointBlockSize);
                memcpy(dest + 4 + index +
                       jointBlockSize * j, &j, sizeof(__int32));
            }

            for (int j = 0; j < 20; j++)
            {

                __int32 jointType = (__int32)ConvertKinect1JointToOpenNuiJoint((NUI_SKELETON_POSITION_INDEX)j);
                if (jointType != -1)
                {
                    __int32 trackingState = (__int32)skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[j];
                    memcpy(dest + 4 + index + jointBlockSize * jointType + sizeof(__int32), &trackingState, sizeof(__int32));

                    double x = skeletonFrame.SkeletonData[i].SkeletonPositions[j].x;
                    memcpy(dest + 4 + index +
                           jointBlockSize * jointType + sizeof(__int32) * 2, &x, sizeof(double));

                    double y = skeletonFrame.SkeletonData[i].SkeletonPositions[j].y;
                    memcpy(dest + 4 + index +
                           jointBlockSize * jointType + sizeof(__int32) * 2 + sizeof(double), &y, sizeof(double));

                    double z = skeletonFrame.SkeletonData[i].SkeletonPositions[j].z;
                    memcpy(dest + 4 + index +
                           jointBlockSize * jointType + sizeof(__int32) * 2 + sizeof(double) * 2, &z, sizeof(double));

                }
            }

            index += jointBlockSize * 25;

            __int16 unknownStatus = (__int16)_OPENNUI body::hand_state::unknown;
            memcpy(dest + 4 + index, &unknownStatus, sizeof(__int16));
            index += sizeof(__int16);
            memcpy(dest + 4 + index, &unknownStatus, sizeof(__int16));
            index += sizeof(__int16);
        }
    }

    return count > 0;
}

__declspec(dllexport) _OPENNUI opennui_device* __stdcall on_load()
{
    ::std::shared_ptr<_OPENNUI opennui_device> device = ::std::make_shared<ms_kinect1>();
    printf("HELLO KINECT1!\n");
    return device.get();
}