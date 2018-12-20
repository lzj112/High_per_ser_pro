#include <sys/eventfd.h>
#include <assert.h>
#include <unistd.h>

#include <iostream>
#include <thread>
using namespace std;

void func(int evfd) 
{
    uint64_t buffer;
    int res;
    while (1) 
    {
        res = read(evfd, &buffer, sizeof(uint64_t));
        assert(res == 8);

        printf("buffer = %lu\n", buffer);
    }
}
int main() 
{
    int evfd = eventfd(2, 1);
    assert(evfd != -1);

    thread t(func, evfd);
    t.detach();

    int res;
    uint64_t buf = 1;
    while (1) 
    {
        res = write(evfd, &buf,sizeof(uint64_t));
        assert(res == 8);
        printf("write = %d\n", res);

        sleep(1);
    }
}