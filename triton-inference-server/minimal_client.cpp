#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <curl/curl.h>
#include <json/json.h>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cmath>

// HTTP响应结构
struct HttpResponse {
    std::string data;
    long response_code;
};

// curl写入回调函数
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, HttpResponse* response) {
    size_t total_size = size * nmemb;
    response->data.append(static_cast<char*>(contents), total_size);
    return total_size;
}

class MinimalTritonClient {
public:
    MinimalTritonClient(const std::string& server_url = "http://localhost:8000")
        : server_url_(server_url) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~MinimalTritonClient() {
        curl_global_cleanup();
    }

    bool IsServerLive() {
        std::string url = server_url_ + "/v2/health/live";
        HttpResponse response = HttpGet(url);
        return response.response_code == 200;
    }

    bool GetModelMetadata(const std::string& model_name, Json::Value& metadata) {
        std::string url = server_url_ + "/v2/models/" + model_name;
        HttpResponse response = HttpGet(url);
        
        if (response.response_code != 200) {
            std::cerr << "获取模型元数据失败: HTTP " << response.response_code << std::endl;
            return false;
        }

        Json::Reader reader;
        return reader.parse(response.data, metadata);
    }

    bool Infer(const std::string& model_name, const std::vector<float>& input_data, 
               std::vector<float>& output_data) {
        std::string url = server_url_ + "/v2/models/" + model_name + "/infer";

        // 构建JSON请求
        Json::Value request;
        request["id"] = "inference_request";
        
        // 输入数据
        Json::Value input;
        input["name"] = "input";
        input["shape"] = Json::Value(Json::arrayValue);
        input["shape"].append(1);   // batch_size
        input["shape"].append(20);  // time_steps
        input["shape"].append(14);  // features
        input["datatype"] = "FP32";
        
        // 将float数据转换为base64或直接以数组形式发送
        Json::Value data_array(Json::arrayValue);
        for (float value : input_data) {
            data_array.append(value);
        }
        input["data"] = data_array;
        
        request["inputs"] = Json::Value(Json::arrayValue);
        request["inputs"].append(input);

        // 输出配置
        Json::Value output;
        output["name"] = "output";
        request["outputs"] = Json::Value(Json::arrayValue);
        request["outputs"].append(output);

        // 发送POST请求
        Json::StreamWriterBuilder builder;
        std::string json_string = Json::writeString(builder, request);
        
        HttpResponse response = HttpPost(url, json_string, "application/json");
        
        if (response.response_code != 200) {
            std::cerr << "推理请求失败: HTTP " << response.response_code << std::endl;
            std::cerr << "响应: " << response.data << std::endl;
            return false;
        }

        // 解析响应
        Json::Value response_json;
        Json::Reader reader;
        if (!reader.parse(response.data, response_json)) {
            std::cerr << "解析推理响应失败" << std::endl;
            return false;
        }

        // 提取输出数据
        if (response_json.isMember("outputs") && response_json["outputs"].isArray() && 
            !response_json["outputs"].empty()) {
            
            const Json::Value& outputs = response_json["outputs"][0];
            if (outputs.isMember("data") && outputs["data"].isArray()) {
                const Json::Value& data = outputs["data"];
                output_data.clear();
                for (const auto& value : data) {
                    output_data.push_back(value.asFloat());
                }
                return true;
            }
        }

        std::cerr << "响应中没有找到输出数据" << std::endl;
        return false;
    }

private:
    std::string server_url_;

    HttpResponse HttpGet(const std::string& url) {
        HttpResponse response;
        response.response_code = 0;

        CURL* curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
            }
            curl_easy_cleanup(curl);
        }
        return response;
    }

    HttpResponse HttpPost(const std::string& url, const std::string& data, 
                         const std::string& content_type) {
        HttpResponse response;
        response.response_code = 0;

        CURL* curl = curl_easy_init();
        if (curl) {
            struct curl_slist* headers = nullptr;
            std::string content_type_header = "Content-Type: " + content_type;
            headers = curl_slist_append(headers, content_type_header.c_str());

            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.length());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.response_code);
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        return response;
    }
};

std::vector<float> GenerateSampleData() {
    std::vector<float> data(20 * 14);
    std::mt19937 gen(42);
    std::normal_distribution<float> dist(0.0f, 1.0f);
    
    for (int i = 0; i < 20; ++i) {
        for (int j = 0; j < 14; ++j) {
            float base_value = dist(gen);
            float pattern = std::sin(2.0f * M_PI * i / 20.0f) * (j + 1) * 0.1f;
            data[i * 14 + j] = base_value + pattern;
        }
    }
    return data;
}

std::vector<float> Softmax(const std::vector<float>& logits) {
    std::vector<float> result(logits.size());
    float max_val = *std::max_element(logits.begin(), logits.end());
    
    float sum = 0.0f;
    for (size_t i = 0; i < logits.size(); ++i) {
        result[i] = std::exp(logits[i] - max_val);
        sum += result[i];
    }
    
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] /= sum;
    }
    
    return result;
}

int main() {
    std::string server_url = "http://localhost:8000";
    std::string model_name = "Times_Classify";

    std::cout << "🚀 连接到 Triton 服务器: " << server_url << std::endl;

    MinimalTritonClient client(server_url);

    // 检查服务器状态
    if (!client.IsServerLive()) {
        std::cerr << "❌ Triton 服务器未运行或无法连接" << std::endl;
        return 1;
    }
    std::cout << "✅ Triton 服务器运行正常" << std::endl;

    // 获取模型信息
    Json::Value metadata;
    if (client.GetModelMetadata(model_name, metadata)) {
        std::cout << "📋 模型名称: " << metadata["name"].asString() << std::endl;
        std::cout << "📋 模型平台: " << metadata["platform"].asString() << std::endl;
    }

    // 生成示例数据
    std::cout << "\n🎲 生成示例数据..." << std::endl;
    std::vector<float> input_data = GenerateSampleData();
    std::cout << "📊 输入数据大小: " << input_data.size() << std::endl;

    // 执行推理
    std::cout << "\n🔮 开始推理..." << std::endl;
    std::vector<float> output_data;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    bool success = client.Infer(model_name, input_data, output_data);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    auto inference_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count() / 1000.0;

    if (!success) {
        std::cerr << "❌ 推理失败" << std::endl;
        return 1;
    }

    std::cout << "⚡ 推理时间: " << std::fixed << std::setprecision(4) 
              << inference_time << " 毫秒" << std::endl;

    std::cout << "📤 输出大小: " << output_data.size() << std::endl;
    std::cout << "📤 原始输出: [";
    for (size_t i = 0; i < output_data.size(); ++i) {
        std::cout << std::fixed << std::setprecision(4) << output_data[i];
        if (i < output_data.size() - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;

    // 计算softmax概率
    std::vector<float> probabilities = Softmax(output_data);

    // 获取预测结果
    auto max_it = std::max_element(probabilities.begin(), probabilities.end());
    int predicted_class = std::distance(probabilities.begin(), max_it);
    float confidence = *max_it;

    std::vector<std::string> labels = {"bird", "uav"};
    std::string predicted_label = (predicted_class < labels.size()) ? 
                                 labels[predicted_class] : 
                                 "Class_" + std::to_string(predicted_class);

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

    std::cout << "\n✅ 推理完成!" << std::endl;
    return 0;
}
