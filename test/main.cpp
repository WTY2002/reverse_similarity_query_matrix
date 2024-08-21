#include <Matrix_encryption.h>
#include <RSQ.h>

int main()
{
    char* fileString1 = "/root/wty/data1.txt";
    char* fileString2 = "/root/wty/data2.txt";
    char* query = "/root/wty/query.txt";
    char* res = "/root/wty/result.txt";


    auto start_time = chrono::high_resolution_clock::now();

    dealData(fileString1, fileString2); // 预处理数据

    // 获取结束时间点
    auto end_time = chrono::high_resolution_clock::now();
    // 计算时间间隔
    chrono::duration<double, milli> total_duration = end_time - start_time;
    // 输出时间间隔
    printf("数据加密外包的时间是：%f 毫秒\n", total_duration.count());
    fflush(stdout);

    auto start_time2 = chrono::high_resolution_clock::now();

    RSQ(query, res); // 查询

    // 获取结束时间点
    auto end_time2 = chrono::high_resolution_clock::now();
    // 计算时间间隔
    chrono::duration<double, milli> total_duration2 = end_time2 - start_time2;
    // 输出时间间隔
    printf("查询的总时间是：%f 毫秒\n", total_duration2.count());
    fflush(stdout);

    return 0;
}
