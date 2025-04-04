# PhotoPainter_B E-Paper Photo Frame

PhotoPainter_B is an enhanced firmware for the Waveshare PhotoPainter (B) e-paper photo frame. This project enables automatic slideshow functionality with various operation modes, supporting custom image selection methods.

## Features

- Battery-powered e-paper photo frame
- Multiple display modes for different slideshow preferences
- Low power consumption with automatic sleep mode
- Image rotation at fixed intervals (default 12 hours)
- Charging status indication with LED
- Low battery protection
- Support for SD card to store images

## Operation Modes

The PhotoPainter_B supports four operation modes:

1. **Mode 0**: Automatically scans the `pic` folder on the SD card, sorts the filenames alphabetically, and displays them sequentially.
2. **Mode 1**: Automatically scans the `pic` folder on the SD card, displays images in the order they are found (unsorted).
3. **Mode 2**: Uses a user-created `fileList.txt` file with custom paths to images. No file limit, allowing for a large collection of images.
4. **Mode 3**: Same as Mode 2, but selects the next image randomly from the `fileList.txt` file instead of sequentially.

You can change the default mode by modifying the `Mode_Value` define in the `main.c` file:

```c
#define Mode_Value 0  // Change to 0, 1, 2, or 3 as needed
```

## Settings

### Hardware Configuration

- **Display**: 7.3-inch e-paper display
- **Processor**: Raspberry Pi Pico (RP2040)
- **Storage**: MicroSD card for image storage
- **Power**: Built-in rechargeable battery with USB-C charging

### Timing Configuration

The default image rotation interval is set to 12 hours. You can modify this by changing the alarm time settings in `main.c`:

```c
// Change the interval by modifying one of these values
alarmTime.hours += 12;    // Change for hours
// alarmTime.minutes += 5;   // Uncomment for minutes
// alarmTime.seconds += 10;  // Uncomment for seconds
```

### Battery Protection

The device will automatically power off when the battery voltage falls below 3.1V:

```c
if(measureVBAT() < 3.1) {   // Low battery threshold
    // Power off sequence
}
```

## File Structure

For Mode 0 and Mode 1, place your images in the `pic` folder at the root of the SD card.

For Mode 2 and Mode 3, create a file named `fileList.txt` at the root of the SD card with each line containing a path to an image file, for example:

```
pic/image1.bmp
pic/image2.bmp
pic/subfolder/image3.bmp
```

## Image Conversion

For the best display quality on the e-paper screen, images should be properly converted. Please use the dedicated image conversion tool from:

[PhotoPainter_image_converter](https://github.com/myevit/PhotoPainter_image_converter)

This tool provides:
- Optimal image formatting for e-ink displays
- Orientation filtering (portrait/landscape)
- Image enhancement options (brightness, contrast, saturation)
- Optional date display on images
- Support for multiple image formats including HEIC
- Batch processing with progress indication

## Usage

1. Convert your images using the [PhotoPainter_image_converter](https://github.com/myevit/PhotoPainter_image_converter)
2. Copy the converted images to the SD card's `pic` folder
3. If using Mode 2 or 3, create a `fileList.txt` file listing the images
4. Insert the SD card into the PhotoPainter device
5. Power on the device and enjoy your slideshows

## Troubleshooting

- If the display doesn't update, check the battery level
- If images don't appear, verify the SD card is properly formatted (FAT32 recommended)
- For Mode 2 and 3, make sure the paths in `fileList.txt` are correct
- Images must be in a compatible format (BMP files work best with this firmware)

## License

This project is available under open-source licensing. Feel free to modify and enhance the code for your own use.
