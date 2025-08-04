#!/usr/bin/env python3
"""
Triton 客户端示例 - 调用 Times_Classify 模型
"""

import numpy as np
import tritonclient.http as httpclient
import tritonclient.grpc as grpcclient
from tritonclient.utils import triton_to_np_dtype
import argparse
import json
import time


class TritonClient:
    def __init__(self, url="localhost:8000", protocol="http"):
        """
        初始化 Triton 客户端
        
        Args:
            url: Triton 服务器地址
            protocol: 协议类型 ("http" 或 "grpc")
        """
        self.url = url
        self.protocol = protocol
        
        if protocol == "http":
            self.client = httpclient.InferenceServerClient(url=url)
        elif protocol == "grpc":
            self.client = grpcclient.InferenceServerClient(url=url)
        else:
            raise ValueError("协议类型必须是 'http' 或 'grpc'")
    
    def check_server_health(self):
        """检查服务器健康状态"""
        try:
            if self.client.is_server_live():
                print("✅ Triton 服务器运行正常")
                return True
            else:
                print("❌ Triton 服务器未响应")
                return False
        except Exception as e:
            print(f"❌ 连接服务器失败: {e}")
            return False
    
    def get_model_info(self, model_name):
        """获取模型信息"""
        try:
            model_metadata = self.client.get_model_metadata(model_name)
            model_config = self.client.get_model_config(model_name)
            
            print(f"\n📋 模型信息: {model_name}")
            print(f"平台: {model_config['platform']}")
            print(f"最大批处理大小: {model_config['max_batch_size']}")
            
            print("\n输入参数:")
            for input_info in model_metadata['inputs']:
                print(f"  - 名称: {input_info['name']}")
                print(f"    数据类型: {input_info['datatype']}")
                print(f"    维度: {input_info['shape']}")
            
            print("\n输出参数:")
            for output_info in model_metadata['outputs']:
                print(f"  - 名称: {output_info['name']}")
                print(f"    数据类型: {output_info['datatype']}")
                print(f"    维度: {output_info['shape']}")
            
            return model_metadata, model_config
        except Exception as e:
            print(f"❌ 获取模型信息失败: {e}")
            return None, None
    
    def list_models(self):
        """列出所有可用模型"""
        try:
            models = self.client.get_model_repository_index()
            print("\n📦 可用模型:")
            for model in models:
                print(f"  - {model['name']} (状态: {model['state']})")
            return models
        except Exception as e:
            print(f"❌ 获取模型列表失败: {e}")
            return []
    
    def prepare_input_data(self, data, input_name="input", batch_size=1):
        """
        准备输入数据
        
        Args:
            data: 输入数据 (numpy array 或 list)
            input_name: 输入名称
            batch_size: 批处理大小
        """
        if isinstance(data, list):
            data = np.array(data, dtype=np.float32)
        
        # 确保数据是 float32 类型
        if data.dtype != np.float32:
            data = data.astype(np.float32)
        
        # 如果数据没有批处理维度，添加批处理维度
        if len(data.shape) == 2 and data.shape[0] == 20 and data.shape[1] == 14:
            data = np.expand_dims(data, axis=0)  # 添加批处理维度
        
        # 重复数据以匹配批处理大小
        if batch_size > 1:
            data = np.repeat(data, batch_size, axis=0)
        
        print(f"📥 输入数据形状: {data.shape}")
        print(f"📥 输入数据类型: {data.dtype}")
        
        # 创建输入对象
        if self.protocol == "http":
            inputs = [httpclient.InferInput(input_name, data.shape, "FP32")]
            inputs[0].set_data_from_numpy(data)
        else:
            inputs = [grpcclient.InferInput(input_name, data.shape, "FP32")]
            inputs[0].set_data_from_numpy(data)
        
        return inputs
    
    def prepare_outputs(self, output_names=["output"]):
        """准备输出配置"""
        if self.protocol == "http":
            outputs = [httpclient.InferRequestedOutput(name) for name in output_names]
        else:
            outputs = [grpcclient.InferRequestedOutput(name) for name in output_names]
        
        return outputs
    
    def infer(self, model_name, inputs, outputs):
        """执行推理"""
        try:
            start_time = time.time()
            
            # 执行推理
            results = self.client.infer(
                model_name=model_name,
                inputs=inputs,
                outputs=outputs
            )
            
            inference_time = time.time() - start_time
            
            # 获取输出结果
            output_data = results.as_numpy('output')
            
            print(f"⚡ 推理时间: {inference_time:.4f} 秒")
            print(f"📤 输出形状: {output_data.shape}")
            print(f"📤 输出数据类型: {output_data.dtype}")
            
            return output_data, inference_time
            
        except Exception as e:
            print(f"❌ 推理失败: {e}")
            return None, None
    
    def predict_with_labels(self, model_name, data, labels=["bird", "uav"]):
        """
        执行预测并返回带标签的结果
        
        Args:
            model_name: 模型名称
            data: 输入数据
            labels: 类别标签列表
        """
        # 准备输入数据
        inputs = self.prepare_input_data(data)
        outputs = self.prepare_outputs()
        
        # 执行推理
        output_data, inference_time = self.infer(model_name, inputs, outputs)
        
        if output_data is not None:
            # 应用 softmax 获取概率
            probabilities = self.softmax(output_data[0])  # 取第一个批次的结果
            
            # 获取预测类别
            predicted_class = np.argmax(probabilities)
            predicted_label = labels[predicted_class] if predicted_class < len(labels) else f"Class_{predicted_class}"
            confidence = probabilities[predicted_class]
            
            print(f"\n🎯 预测结果:")
            print(f"预测类别: {predicted_label}")
            print(f"置信度: {confidence:.4f}")
            print(f"原始输出: {output_data[0]}")
            print(f"概率分布: {probabilities}")
            
            return {
                "predicted_class": predicted_class,
                "predicted_label": predicted_label,
                "confidence": float(confidence),
                "probabilities": probabilities.tolist(),
                "raw_output": output_data[0].tolist(),
                "inference_time": inference_time
            }
        
        return None
    
    @staticmethod
    def softmax(x):
        """计算 softmax"""
        exp_x = np.exp(x - np.max(x))  # 减去最大值以提高数值稳定性
        return exp_x / np.sum(exp_x)


def generate_sample_data():
    """生成示例数据 (20x14 的时间序列数据)"""
    # 生成随机的时间序列数据
    np.random.seed(42)  # 为了结果可重现
    data = np.random.randn(20, 14).astype(np.float32)
    
    # 添加一些模式使其更像真实的时间序列数据
    for i in range(14):
        data[:, i] += np.sin(np.linspace(0, 2*np.pi, 20)) * (i + 1) * 0.1
    
    return data


def main():
    parser = argparse.ArgumentParser(description="Triton Times_Classify 客户端")
    parser.add_argument("--url", default="localhost:8000", help="Triton 服务器地址")
    parser.add_argument("--protocol", default="http", choices=["http", "grpc"], help="协议类型")
    parser.add_argument("--model", default="Times_Classify", help="模型名称")
    parser.add_argument("--data-file", help="输入数据文件路径 (numpy .npy 文件)")
    parser.add_argument("--batch-size", type=int, default=1, help="批处理大小")
    
    args = parser.parse_args()
    
    # 创建客户端
    print(f"🚀 连接到 Triton 服务器: {args.url} (协议: {args.protocol})")
    client = TritonClient(url=args.url, protocol=args.protocol)
    
    # 检查服务器健康状态
    if not client.check_server_health():
        return
    
    # 列出可用模型
    client.list_models()
    
    # 获取模型信息
    model_metadata, model_config = client.get_model_info(args.model)
    if model_metadata is None:
        return
    
    # 准备输入数据
    if args.data_file:
        try:
            data = np.load(args.data_file)
            print(f"📁 从文件加载数据: {args.data_file}")
        except Exception as e:
            print(f"❌ 加载数据文件失败: {e}")
            return
    else:
        print("🎲 生成示例数据...")
        data = generate_sample_data()
    
    print(f"📊 输入数据形状: {data.shape}")
    print(f"📊 数据范围: [{data.min():.4f}, {data.max():.4f}]")
    
    # 执行预测
    print(f"\n🔮 开始推理...")
    result = client.predict_with_labels(args.model, data)
    
    if result:
        print(f"\n✅ 推理完成!")
        print(f"📈 结果摘要:")
        print(json.dumps(result, indent=2, ensure_ascii=False))
    else:
        print("❌ 推理失败")


if __name__ == "__main__":
    main()
