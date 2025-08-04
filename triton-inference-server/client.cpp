#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <cmath>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>
#include <algorithm>

#include "http_client.h"

namespace tc = triton::client;

class TritonClient {
public:
    TritonClient(const std::string& url = "localhost:8000", bool verbose = false) 
        : server_url_(url), verbose_(verbose) {
        // 创建HTTP客户端
        tc::Error err = tc::InferenceServerHttpClient::Create(&client_, server_url_, verbose_);
        if (!err.IsOk()) {
            std::cerr << "❌ 创建客户端失败: " << err << std::endl;
            client_ = nullptr;
        }
    }

    ~TritonClient() {
        if (client_) {
            delete client_;
        }
    }

    bool CheckServerHealth() {
        if (!client_) {
            std::cerr << "❌ 客户端未初始化" << std::endl;
            return false;
        }

        bool live;
        tc::Error err = client_->IsServerLive(&live);
        if (!err.IsOk()) {
            std::cerr << "❌ 连接服务器失败: " << err << std::endl;
            return false;
        }

        if (live) {
            std::cout << "✅ Triton 服务器运行正常" << std::endl;
            return true;
        } else {
            std::cerr << "❌ Triton 服务器未响应" << std::endl;
            return false;
        }
    }

    bool GetModelInfo(const std::string& model_name) {
        if (!client_) return false;

        std::string model_metadata;
        tc::Error err = client_->ModelMetadata(&model_metadata, model_name);
        if (!err.IsOk()) {
            std::cerr << "❌ 获取模型元数据失败: " << err << std::endl;
            return false;
        }

        std::string model_config;
        err = client_->ModelConfig(&model_config, model_name);
        if (!err.IsOk()) {
            std::cerr << "❌ 获取模型配置失败: " << err << std::endl;
            return false;
        }

        std::cout << "\n📋 模型信息: " << model_name << std::endl;
        std::cout << "元数据: " << model_metadata << std::endl;
        std::cout << "配置: " << model_config << std::endl;

        return true;
    }

    bool ListModels() {
        if (!client_) return false;

        std::string repository_index;
        tc::Error err = client_->ModelRepositoryIndex(&repository_index);
        if (!err.IsOk()) {
            std::cerr << "❌ 获取模型列表失败: " << err << std::endl;
            return false;
        }

        std::cout << "\n📦 模型仓库信息:" << std::endl;
        std::cout << repository_index << std::endl;

        return true;
    }

    std::vector<float> GenerateSampleData() {
        std::vector<float> data(20 * 14);
        
        // 使用固定种子以获得可重现的结果
        std::mt19937 gen(42);
        std::normal_distribution<float> dist(0.0f, 1.0f);

        // 生成随机数据
        for (int i = 0; i < 20; ++i) {
            for (int j = 0; j < 14; ++j) {
                float base_value = dist(gen);
                // 添加正弦波模式
                float pattern = std::sin(2.0f * M_PI * i / 20.0f) * (j + 1) * 0.1f;
                data[i * 14 + j] = base_value + pattern;
            }
        }

        return data;
    }

    std::vector<float> Softmax(const std::vector<float>& logits) {
        std::vector<float> result(logits.size());
        
        // 找到最大值以提高数值稳定性
        float max_val = *std::max_element(logits.begin(), logits.end());
        
        // 计算exp和sum
        float sum = 0.0f;
        for (size_t i = 0; i < logits.size(); ++i) {
            result[i] = std::exp(logits[i] - max_val);
            sum += result[i];
        }
        
        // 归一化
        for (size_t i = 0; i < result.size(); ++i) {
            result[i] /= sum;
        }
        
        return result;
    }

    bool PredictWithLabels(const std::string& model_name, 
                          const std::vector<float>& input_data,
                          const std::vector<std::string>& labels = {"bird", "uav"}) {
        if (!client_) return false;

        // 输入数据形状 [1, 20, 14]
        std::vector<int64_t> input_shape = {1, 20, 14};
        
        std::cout << "📥 输入数据形状: [" << input_shape[0] << ", " 
                  << input_shape[1] << ", " << input_shape[2] << "]" << std::endl;
        std::cout << "📥 输入数据大小: " << input_data.size() << std::endl;

        // 准备输入
        tc::InferInput* input;
        tc::Error err = tc::InferInput::Create(&input, "input", input_shape, "FP32");
        if (!err.IsOk()) {
            std::cerr << "❌ 创建输入失败: " << err << std::endl;
            return false;
        }

        std::shared_ptr<tc::InferInput> input_ptr(input);

        // 设置输入数据
        err = input_ptr->AppendRaw(reinterpret_cast<const uint8_t*>(input_data.data()), 
                                   input_data.size() * sizeof(float));
        if (!err.IsOk()) {
            std::cerr << "❌ 设置输入数据失败: " << err << std::endl;
            return false;
        }

        // 准备输出
        tc::InferRequestedOutput* output;
        err = tc::InferRequestedOutput::Create(&output, "output");
        if (!err.IsOk()) {
            std::cerr << "❌ 创建输出失败: " << err << std::endl;
            return false;
        }

        std::shared_ptr<tc::InferRequestedOutput> output_ptr(output);

        // 执行推理
        std::vector<tc::InferInput*> inputs = {input_ptr.get()};
        std::vector<const tc::InferRequestedOutput*> outputs = {output_ptr.get()};

        tc::InferResult* result;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        err = client_->Infer(&result, model_name, inputs, outputs);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;

        if (!err.IsOk()) {
            std::cerr << "❌ 推理失败: " << err << std::endl;
            return false;
        }

        std::shared_ptr<tc::InferResult> result_ptr(result);

        std::cout << "⚡ 推理时间: " << std::fixed << std::setprecision(4) 
                  << inference_time << " 毫秒" << std::endl;

        // 获取输出数据
        const uint8_t* output_buffer;
        size_t output_byte_size;
        err = result_ptr->RawData("output", &output_buffer, &output_byte_size);
        if (!err.IsOk()) {
            std::cerr << "❌ 获取输出数据失败: " << err << std::endl;
            return false;
        }

        // 转换输出数据
        const float* output_data = reinterpret_cast<const float*>(output_buffer);
        size_t output_size = output_byte_size / sizeof(float);

        std::vector<float> raw_output(output_data, output_data + output_size);
        
        std::cout << "📤 输出大小: " << output_size << std::endl;
        std::cout << "📤 原始输出: [";
        for (size_t i = 0; i < raw_output.size(); ++i) {
            std::cout << std::fixed << std::setprecision(4) << raw_output[i];
            if (i < raw_output.size() - 1) std::cout << ", ";
        }
        std::cout << "]" << std::endl;

        // 计算softmax概率
        std::vector<float> probabilities = Softmax(raw_output);

        // 获取预测结果
        auto max_it = std::max_element(probabilities.begin(), probabilities.end());
        int predicted_class = std::distance(probabilities.begin(), max_it);
        float confidence = *max_it;

        std::string predicted_label = (predicted_class < labels.size()) ? 
                                     labels[predicted_class] : 
                                     "Class_" + std::to_string(predicted_class);

        // 显示结果
        std::cout << "\n🎯 预测结果:" << std::endl;
        std::cout << "预测类别: " << predicted_label << std::endl;
        std::cout << "置信度: " << std::fixed << std::setprecision(4) << confidence << std::endl;
        std::cout << "概率分布: ";
        for (size_t i = 0; i < probabilities.size() && i < labels.size(); ++i) {
            std::cout << labels[i] << "=" << std::fixed << std::setprecision(4) 
                      << probabilities[i];
            if (i < std::min(probabilities.size(), labels.size()) - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;

        return true;
    }

private:
    std::string server_url_;
    bool verbose_;
    tc::InferenceServerHttpClient* client_;
};

void PrintUsage(const char* program_name) {
    std::cout << "用法: " << program_name << " [选项]" << std::endl;
    std::cout << "选项:" << std::endl;
    std::cout << "  --url URL          Triton服务器地址 (默认: localhost:8000)" << std::endl;
    std::cout << "  --model MODEL      模型名称 (默认: Times_Classify)" << std::endl;
    std::cout << "  --verbose          启用详细日志" << std::endl;
    std::cout << "  --help             显示此帮助信息" << std::endl;
}

int main(int argc, char** argv) {
    std::string url = "localhost:8000";
    std::string model_name = "Times_Classify";
    bool verbose = false;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        } else if (arg == "--url" && i + 1 < argc) {
            url = argv[++i];
        } else if (arg == "--model" && i + 1 < argc) {
            model_name = argv[++i];
        } else if (arg == "--verbose") {
            verbose = true;
        } else {
            std::cerr << "未知参数: " << arg << std::endl;
            PrintUsage(argv[0]);
            return 1;
        }
    }

    std::cout << "🚀 连接到 Triton 服务器: " << url << std::endl;

    // 创建客户端
    TritonClient client(url, verbose);

    // 检查服务器健康状态
    if (!client.CheckServerHealth()) {
        return 1;
    }

    // 列出模型
    client.ListModels();

    // 获取模型信息
    client.GetModelInfo(model_name);

    // 生成示例数据
    std::cout << "\n🎲 生成示例数据..." << std::endl;
    std::vector<float> data = client.GenerateSampleData();

    std::cout << "📊 输入数据大小: " << data.size() << std::endl;
    std::cout << "📊 数据范围: [" << std::fixed << std::setprecision(4) 
              << *std::min_element(data.begin(), data.end()) << ", "
              << *std::max_element(data.begin(), data.end()) << "]" << std::endl;

    // 执行预测
    std::cout << "\n🔮 开始推理..." << std::endl;
    bool success = client.PredictWithLabels(model_name, data);

    if (success) {
        std::cout << "\n✅ 推理完成!" << std::endl;
    } else {
        std::cerr << "\n❌ 推理失败!" << std::endl;
        return 1;
    }

    return 0;
}
