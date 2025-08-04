# Triton Inference Server

This project provides a setup for the Triton Inference Server using Docker Compose. The Triton Inference Server is designed to serve machine learning models in production environments, providing a robust and scalable solution for inference.

## Project Structure

- `docker-compose.yml`: Defines the Docker Compose configuration, specifying the Triton Inference Server service, including GPU support, port mappings, and model repository mounting.
- `model_repository/`: Directory for storing models. The `.gitkeep` file ensures this directory is tracked in Git.
- `config/triton.conf`: Configuration file for the Triton server, which may include model loading settings and performance optimization parameters.
- `scripts/start.sh`: Script to start the Triton Inference Server using Docker Compose.
- `scripts/stop.sh`: Script to stop the Triton Inference Server using Docker Compose.

## Getting Started

1. **Clone the Repository**: Clone this repository to your local machine.
2. **Model Repository**: Place your models in the `model_repository` directory.
3. **Configuration**: Modify the `config/triton.conf` file as needed to suit your model serving requirements.
4. **Start the Server**: Run the `scripts/start.sh` script to start the Triton Inference Server.
5. **Stop the Server**: Use the `scripts/stop.sh` script to stop the server when needed.

## Requirements

- Docker
- Docker Compose
- NVIDIA GPU (for GPU support)

## License

This project is licensed under the MIT License. See the LICENSE file for more details.