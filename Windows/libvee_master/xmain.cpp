#include <vee/scheduler.h>
#include <conio.h>
#include <iostream>
#define HR for(int i = 0; i < 80; ++i) printf("_")

vee::alram_channel signal;

int receiver()
{
    signal.recv();
    printf("signal received!!\n");
    return 0;
}

int sender()
{
    do
    {
        printf("try send a signal\n");
    }
    while (!signal.try_send());
    return 0;
}

int sum(int a, int b)
{
    //printf("%d + %d = %d\n", a, b, a + b);
    return a + b;
}

struct test_object
{
    test_object()
    {
        printf("%s\n", __FUNCSIG__);
    }
    ~test_object()
    {
        printf("%s\n", __FUNCSIG__);
    }
    test_object(const test_object&)
    {
        printf("%s\n", __FUNCSIG__);
    }
    test_object(test_object&&)
    {
        printf("%s\n", __FUNCSIG__);
    }
    test_object& operator=(test_object&)
    {
        printf("%s\n", __FUNCSIG__);
        return *this;
    }
    test_object& operator=(test_object&&)
    {
        printf("%s\n", __FUNCSIG__);
        return *this;
    }
};

void copy_test(test_object& lvalue)
{
    printf("%s\n", __FUNCSIG__);
}

#include <typeinfo.h>

int xmain()
{
    HR;
    printf("# SIGNAL TEST\n");
    {
        std::thread t1(sender);
        std::thread t2(receiver);
        t1.join();
        t2.join();
    }
    HR;
    printf("# TUPLE UNPACKER TEST\n");
    {
        std::function<int(int, int)> func = sum;
        vee::call_by_tuple(sum, std::make_tuple(1, 2)); // function type
        vee::call_by_tuple(func, std::make_tuple(3, 4)); // std::function type
    }
    HR;
    printf("# FORWARD TEST\n");
    {
        vee::call_by_tuple(copy_test, std::make_tuple(test_object()));
    }
    /*HR;
    printf("# ACTOR TEST\n");
    {
    const int JOB_QUEUE_SIZE = 100;
    vee::actor<int(int, int)> actor(JOB_QUEUE_SIZE);
    vee::delegate<int(int, int)> e;
    e += sum;
    printf("job requested! returns: %d\n", actor.request(e, 1, 2));
    printf("job requested! returns: %d\n", actor.request(e, std::make_tuple(3, 4)));
    printf("job requested! returns: %d\n", actor.request(std::move(e), 1, 2));
    }*/
    HR;
    printf("QUEUE TEST\n");
    {
        vee::syncronized_ringqueue<int> queue(100);
        vee::syncronized_ringqueue<int> result(500);
        auto producer = [&queue]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds::duration(100));
            int cnt = 0;
            for (int i = 0; i < 500; ++i)
            {
                while (!queue.enqueue(i))
                {
                    ++cnt;
                }
            }
            printf("Enqueue failure!!! %d times\n", cnt);
        };
        auto consumer = [&queue, &result]()
        {
            int buf = 0, cnt = 0;
            for (int i = 0; i < 250; ++i)
            {
                while (!queue.dequeue(buf))
                {
                    ++cnt;
                }
                //printf("tid: %d\n", std::this_thread::get_id());
                result.enqueue(buf);
                //printf("value: %d\n", buf);
                buf = -1;
            }
            printf("Dequeue failure!!! %d times\n", cnt);
        };
        std::thread t1(producer);
        std::thread t2(consumer);
        std::thread t3(consumer);
        t1.join();
        t2.join();
        t3.join();
    }
    HR;
    printf("QUEUE COPY TEST\n");
    {
        vee::syncronized_ringqueue<int> B(100);
        for (int i = 0; i < 100; ++i)
            B.enqueue(i);
        vee::syncronized_ringqueue<int> A(B);
        //A = B;
        for (int i = 0; i < 100; ++i)
        {
            int buf;
            A.dequeue(buf);
            printf("value: %d\n", buf);
        }
    }
    //HR;
    //{
    //    printf("SLEEP FUNCTION LOAD TEST\n");
    //    vee::delegate<int(int, int)> e;
    //    auto func = [](int a, int b) -> int
    //    {
    //        std::this_thread::sleep_for(std::chrono::milliseconds::duration(5));
    //        return a - b;
    //    };
    //    auto start = std::chrono::system_clock::now();
    //    std::thread t1([func]()
    //    {
    //        for (int i = 0; i < 1000; ++i)
    //            func(1, 2);
    //    });
    //    std::thread t2([func]()
    //    {
    //        for (int i = 0; i < 1000; ++i)
    //            func(1, 2);
    //    });
    //    t1.join();
    //    t2.join();
    //    auto end = std::chrono::system_clock::now();
    //    std::cout << "테스트 종료: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
    //}
    HR;
    {
        printf("SCHEDULER TEST\n");
        std::atomic<int> counter;
        auto start = std::chrono::system_clock::now();
        {
            // scheduler(초기 액터 갯수, 액터 당 작업 큐 용량)
            vee::scheduler<int(int, int)> scheduler(2, 2000);
            vee::delegate<int(int, int)> e;
            e += [&counter](int a, int b) -> int
            {
                ++counter;
                //std::this_thread::sleep_for(std::chrono::milliseconds::duration(5));
                return a - b;
            };
            for (int i = 0; i < 1000; ++i)
            {
                scheduler.request(e, std::make_tuple(1, 2));
                //std::this_thread::sleep_for(std::chrono::milliseconds::duration(1));
                scheduler.request(e, std::make_tuple(3, 4));
                //std::this_thread::sleep_for(std::chrono::milliseconds::duration(1));
                // 총 수행 시간은 10ms.. 그런데 스레드가 두개 ..
            }
            // (1000 * 10 = 10000) / 2 ms = 5 sec
            auto request_completed = std::chrono::system_clock::now();
            std::cout << "작업 위탁: " << std::chrono::duration_cast<std::chrono::milliseconds>(request_completed - start).count() << std::endl;
        }
        auto end = std::chrono::system_clock::now();
        std::cout << "스케줄러 종료: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << std::endl;
        printf("counter: %d\n", counter);
    }
    HR;
    printf("Press any key to exit!\n");
    getch();
    return 0;
}