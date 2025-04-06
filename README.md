# PhotoPainter_B E-Paper Photo Frame

PhotoPainter_B is an enhanced firmware for the Waveshare PhotoPainter (B) e-paper photo frame. This project enables automatic slideshow functionality with various operation modes, supporting custom image selection methods.

## Features

- Battery-powered e-paper photo frame
- Multiple display modes for different slideshow preferences
- Low power consumption with automatic sleep mode
- Image rotation at fixed intervals (configurable from 1 minute to 24 hours)
- True random image selection in Mode 3 using RP2040 hardware RNG
- Settings configuration via settings.txt file
- Charging status indication with LED
- Low battery protection
- Support for SD card to store images

## Operation Modes

The PhotoPainter_B supports four operation modes:

1. **Mode 0**: Automatically scans the `pic` folder on the SD card, sorts the filenames alphabetically, and displays them sequentially.
2. **Mode 1**: Automatically scans the `pic` folder on the SD card, displays images in the order they are found (unsorted).
3. **Mode 2**: Uses a user-created `fileList.txt` file with custom paths to images. No file limit, allowing for a large collection of images.
4. **Mode 3**: Same as Mode 2, but selects the next image randomly from the `fileList.txt` file using true hardware random number generation.

## Settings

You can now configure the device using a `settings.txt` file on the SD card. If this file doesn't exist, it will be created automatically with default values.

### Settings File Format

The settings.txt file uses a simple key-value format:

```
Mode=3
TimeInterval=720
CurrentIndex=1
```

- **Mode**: Operation mode (0-3) as described above
- **TimeInterval**: Time between image changes in minutes (1-1440, default is 720 = 12 hours)
- **CurrentIndex**: Current position in the image list (automatically updated)

You can edit this file manually to change settings without modifying the code.

### Hardware Configuration

- **Display**: 7.3-inch e-paper display
- **Processor**: Raspberry Pi Pico (RP2040)
- **Storage**: MicroSD card for image storage
- **Power**: Built-in rechargeable battery with USB-C charging

### Timing Configuration

The image rotation interval can now be set directly in the settings.txt file:

```
TimeInterval=720  # 12 hours (in minutes)
```

For shorter intervals:
```
TimeInterval=60   # 1 hour
TimeInterval=5    # 5 minutes
TimeInterval=1    # 1 minute
```

Note: When using very short intervals (e.g., 1 minute), the device should be connected to power. In battery mode, the device powers off between updates and requires hardware support to wake up from RTC alarm.

### Random Image Selection

Mode 3 now uses the RP2040's hardware ring oscillator as a source of true randomness, ensuring a better distribution of random images and avoiding repetition.

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
4. (Optional) Create or modify `settings.txt` to customize operation mode and timing
5. Insert the SD card into the PhotoPainter device
6. Power on the device and enjoy your slideshows

## Troubleshooting

- If the display doesn't update, check the battery level
- If images don't appear, verify the SD card is properly formatted (FAT32 recommended)
- For Mode 2 and 3, make sure the paths in `fileList.txt` are correct
- If settings aren't being saved, check SD card permissions and ensure it's not write-protected
- For 1-minute intervals, keep the device connected to power to ensure it stays active
- Images must be in a compatible format (BMP files work best with this firmware)
- If you experience issues, check the debug output when connected via USB

## Recent Updates

- Added settings.txt file for easy configuration
- Improved random image selection using hardware RNG
- Enhanced mount/unmount handling for more reliable file operations
- Better error recovery and validation for all user inputs
- Improved RTC alarm handling
- Added comprehensive debug logging

## License

This project is available under open-source licensing. Feel free to modify and enhance the code for your own use.
