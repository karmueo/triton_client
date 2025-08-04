#!/bin/bash

# Navigate to the directory containing the docker-compose.yml file
cd "$(dirname "$0")/.."

# Start the Triton Inference Server using Docker Compose
docker compose up -d