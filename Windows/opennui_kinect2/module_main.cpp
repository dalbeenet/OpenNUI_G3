#include <opennui/opennui.h>
#include <atomic>
#include <memory>
#include <iostream>
#include <Kinect.h>

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

class ms_kinect2: public _OPENNUI opennui_device
{
    DEFINE_OPENNUI_DEVICE(ms_kinect2);
private:
    IColorFrameReader*  pColorReader;
	IFrameDescription*  pColorDescription;
	IColorFrameSource*  pColorSource;
	IDepthFrameReader*  pDepthReader;
	IFrameDescription*  pDepthDescription;
	IDepthFrameSource*  pDepthSource;
	IBodyFrameReader*   pBodyReader;
	IBodyFrameSource*   pBodySource;
	IKinectSensor*      pSensor;
    ::std::atomic<_OPENNUI_DEVICE state_type> _state = _OPENNUI_DEVICE state_type::unloaded;
};

ms_kinect2::~ms_kinect2()
{
}

_OPENNUI_DEVICE state_type ms_kinect2::open()
{
    if (_state != _OPENNUI_DEVICE state_type::unloaded)
    {
        return _OPENNUI_DEVICE state_type::opened;
    }

    // Sensor
    HRESULT hResult = S_OK;
    hResult = GetDefaultKinectSensor(&pSensor);
    if (FAILED(hResult))
    {
        std::cerr << "Error : GetDefaultKinectSensor" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }

    hResult = pSensor->Open();
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::Open()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }

    // Color
    hResult = pSensor->get_ColorFrameSource(&pColorSource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_ColorFrameSource()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    hResult = pColorSource->OpenReader(&pColorReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IColorFrameSource::OpenReader()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }

    hResult = pColorSource->get_FrameDescription(&pColorDescription);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IColorFrameSource::get_FrameDescription()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    SafeRelease(pColorSource);

    // Depth
    hResult = pSensor->get_DepthFrameSource(&pDepthSource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_DepthFrameSource()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    hResult = pDepthSource->OpenReader(&pDepthReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IDepthFrameSource::OpenReader()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    SafeRelease(pDepthSource);
    // Body
    hResult = pSensor->get_BodyFrameSource(&pBodySource);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IKinectSensor::get_BodyFrameSource()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    hResult = pBodySource->OpenReader(&pBodyReader);
    if (FAILED(hResult))
    {
        std::cerr << "Error : IBodyFrameSource::OpenReader()" << std::endl;
        return _OPENNUI_DEVICE state_type::error;
    }
    SafeRelease(pBodySource);
    _state = _OPENNUI_DEVICE state_type::opened;
    
    return _OPENNUI_DEVICE state_type::opened;
}

//TODO: implementation
_OPENNUI_DEVICE state_type ms_kinect2::suspend()
{
    return _OPENNUI_DEVICE state_type::error;
}

//TODO: implementation
_OPENNUI_DEVICE state_type ms_kinect2::release()
{
    return _OPENNUI_DEVICE state_type::error;
}

_OPENNUI_DEVICE state_type ms_kinect2::state() const
{
    return _state;
}

void ms_kinect2::name(_OPENNUI_DEVICE string& dst) const
{
    dst = "Kinect2";
}

void ms_kinect2::vendor(_OPENNUI_DEVICE string& dst) const
{
    dst = "Microsoft";
}

//TODO: implementation
void ms_kinect2::uuid(_OPENNUI_DEVICE string& dst) const
{
    dst = "Undefined";
}

void ms_kinect2::rivision(_OPENNUI_DEVICE string& dst) const
{
    dst = "1409";
}

_OPENNUI bitflag ms_kinect2::databits() const
{
    return (~0) & (
        static_cast<int>(_OPENNUI_DEVICE data_bit::depth) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::color) |
        static_cast<int>(_OPENNUI_DEVICE data_bit::body)  |
        static_cast<int>(_OPENNUI_DEVICE data_bit::hand)
        );
}

void ms_kinect2::get_color_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 1920;
    out.height = 1080;
    out.channel = 4;
    out.byte_per_pixel = 4;
    return;
}

void ms_kinect2::get_depth_frame_info(_OPENNUI video_frame_info& out) const
{
    out.width = 512;
    out.height = 424;
    out.byte_per_pixel = 2;
    out.channel = 1;
    return;
}

void ms_kinect2::get_body_tracking_info(_OPENNUI body_tracking_info& out) const
{
    out.maximum_tracking_bodies = BODY_COUNT;
    out.number_of_joints = 100;
    return;
}

bool ms_kinect2::acquire_color_frame(const _OPENNUI byte* dst)
{
    bool result = false;
    IColorFrame* pColorFrame = nullptr;
    static unsigned int bufferSize = 1920 * 1080 * 4 * sizeof(unsigned char);

    HRESULT hResult = S_OK;
    hResult = pColorReader->AcquireLatestFrame(&pColorFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pColorFrame->CopyConvertedFrameDataToArray(bufferSize, (BYTE*)dst, ColorImageFormat::ColorImageFormat_Rgba);
        if (SUCCEEDED(hResult))
            result = true;
    }
    SafeRelease(pColorFrame);

#ifdef ENVIRONMENT64
    BYTE* dest = (BYTE*)dst;
    for (int i = 0; i < 1920 * 1080; ++i)
    {
        BYTE r = *(dest + (i * 4));
        BYTE b = *(dest + (i * 4) + 2);
        *(dest + (i * 4)) = b;
        *(dest + (i * 4) + 2) = r;
    }
#else
    void* source = (BYTE*)dst;
    __asm                             
    {
        mov ecx, 1920 * 1080    // Set Up A Counter (Dimensions Of Memory Block)
            mov ebx, source     // Points ebx To Our Data
label :
        mov al, [ebx + 0]
            mov ah, [ebx + 2]
            mov[ebx + 2], al
            mov[ebx + 0], ah
            add ebx, 4
            dec ecx             // Decreases Our Loop Counter
            jnz label           // If Not Zero Jump Back To Label
    }
#endif

    return result;
}

bool ms_kinect2::acquire_depth_frame(const _OPENNUI byte* dst)
{
    bool result = false;
    IDepthFrame* pDepthFrame = NULL;
    static unsigned int bufferSize = 512 * 424;
    HRESULT hResult = S_OK;

    hResult = pDepthReader->AcquireLatestFrame(&pDepthFrame);
    if (SUCCEEDED(hResult))
    {
        hResult = pDepthFrame->CopyFrameDataToArray(bufferSize, (UINT16*)dst);
        if (SUCCEEDED(hResult))
            result = true;
    }
    SafeRelease(pDepthFrame);

    return result;
}

bool ms_kinect2::acquire_body_frame(const _OPENNUI byte* dst)
{
    unsigned char* dest = const_cast<unsigned char*>(dst);
    bool result = false;
    IBodyFrame* pBodyFrame = nullptr;
    HRESULT hr = pBodyReader->AcquireLatestFrame(&pBodyFrame);
    if (FAILED(hr))
    {
        if (pBodyFrame != NULL)
        {
            pBodyFrame->Release();
            pBodyFrame = NULL;
        }
        return false;
    }

    IBody* ppBodies[BODY_COUNT] = { 0 };
    hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
    if (SUCCEEDED(hr))
    {
        int index = 0;
        __int32 count = 0;
        BOOLEAN isTracked = FALSE;
        memcpy(dest, &count, sizeof(__int32));
        for (__int32 i = 0; i < _countof(ppBodies); ++i)
        {

            if (ppBodies[i] != NULL && SUCCEEDED(ppBodies[i]->get_IsTracked(&isTracked)) && isTracked == TRUE)
            {
                Joint pJoints[JointType_Count];
                hr = ppBodies[i]->GetJoints(_countof(pJoints), pJoints);

                if (SUCCEEDED(hr))
                {
                    result = true;
                    count++;
                    memcpy(dest, &count, sizeof(__int32));

                    UINT64 trackedId = -1;
                    hr = ppBodies[i]->get_TrackingId(&trackedId);
                    __int32 trackingId = static_cast<__int32>(trackedId);
                    memcpy(dest + 4 + index, &trackingId, sizeof(__int32));
                    index += sizeof(__int32);

                    *(dest + 4 + index) = true;
                    index++;
                    __int32 jointCount = JointType_Count;
                    memcpy(dest + 4 + index, &jointCount, sizeof(__int32));
                    index += sizeof(__int32);

                    for (int j = 0; j < _countof(pJoints); ++j)
                    {
                        _OPENNUI body::joint joint;
                        joint.type = (_OPENNUI body::joint_type)pJoints[j].JointType;
                        joint.state = (_OPENNUI body::tracking_state)pJoints[j].TrackingState;
                        joint.position.x = pJoints[j].Position.X;
                        joint.position.y = pJoints[j].Position.Y;
                        joint.position.z = pJoints[j].Position.Z;
                        memcpy(dest + 4 + index, &joint, sizeof(_OPENNUI body::joint));
                        index += sizeof(_OPENNUI body::joint);
                    }

                    HandState leftHand = HandState_Unknown;
                    HandState rightHand = HandState_Unknown;
                    ppBodies[i]->get_HandLeftState(&leftHand);
                    ppBodies[i]->get_HandRightState(&rightHand);
                    __int16 leftHandStatus = leftHand;
                    memcpy(dest + 4 + index, &leftHandStatus, sizeof(__int16));
                    index += sizeof(__int16);

                    __int16 rightHandStatus = rightHand;
                    memcpy(dest + 4 + index, &rightHandStatus, sizeof(__int16));
                    index += sizeof(__int16);

                }
            }
        }
    }

    for (int i = 0; i < _countof(ppBodies); ++i)
    {
        SafeRelease(ppBodies[i]);
    }

    SafeRelease(pBodyFrame);

    return result;
}

__declspec(dllexport) ::std::shared_ptr<_OPENNUI opennui_device> __stdcall on_load()
{
    ::std::shared_ptr<_OPENNUI opennui_device> device = ::std::make_shared<ms_kinect2>();
    return device;
}