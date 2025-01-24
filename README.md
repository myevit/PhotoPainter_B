# PhotoPainter (B)

## Waveshare Electronics
waveshare electronics</br>
![waveshare_logo.png](waveshare_logo.png)

---

## 中文说明

本例程为 PhotoPainter (B) 的示例代码，友情开源，有bug欢迎反馈。

相关资料地址：  
[PhotoPainter - Waveshare Wiki](https://www.waveshare.net/wiki/PhotoPainter)  

## 图片制作

一、使用 Photoshop 制作散点图，然后导入到 SD 卡。参考教程：  
[E-Paper-Floyd-Steinberg - Waveshare Wiki](https://www.waveshare.net/wiki/E-Paper-Floyd-Steinberg)  

三、使用我们提供的六色抖动图片转换工具。下载链接：  
[PhotoPainter_B-BMP.zip](https://www.waveshare.net/w/upload/3/37/PhotoPainter_B-BMP.zip)  

### 图片转换工具使用

#### Windows 系统

- **单张图片转换**  
  将`图片`拖到 `convert.exe` 上即可完成转换。

- **批量转换图片**  
  将`图片`、`convert.exe` 和 `converterTo6color_all.cmd` 放在同一个文件夹中，然后双击 `converterTo6color_all.cmd` 文件即可进行批量转换。

#### MAC 系统

将`图片`、`convert`、`converterTo6color_all` 放在一个文件夹

打开终端，进入对应的文件夹
> cd xxxx/xxx/xxx/

- **单张图片转换**  
  输入命令，给予 `convert` 可执行权限
  >sudo chmod +x convert
  
  转换图片
  >./convert 图片名称

- **批量转换图片**  
  要先对 `convert` 文件进行权限操作给予后才行
  
  输入命令，给予 `converterTo6color_all.sh` 可执行权限
  >sudo chmod +x converterTo6color_all.sh

  转换图片
  >./converterTo6color_all.sh

---

## English

This example code is for PhotoPainter (B), which is open-sourced in a friendly manner. If you find any bugs, please feel free to report them.

Related documentation can be found at:  
[PhotoPainter - Waveshare Wiki](https://www.waveshare.com/wiki/PhotoPainter)  

## Image Preparation

1.Using Photoshop to create a dithered image and then importing it to an SD card. For a tutorial, please refer to:  
[E-Paper-Floyd-Steinberg - Waveshare Wiki](https://www.waveshare.com/wiki/E-Paper_Floyd-Steinberg)  

3.Using our provided 6-color dithering image conversion tool. Download link:  
[PhotoPainter_B-BMP.zip](https://www.waveshare.net/w/upload/3/37/PhotoPainter_B-BMP.zip)  

### How to Use the Image Conversion Tool

#### For Windows

- **Single Image Conversion**  
  Drag the image onto `convert.exe` to complete the conversion.

- **Batch Image Conversion**  
  Place the `images`, `convert.exe`, and `converterTo6color_all.cmd` in the same folder. Double-click the `converterTo6color_all.cmd` file to perform batch conversion.

#### For MAC

Place the `images`, `convert` and `converterTo6color_all` in the same folder.

Open the terminal and navigate to the corresponding folder:
> cd xxxx/xxx/xxx/

- **Single Image Conversion**  
  Grant execution permission to `convert`:
  >sudo chmod +x convert
  
  Convert the image:
  >./convert image_name

- **Batch Image Conversion**  
  You need to first grant execution permissions to the `convert` file.

  Grant execution permission to `converterTo6color_all.sh`:
  >sudo chmod +x converterTo6color_all.sh

  Convert the images:
  >./converterTo6color_all.sh

---
