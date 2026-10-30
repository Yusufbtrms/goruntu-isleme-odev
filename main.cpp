// Görüntü İşleme Ödevi: Bir dizindeki görüntüleri okuyup 1024x768'e yeniden boyutlandırma.
// OpenCV CUDA ile derlenmişse GPU (cv::cuda::resize), değilse CPU (cv::resize) kullanılır.
//
// Kullanım: ./resize <girdi_dizini> [cikti_dizini]
//   cikti_dizini verilmezse "<girdi_dizini>/resized" kullanılır.

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

// CUDA modülleri derlemede varsa kullan
#if __has_include(<opencv2/core/cuda.hpp>) && __has_include(<opencv2/cudawarping.hpp>)
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudawarping.hpp>
#define USE_CUDA_MODULES 1
#else
#define USE_CUDA_MODULES 0
#endif

namespace fs = std::filesystem;

static const int TARGET_WIDTH = 1024;
static const int TARGET_HEIGHT = 768;

static bool isImageFile(const fs::path& p) {
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    static const std::vector<std::string> exts = {
        ".jpg", ".jpeg", ".png", ".bmp", ".tif", ".tiff", ".webp", ".pgm", ".ppm"};
    return std::find(exts.begin(), exts.end(), ext) != exts.end();
}

static bool cudaAvailable() {
#if USE_CUDA_MODULES
    try {
        return cv::cuda::getCudaEnabledDeviceCount() > 0;
    } catch (const cv::Exception&) {
        return false;
    }
#else
    return false;
#endif
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Kullanim: " << argv[0] << " <girdi_dizini> [cikti_dizini]\n";
        return 1;
    }

    fs::path inputDir = argv[1];
    fs::path outputDir = (argc >= 3) ? fs::path(argv[2]) : inputDir / "resized";

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Hata: dizin bulunamadi: " << inputDir << "\n";
        return 1;
    }
    fs::create_directories(outputDir);

    const bool useGpu = cudaAvailable();
    std::cout << "OpenCV surumu: " << CV_VERSION << "\n";
    std::cout << "Islem birimi : " << (useGpu ? "GPU (CUDA)" : "CPU") << "\n";
    std::cout << "Hedef boyut  : " << TARGET_WIDTH << "x" << TARGET_HEIGHT << "\n\n";

    const cv::Size targetSize(TARGET_WIDTH, TARGET_HEIGHT);
    int processed = 0, failed = 0;

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file() || !isImageFile(entry.path())) continue;

        cv::Mat img = cv::imread(entry.path().string(), cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "Okunamadi: " << entry.path().filename() << "\n";
            ++failed;
            continue;
        }

        cv::Mat resized;
#if USE_CUDA_MODULES
        if (useGpu) {
            cv::cuda::GpuMat gpuSrc, gpuDst;
            gpuSrc.upload(img);
            cv::cuda::resize(gpuSrc, gpuDst, targetSize, 0, 0, cv::INTER_LINEAR);
            gpuDst.download(resized);
        } else {
            cv::resize(img, resized, targetSize, 0, 0, cv::INTER_LINEAR);
        }
#else
        cv::resize(img, resized, targetSize, 0, 0, cv::INTER_LINEAR);
#endif

        fs::path outPath = outputDir / entry.path().filename();
        if (!cv::imwrite(outPath.string(), resized)) {
            std::cerr << "Yazilamadi: " << outPath << "\n";
            ++failed;
            continue;
        }

        std::cout << entry.path().filename().string() << "  " << img.cols << "x" << img.rows
                  << " -> " << resized.cols << "x" << resized.rows << "\n";
        ++processed;
    }

    std::cout << "\nBitti. Basarili: " << processed << ", Basarisiz: " << failed << "\n";
    std::cout << "Ciktilar: " << outputDir << "\n";
    return failed == 0 ? 0 : 2;
}
