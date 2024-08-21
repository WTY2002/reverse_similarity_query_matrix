/**
* @author: WTY
* @date: 2024/8/19
* @description: Definition of constants, operations, and header files
*/

#ifndef RSQ_H
#define RSQ_H
#include "Matrix_encryption.h"
#include <queue>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/mman.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <cmath>

// 定义最大堆的大小
const int K_MAX = 5;

// 保存随机数R11，方便后续解密
extern double R11;

// 原始数据集X和Y
extern vector<vector<double>> data_x, data_y;

// 存储每个数据点X的K个最近邻的距离
extern vector<vector<double>> distances;

// 密文数据集
extern vector<vector<VectorXd>> ciphertext;

// 加密矩阵
extern MatrixXd encryptMatrix;


/**
 * @Method: readDataFromFile
 * @Description: 读取文件中的doubles，并返回一个vector<vector<double>>类型的数据
 * @param char* filename 文件名
 * @return vector<vector<double>> doubles数据
 */
vector<vector<double>> readDataFromFile(const char* filename);

/**
 * @Method: readDataFromFile
 * @Description: 读取文件中指定行的doubles，并返回一个vector<double>类型的数据
 * @param const char* filename 文件名
 * @param int lineNumber 行号
 * @return vector<double> doubles数据
 */
vector<double> readDataFromFile(const char* filename, int lineNumber);

/**
  * @Method: dealData
  * @Description: 对数据集进行预计算
  * @param char* fileString_x 读取数据集x的地址
  * @param char* fileString_y 读取数据集y的地址
  * @return 状态码，1：成功；0：失败
  */
int dealData(char* fileString_x, char* fileString_y);

/**
 * @Method: RSQ
 * @Description: 发起查询请求，并返回查询结果
 * @param char* fileString 读取数据的地址
 * @param char* resultFilePath 输出数据的地址
 * @return 状态码，1：成功；0：失败
 */
int RSQ(char* fileString, char* resultFilePath);


#endif //RSQ_H
