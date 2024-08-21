/**
* @author: WTY
* @date: 2024/8/19
* @description: Definition of constants, operations, and header files
*/

#include "RSQ.h"

// 保存随机数R11，方便后续解密
double R11;

// 原始数据集X和Y
vector<vector<double>> data_x, data_y;

// 存储每个数据点X的K个最近邻的距离
vector<vector<double>> distances;

// 密文数据集
vector<vector<VectorXd>> ciphertext;

// 加密矩阵
MatrixXd encryptMatrix;


/**
 * @Method: readDataFromFile
 * @Description: 读取文件中的doubles，并返回一个vector<vector<double>>类型的数据
 * @param char* filename 文件名
 * @return vector<vector<double>> doubles数据
 */
vector<vector<double>> readDataFromFile(const char* filename) {
    vector<vector<double>> data_list;
    ifstream infile(filename);

    if (!infile.is_open()) {
        cerr << "Error opening file" << endl;
        return data_list;
    }

    string line;
    while (getline(infile, line)) {
        vector<double> row;
        stringstream ss(line);
        double number;

        while (ss >> number) {
            row.push_back(number);
        }

        data_list.push_back(row);
    }

    infile.close();
    return data_list;
}

/**
 * @Method: readDataFromFile
 * @Description: 读取文件中指定行的doubles，并返回一个vector<double>类型的数据
 * @param const char* filename 文件名
 * @param int lineNumber 行号
 * @return vector<double> doubles数据
 */
vector<double> readDataFromFile(const char* filename, int lineNumber) {
    ifstream infile(filename);
    string line;
    vector<double> result;
    int currentLine = 0;

    while (getline(infile, line)) {
        if (++currentLine == lineNumber) {
            istringstream iss(line);
            double number;
            while (iss >> number) {
                result.push_back(number);
            }
            break;
        }
    }

    return result;
}

/**
 * @Method: compute_distances
 * @Description: 计算数据集X中每个数据点与数据集Y中所有数据点的距离
 * @param int start 开始位置
 * @param int end 结束位置
 */
void calculateDistances(int start, int end) {
    for (int i = start; i < end; ++i) {
        priority_queue<double> heap;
        for (int j = 0; j < data_y.size(); ++j) {
            double distance_ij = 0;
            for (int k = 0; k < data_x[i].size(); ++k) {
                distance_ij += pow(data_x[i][k] - data_y[j][k], 2);
            }
            if (heap.size() < K_MAX) {
                heap.push(distance_ij);
            } else if (distance_ij < heap.top()) {
                heap.pop();
                heap.push(distance_ij);
            }
        }
        while (!heap.empty()) {
            distances[i][heap.size() - 1] = heap.top();
            heap.pop();
        }
    }
}

/**
 * @Method: encryptData
 * @Description: 对数据集X进行加密
 * @param int start 开始位置
 * @param int end 结束位置
 * @param double r11 随机数R11
 * @param double r12 随机数R12
 * @param double r13 随机数R13
 */
void encryptData(int start, int end, double r11, double r12, double r13) {
    for (int i = start; i < end; ++i) {
        vector<double> t(data_x[i].size() + 5);
        double quadratic_sum = 0;
        for (int j = 0; j < data_x[i].size(); ++j) {
            quadratic_sum += pow(data_x[i][j], 2);
            t[j + 1] = data_x[i][j] * -2 * r11;
        }
        t[data_x[i].size() + 1] = r11;
        t[data_x[i].size() + 2] = r11 * r12;
        t[data_x[i].size() + 3] = r11 * -r12;
        t[data_x[i].size() + 4] = r13;

        for (int k = 0; k < K_MAX; ++k) {
            t[0] = (quadratic_sum - distances[i][k]) * r11;
            VectorXd v = Eigen::Map<VectorXd>(t.data(), t.size());
            v = encryptMatrix.transpose() * v;
            ciphertext[i][k] = v;
        }
    }
}

/**
  * @Method: dealData
  * @Description: 对数据集进行预计算
  * @param char* fileString_x 读取数据集x的地址
  * @param char* fileString_y 读取数据集y的地址
  * @return 状态码，1：成功；0：失败
  */
int dealData(char* fileString_x, char* fileString_y) {
    auto start_time = chrono::high_resolution_clock::now();
    // 读取数据集X和Y
    data_x = readDataFromFile(fileString_x);
    data_y = readDataFromFile(fileString_y);

    // 获取结束时间点
    auto end_time = chrono::high_resolution_clock::now();
    // 计算时间间隔
    chrono::duration<double, milli> total_duration = end_time - start_time;
    // 输出时间间隔
    printf("数据读取的时间是：%f 毫秒\n", total_duration.count());
    fflush(stdout);

    if (data_x.empty() || data_y.empty()) {
        cerr << "Error: Failed to read data" << endl;
        return 0;
    }

    start_time = chrono::high_resolution_clock::now();

    // 初始化距离矩阵
    distances = vector<vector<double>>(data_x.size(), vector<double>(K_MAX));
    // 定义一个优先队列，用于存储距离
    priority_queue<double> heap;

    // for (int i = 0; i < data_x.size(); i++) {
    //     for (int j = 0; j < data_y.size(); j++) {
    //         double distance_ij = 0;
    //         // 计算距离
    //         for (int k = 0; k < data_x[i].size(); k++) {
    //             distance_ij += pow(data_x[i][k] - data_y[j][k], 2);
    //         }
    //         // 维护优先队列的大小为K_MAX
    //         if (heap.size() < K_MAX) {
    //             heap.push(distance_ij);
    //         } else if (distance_ij < heap.top()) {
    //             heap.pop();
    //             heap.push(distance_ij);
    //         }
    //     }
    //     // 将优先队列中的元素存入距离矩阵
    //     while (!heap.empty()) {
    //         distances[i][heap.size() - 1] = heap.top();
    //         heap.pop();
    //     }
    // }

    // int num_threads = thread::hardware_concurrency(); // 获取可用的线程数
    int num_threads = 30; // 设置线程数
    vector<thread> threads;
    int data_size = data_x.size();
    int chunk_size = data_size / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? data_size : start + chunk_size;
        threads.emplace_back(calculateDistances, start, end);
    }

    for (auto& th : threads) {
        th.join();
    }

    end_time = chrono::high_resolution_clock::now();
    total_duration = end_time - start_time;
    // 输出时间间隔
    printf("预处理数据的总时间是：%f 毫秒\n", total_duration.count());
    fflush(stdout);

    start_time = chrono::high_resolution_clock::now();

    // 初始化密文矩阵
    ciphertext = vector<vector<VectorXd>>(data_x.size(), vector<VectorXd>(K_MAX));

    // 生成加密矩阵
    encryptMatrix = generateInvertibleMatrix(data_x[0].size() + 5);

    // 生成三个随机值r11,r12,r13，其中r11 > r13 > 0
    double r11 = generateRandomDouble();
    double r12 = generateRandomDouble();
    double r13 = generateRandomDouble();
    while (r11 <= r13) {
        r11 = generateRandomDouble();
        r13 = generateRandomDouble();
    }
    R11 = r11; // 保存r11的值

    // // 对每个明文数据进行加密
    // for (int i = 0; i < data_x.size(); i++) {
    //     vector<double> t(data_x[i].size() + 5);
    //
    //     // 计算每一维数据的平方和
    //     double quadratic_sum = 0;
    //     for (int j = 0; j < data_x[i].size(); j++) {
    //         quadratic_sum += pow(data_x[i][j], 2);
    //         t[j + 1] = data_x[i][j] * -2 * r11;
    //     }
    //     t[data_x[i].size() + 1] = r11;
    //     t[data_x[i].size() + 2] = r11 * r12;
    //     t[data_x[i].size() + 3] = r11 * -r12;
    //     t[data_x[i].size() + 4] = r13;
    //
    //     // 将vector<double>转换为Eigen::VectorXd
    //     for (int k = 0; k < K_MAX; k++) {
    //         t[0] = (quadratic_sum - distances[i][k]) * r11;
    //         VectorXd v = Eigen::Map<VectorXd>(t.data(), t.size()); // 将vector<double>转换为Eigen::VectorXd
    //         // 加密
    //         v = encryptMatrix.transpose() * v;
    //         ciphertext[i][k] = v; // 将加密后的数据存储到ciphertext中
    //     }
    // }

    vector<thread> threads2;
    data_size = data_x.size();
    chunk_size = data_size / num_threads;

    for (int t = 0; t < num_threads; ++t) {
        int start = t * chunk_size;
        int end = (t == num_threads - 1) ? data_size : start + chunk_size;
        threads2.emplace_back(encryptData, start, end, r11, r12, r13);
    }

    for (auto& th : threads2) {
        th.join();
    }



    end_time = chrono::high_resolution_clock::now();
    total_duration = end_time - start_time;
    // 输出时间间隔
    printf("加密数据的总时间是：%f 毫秒\n", total_duration.count());
    fflush(stdout);
    cout << "--------------------------------------------" << endl;

    return 1;
}

/**
 * @Method: RSQ
 * @Description: 发起查询请求，并返回查询结果
 * @param char* fileString 读取数据的地址
 * @param char* resultFilePath 输出数据的地址
 * @return 状态码，1：成功；0：失败
 */
int RSQ(char* fileString, char* resultFilePath) {
    vector<vector<double>> query_data(2); // 读取查询数据
    query_data[0] = readDataFromFile(fileString, 1);
    query_data[1] = readDataFromFile(fileString, 2);

    // 检查查询数据是否为空
    if (query_data[0].size() == 0 || query_data[1].size() == 0) {
        cerr << "Error: Failed to read data" << endl;
        return 0;
    }

    // 逆矩阵，用于解密
    MatrixXd encryptMatrixInverse = calculateInverseMatrix(encryptMatrix);

    // 将查询数据加密
    vector<double> t(query_data[1].size() + 5);

    // 生成三个随机值r21,r22,r23，其中r21 > r23 > 0
    double r21 = generateRandomDouble();
    double r22 = generateRandomDouble();
    double r23 = generateRandomDouble();
    while (r21 <= r23) {
        r21 = generateRandomDouble();
        r23 = generateRandomDouble();
    }

    // 计算每一维数据的平方和
    double quadratic_sum = 0;
    for (int j = 0; j < query_data[1].size(); j++) {
        quadratic_sum += pow(query_data[1][j], 2);
        t[j + 1] = query_data[1][j] * r21;
    }
    t[0] = r21;
    t[query_data[1].size() + 1] = quadratic_sum * r21;
    t[query_data[1].size() + 2] = r22 * r21;
    t[query_data[1].size() + 3] = r22 * r21;
    t[query_data[1].size() + 4] = -r23;
    VectorXd q = Eigen::Map<VectorXd>(t.data(), t.size()); // 将vector<double>转换为Eigen::VectorXd
    q = encryptMatrixInverse * q; // 将q用逆矩阵进行加密

    vector<int> data_index; // 存储满足条件的数据索引
    int k = query_data[0][0] - 1;

    // 遍历密文数据集ciphertext，寻找满足条件的数据
    for (int i = 0; i < ciphertext.size(); i++) {
        if (ciphertext[i][k].dot(q) <= 0) {
            data_index.push_back(i);
        }
    }

    // 将查询结果写入文件
    ofstream resultFile(resultFilePath);
    if (resultFile.is_open()) {
        for (int i = 0; i < data_index.size(); i++) {
            // 将ciphertext[data_index[i]][0]解密
            VectorXd decryptedVector = ciphertext[data_index[i]][0].transpose() * encryptMatrixInverse;
            for (int j = 1; j < decryptedVector.size() - 4; j++) {
                resultFile << decryptedVector[j] / (-2) / R11 << " "; // 写入文件
            }
            resultFile << endl; // 换行
        }
        resultFile.close(); // 关闭文件
    } else {
        cerr << "Error: Failed to open result file" << endl;
        return 0;
    }
    return 1;
}