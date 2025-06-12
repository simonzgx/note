FROM xxx

RUN yum update -y && yum groupinstall "Development Tools" -y 

RUN yum install which cmake rapidjson-devel wget tar vim lrzsz -y

RUN python3 -m pip install distro requests build wheel numpy

RUN yum install python3-devel boost-devel numactl-devel re2-devel libb64-devel openssl-devel libarchive-devel -y

RUN dnf install 'dnf-command(config-manager)' && dnf config-manager --add-repo https://developer.download.nvidia.com/compute/cuda/repos/rhel8/x86_64/cuda-rhel8.repo && \
    dnf install -y datacenter-gpu-manager

ENV PATH=/usr/local/cuda-12/bin:$PATH

ENV LD_LIBRARY_PATH=/usr/local/cuda-12/lib64:$LD_LIBRARY_PATH

RUN git clone https://github.com/triton-inference-server/server.git -b r24.12 && cd server && \
    ./build.py -v --no-container-build --build-dir=`pwd`/build --enable-logging --enable-stats --enable-tracing  \
    --enable-metrics --enable-gpu-metrics --enable-cpu-metrics --build-type=Debug --endpoint=http --endpoint=grpc \
    --enable-gpu --endpoint=http --endpoint=grpc && mv ./build/opt/* /opt && cd ../ && rm -rf server

RUN git clone https://github.com/simonzgx/python_backend.git -b r24.12 && cd python_backend && mkdir build && cd build && \
    cmake -DTRITON_ENABLE_GPU=ON -DTRITON_BACKEND_REPO_TAG=r24.12 -DTRITON_COMMON_REPO_TAG=r24.12 -DTRITON_CORE_REPO_TAG=r24.12 \
    -DPYTHON_INCLUDE_DIR=/usr/include/python3.11 -DPYTHON_EXECUTABLE=/usr/bin/python3 -DPYTHON_LIBRARY=/usr/lib64/libpython3.11.so.1.0 -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/install .. && \
    make -j && make install && mkdir -p /opt/tritonserver/backends/python && cp -rf install/backends/python/* /opt/tritonserver/backends/python && cd ../../ && rm -rf python_backend

RUN wget https://developer.nvidia.com/downloads/compute/machine-learning/tensorrt/10.3.0/tars/TensorRT-10.3.0.26.Linux.x86_64-gnu.cuda-12.5.tar.gz && tar xzvf TensorRT-10.3.0.26.Linux.x86_64-gnu.cuda-12.5.tar.gz && \
    mv TensorRT-10.3.0.26 /usr/local/tensorrt && echo "/usr/local/tensorrt/lib/" >> /etc/ld.so.conf.d/nvidia.conf && echo 'export LD_LIBRARY_PATH=/usr/local/tensorrt/lib:$LD_LIBRARY_PATH' >> ~/.bashrc && \
    echo 'export LIBRARY_PATH=/usr/local/tensorrt/lib:$LIBRARY_PATH' >> ~/.bashrc && echo 'export PATH=/usr/local/tensorrt/bin:$PATH' >> ~/.bashrc && python3 -m pip install /usr/local/tensorrt/python/tensorrt*-cp311* && \
    source ~/.bashrc && rm -f TensorRT-10.3.0.26.Linux.x86_64-gnu.cuda-12.5*

RUN CMAKE_CUDA_ARCHITECTURES="50;52;53;60;61;62;70;72;75;80;86;87;89;90" && git clone https://github.com/triton-inference-server/tensorrt_backend.git -b r24.12 && cd tensorrt_backend/ && mkdir build && cd build && \
    source ~/.bashrc && bash -c "cmake -DTRITON_TENSORRT_INCLUDE_PATHS=/usr/local/tensorrt/include -DTRITON_ENABLE_GPU=ON \
    -DCMAKE_INCLUDE_PATH=/usr/local/tensorrt/include -DNVINFER_LIBRARY=/usr/local/tensorrt/lib/libnvinfer.so -DNVINFER_PLUGIN_LIBRARY=/usr/local/tensorrt/lib/libnvinfer_plugin.so \
    -DCMAKE_INSTALL_PREFIX:PATH=`pwd`/install -DTRITON_BACKEND_REPO_TAG=r24.12 -DTRITON_CORE_REPO_TAG=r24.12 -DTRITON_COMMON_REPO_TAG=r24.12 .." && \
    sed -i 's/100-real;101-real;120-real//' _deps/repo-backend-src/CMakeLists.txt && make install && mv install/backends/tensorrt /opt/tritonserver/backends/ && cd ../../ && rm -rf tensorrt_backend

