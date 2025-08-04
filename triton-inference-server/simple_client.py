#!/usr/bin/env python3
"""
简单的 Triton 客户端示例
"""

import numpy as np
import tritonclient.http as httpclient


def simple_inference():
    """简单的推理示例"""
    
    # 连接到 Triton 服务器
    url = "localhost:8000"
    client = httpclient.InferenceServerClient(url=url)
    
    # 检查服务器状态
    if not client.is_server_live():
        print("❌ Triton 服务器未运行")
        return
    
    print("✅ Triton 服务器运行正常")
    
    # 模型名称
    model_name = "Times_Classify"
    
    # 准备输入数据 (20x14 的时间序列数据)
    input_data = np.random.randn(1, 20, 14).astype(np.float32)  # 批次大小=1
    print(f"输入数据形状: {input_data.shape}")
    
    # 创建输入对象
    inputs = [httpclient.InferInput("input", input_data.shape, "FP32")]
    inputs[0].set_data_from_numpy(input_data)
    
    # 创建输出对象
    outputs = [httpclient.InferRequestedOutput("output")]
    
    try:
        # 执行推理
        results = client.infer(
            model_name=model_name,
            inputs=inputs,
            outputs=outputs
        )
        
        # 获取输出
        output = results.as_numpy("output")
        print(f"输出形状: {output.shape}")
        print(f"原始输出: {output[0]}")
        
        # 计算 softmax 概率
        exp_output = np.exp(output[0] - np.max(output[0]))
        probabilities = exp_output / np.sum(exp_output)
        
        # 获取预测结果
        predicted_class = np.argmax(probabilities)
        confidence = probabilities[predicted_class]
        
        # 类别标签
        labels = ["bird", "uav"]
        predicted_label = labels[predicted_class]
        
        print(f"\n🎯 预测结果:")
        print(f"预测类别: {predicted_label}")
        print(f"置信度: {confidence:.4f}")
        print(f"概率分布: bird={probabilities[0]:.4f}, uav={probabilities[1]:.4f}")
        
    except Exception as e:
        print(f"❌ 推理失败: {e}")


if __name__ == "__main__":
    simple_inference()
