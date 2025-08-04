#!/bin/bash

tritonserver \
    --model-repository=../model_repository \
    --disable-auto-complete-config \
    --log-verbose=1