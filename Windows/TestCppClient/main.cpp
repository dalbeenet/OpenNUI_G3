#include <iostream>
#include <array>
#include <vee/voost/pipe.h>
#include <conio.h>
using namespace vee;
using namespace voost;

int main()
{
    auto stream = interprocess::win32::create_named_pipe();
    try
    {
        stream->connect("\\\\.\\pipe\\opennuipipe",
                        interprocess::creation_option::open_existing,
                        interprocess::pipe_data_transfer_mode::iomode_message,
                        5000);
        ::std::array<char, 512> buffer;
        buffer.fill(0);
        // For receive welcome message
        uint32_t bytes_transferred = stream->read((byte*)buffer.data(), buffer.size());
        printf("%d bytes transferred from server.\nresponse> %s\n", bytes_transferred, buffer.data());
        while (true)
        {
            buffer.fill(0);
            printf("message> ");
            gets(buffer.data());
            bytes_transferred = stream->write((byte*)buffer.data(), strlen(buffer.data()) + 1);
            printf("%d byte transferred.\n", bytes_transferred);
            buffer.fill(0);
            bytes_transferred = stream->read((byte*)buffer.data(), buffer.size());
            printf("%d bytes transferred from server.\nresponse> %s\n", bytes_transferred, buffer.data());
        }
    }
    catch (vee::exception& e)
    {
        printf("exception occured!\nerror_code: %d\nmessage: %s\n", e.code, e.desc.data());
    }
    printf("press any key to exit...\n");
    _getch();
    return 0;
}