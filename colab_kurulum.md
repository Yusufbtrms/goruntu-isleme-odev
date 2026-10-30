# Google Colab'da CUDA Destekli OpenCV Kurulumu ve Çalıştırma

Her kod bloğu Colab'da ayrı bir hücre olarak çalıştırılır.

## 0. GPU'yu aç

Menüden **Çalışma zamanı > Çalışma zamanı türünü değiştir > T4 GPU** seç.

## 1. GPU ve CUDA kontrolü

```
!nvidia-smi
!nvcc --version
!nvidia-smi --query-gpu=name,compute_cap --format=csv
```

Son komuttaki `compute_cap` değerini not al (T4 için 7.5). Aşağıdaki `CUDA_ARCH_BIN` değerine yazılacak.

## 2. Gerekli paketler

```
!apt-get update -qq
!apt-get install -y -qq cmake build-essential pkg-config git \
    libjpeg-dev libpng-dev libtiff-dev libgtk2.0-dev
```

## 3. OpenCV ve opencv_contrib kaynak kodunu indir

CUDA modülleri (`cudawarping` vb.) `opencv_contrib` içinde olduğu için ikisi de gerekli.

```
%cd /content
!git clone --depth 1 --branch 4.10.0 https://github.com/opencv/opencv.git
!git clone --depth 1 --branch 4.10.0 https://github.com/opencv/opencv_contrib.git
```

## 4. CMake ile yapılandır

`CUDA_ARCH_BIN` değerini 1. adımda gördüğün `compute_cap` ile değiştir.

```
%cd /content/opencv
!mkdir -p build
%cd /content/opencv/build
!cmake -D CMAKE_BUILD_TYPE=RELEASE \
       -D CMAKE_INSTALL_PREFIX=/usr/local \
       -D OPENCV_EXTRA_MODULES_PATH=/content/opencv_contrib/modules \
       -D WITH_CUDA=ON \
       -D CUDA_ARCH_BIN=7.5 \
       -D WITH_CUDNN=OFF \
       -D OPENCV_DNN_CUDA=OFF \
       -D ENABLE_FAST_MATH=ON \
       -D CUDA_FAST_MATH=ON \
       -D BUILD_opencv_python3=OFF \
       -D BUILD_TESTS=OFF \
       -D BUILD_PERF_TESTS=OFF \
       -D BUILD_EXAMPLES=OFF \
       ..
```

Çıktıda `NVIDIA CUDA: YES` yazdığını kontrol et.

## 5. Derle ve kur (yaklaşık 30-60 dakika sürer)

```
!make -j$(nproc)
!make install
!ldconfig
```

Çalışma zamanı kapanırsa kurulum silinir, tekrar yapman gerekir. Uzun sürmemesi için derleme bitince `/usr/local` içeriğini Google Drive'a kopyalayıp sonra geri yükleyebilirsin.

## 6. Kurulumu doğrula

```
!pkg-config --modversion opencv4
!pkg-config --libs opencv4 | tr ' ' '\n' | grep -i cuda
```

## 7. Ödev kodunu yükle ve derle

Sol taraftaki dosya paneline `main.cpp` dosyasını sürükle (ya da `files.upload()` kullan), sonra:

```
%cd /content
!g++ main.cpp -o resize -std=c++17 $(pkg-config --cflags --libs opencv4)
```

## 8. Görüntüleri hazırla ve çalıştır

```
!mkdir -p /content/images
# Görüntüleri sol panelden /content/images içine yükle
!./resize /content/images /content/images_resized
```

Program başta `Islem birimi : GPU (CUDA)` yazdırıyorsa CUDA çalışıyor demektir. Çıktı görüntüleri `/content/images_resized` içinde 1024x768 olarak oluşur.

## Yerelde (VS Code, NVIDIA GPU varsa)

Aynı `main.cpp` dosyası değişmeden çalışır. CUDA'lı OpenCV kurmak yerine normal OpenCV ile de derlenir, o durumda program otomatik CPU'ya geçer:

```
g++ main.cpp -o resize -std=c++17 `pkg-config --cflags --libs opencv4`
./resize ./images ./images_resized
```
