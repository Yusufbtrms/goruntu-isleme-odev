# Görüntü İşleme Ödevi: Görüntüleri Yeniden Boyutlandırma

Bir dizindeki görüntü dosyalarını okuyup 1024x768 çözünürlüğüne yeniden boyutlandıran C++ programı (OpenCV).
OpenCV CUDA ile derlenmişse işlem GPU'da (`cv::cuda::resize`), değilse CPU'da (`cv::resize`) yapılır.

## Dosyalar

- `main.cpp`: Ana program
- `colab_kurulum.md`: Google Colab'da CUDA destekli OpenCV kurulum adımları

## Derleme

```
g++ main.cpp -o resize -std=c++17 $(pkg-config --cflags --libs opencv4)
```

OpenCV 5 kuruluysa `opencv4` yerine `opencv5` yazın.

## Çalıştırma

```
./resize <girdi_dizini> [cikti_dizini]
```

Çıktı dizini verilmezse `<girdi_dizini>/resized` kullanılır.

Örnek:

```
./resize images images_resized
```

## Desteklenen formatlar

jpg, jpeg, png, bmp, tif, tiff, webp, pgm, ppm
