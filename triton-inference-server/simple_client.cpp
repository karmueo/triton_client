#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <iomanip>

#include "http_client.h"

namespace tc = triton::client;

int main() {
    std::string server_url = "localhost:8000";
    std::string model_name = "Times_Classify";

    std::cout << "🚀 连接到 Triton 服务器: " << server_url << std::endl;

    // 创建HTTP客户端
    tc::InferenceServerHttpClient* client;
    tc::Error err = tc::InferenceServerHttpClient::Create(&client, server_url, false);
    if (!err.IsOk()) {
        std::cerr << "❌ 创建客户端失败: " << err << std::endl;
        return 1;
    }

    // 检查服务器状态
    bool live;
    err = client->IsServerLive(&live);
    if (!err.IsOk() || !live) {
        std::cerr << "❌ Triton 服务器未运行" << std::endl;
        delete client;
        return 1;
    }
    std::cout << "✅ Triton 服务器运行正常" << std::endl;

    // 生成示例数据 (20x14)
    std::vector<float> input_data(20 * 14);
    std::mt19937 gen(42);
    std::normal_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 20 * 14; ++i) {
        input_data[i] = dist(gen);
    }

    std::cout << "输入数据大小: " << input_data.size() << std::endl;

    // 准备输入
    std::vector<int64_t> input_shape = {1, 20, 14};  // 批次大小=1
    tc::InferInput* input;
    err = tc::InferInput::Create(&input, "input", input_shape, "FP32");
    if (!err.IsOk()) {
        std::cerr << "❌ 创建输入失败: " << err << std::endl;
        delete client;
        return 1;
    }

    err = input->AppendRaw(reinterpret_cast<const uint8_t*>(input_data.data()),
                          input_data.size() * sizeof(float));
    if (!err.IsOk()) {
        std::cerr << "❌ 设置输入数据失败: " << err << std::endl;
        delete input;
        delete client;
        return 1;
    }

    // 准备输出
    tc::InferRequestedOutput* output;
    err = tc::InferRequestedOutput::Create(&output, "output");
    if (!err.IsOk()) {
        std::cerr << "❌ 创建输出失败: " << err << std::endl;
        delete input;
        delete client;
        return 1;
    }

    // 执行推理
    std::vector<tc::InferInput*> inputs = {input};
    std::vector<const tc::InferRequestedOutput*> outputs = {output};

    tc::InferResult* result;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    err = client->Infer(&result, model_name, inputs, outputs);
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;

    if (!err.IsOk()) {
        std::cerr << "❌ 推理失败: " << err << std::endl;
        delete output;
        delete input;
        delete client;
        return 1;
    }

    // 获取输出数据
    const uint8_t* output_buffer;
    size_t output_byte_size;
    err = result->RawData("output", &output_buffer, &output_byte_size);
    if (!err.IsOk()) {
        std::cerr << "❌ 获取输出数据失败: " << err << std::endl;
        delete result;
        delete output;
        delete input;
        delete client;
        return 1;
    }

    const float* output_data = reinterpret_cast<const float*>(output_buffer);
    size_t output_size = output_byte_size / sizeof(float);

    std::cout << "输出大小: " << output_size << std::endl;
    std::cout << "原始输出: [" << output_data[0] << ", " << output_data[1] << "]" << std::endl;

    // 计算softmax概率
    float max_val = std::max(output_data[0], output_data[1]);
    float exp0 = std::exp(output_data[0] - max_val);
    float exp1 = std::exp(output_data[1] - max_val);
    float sum = exp0 + exp1;
    
    float prob_bird = exp0 / sum;
    float prob_uav = exp1 / sum;

    // 获取预测结果
    int predicted_class = (prob_bird > prob_uav) ? 0 : 1;
    std::string predicted_label = (predicted_class == 0) ? "bird" : "uav";
    float confidence = (predicted_class == 0) ? prob_bird : prob_uav;

    std::cout << "\n🎯 预测结果:" << std::endl;
    std::cout << "预测类别: " << predicted_label << std::endl;
    std::cout << "置信度: " << std::fixed << std::setprecision(4) << confidence << std::endl;
    std::cout << "概率分布: bird=" << std::fixed << std::setprecision(4) << prob_bird 
              << ", uav=" << std::fixed << std::setprecision(4) << prob_uav << std::endl;

    // 清理资源
    delete result;
    delete output;
    delete input;
    delete client;

    return 0;
}
