# Handwritten digit recognition using STM32 X-CUBE-AI

## Description
This project aims to use CNN deep learning models in firmware using STM32 X-CUBE-AI.
recognizes handwritten numbers drawn on the screen, and outputs the results.  
development target is stm32F769i disco board and built based on STM32CubeMX and STM32CubeIDE.  

![NumCheck_1](https://github.com/user-attachments/assets/4f72338a-f010-4cee-9248-c8e51eb2fb4e)  

The LCD has a handwriting area, an action button, and a result display area.  
The handwriting area consists of 28 x 28 pixels, and the drawn part is processed as 1 and the undrawn part as 0.  
When you press the Run button, the input handwritten data is organized into one array and goes to the AI input.  
CNN model completes its operation, the results appear on the LCD (three highest predictions, number type: 0 to 9)  

**--Version--**  
STM32CUBE F7 v1.17.2  
STM32CUBE MX 6.11.1 
STM32CUBE IDE 1.15.1  
X-CUBE-AI 9.0.0

## Project Structure
```
STM32_AI_Handwrite
├─ .ai
├─ .cproject
├─ .gitignore
├─ .mxproject
├─ .project -----------------------------> Main project file
├─ BSP ----------------------------------> BSP for LCD and touch screen use
│  ├─ Components
│  │  ├─ nt35510
│  │  ├─ otm8009a
│  │  └─ ts.h
│  ├─ stm32f769i_discovery.c
│  ├─ stm32f769i_discovery.h
│  ├─ stm32f769i_discovery_lcd.c
│  ├─ stm32f769i_discovery_lcd.h
│  ├─ stm32f769i_discovery_sdram.c
│  ├─ stm32f769i_discovery_sdram.h
│  ├─ stm32f769i_discovery_ts.c
│  └─ stm32f769i_discovery_ts.h
├─ Core --------------------------------> Main application code
│  ├─ Inc
│  ├─ Src
│  └─ Startup
├─ Drivers -----------------------------> HAL Driver
│  ├─ CMSIS
│  └─ STM32F7xx_HAL_Driver
├─ HandWriteNumber.tflite --------------> Model converted to TensorFlow Lite
├─ Hand_Write_Number.ipynb -------------> My CNN model code (Jupyter Notebook)
├─ Middlewares
│  └─ ST
│     └─ AI ----------------------------> X-CUBE-AI middleware package
├─ Number_Check.ioc --------------------> CubeMX .ioc File
├─ Number_Check.launch
├─ README.md
├─ STM32F769NIHX_FLASH.ld
├─ STM32F769NIHX_RAM.ld
├─ X-CUBE-AI ---------------------------> X-CUBE-AI generate file
└─ best-HandWriteNumber.h5 
```

## AI Model Information  

**1. Training Dataset**  
Training dataset is TensorFlow's MNIST handwritten digit data set,28x28.  
(ref. https://www.tensorflow.org/datasets/catalog/mnist)  

  
**2. Model Layer** 
```
intput shape = 28, 28, 1
_________________________________________________________________
 Layer (type)                Output Shape              Param #   
=================================================================
 conv2d_10 (Conv2D)          (None, 28, 28, 32)        320       
                                                                 
 max_pooling2d_10 (MaxPoolin  (None, 14, 14, 32)       0         
 g2D)                                                            
                                                                 
 conv2d_11 (Conv2D)          (None, 14, 14, 64)        18496     
                                                                 
 max_pooling2d_11 (MaxPoolin  (None, 7, 7, 64)         0         
 g2D)                                                            
                                                                 
 flatten_5 (Flatten)         (None, 3136)              0         
                                                                 
 dense_12 (Dense)            (None, 100)               313700    
                                                                 
 dropout_7 (Dropout)         (None, 100)               0         
                                                                 
 dense_13 (Dense)            (None, 10)                1010      
                                                                 
=================================================================
Total params: 333,526
Trainable params: 333,526
Non-trainable params: 0
```

**3. Loss Graph**  
![image](https://github.com/user-attachments/assets/8869ef46-2a5e-41eb-aa31-53d2d3d1a0c9)  

## How to run
1. Execute the .project file for add the project to the CubeIDE  
(Merging may be necessary due to differences in program versions.)
3. Build project. (Target: stm32F769i disco board)
4. Connect the target board and RUN it.
5. When the program runs normally, draw a number and press the Run button on the screen to predict the value.  
and Pressing the Clear button, screen and results will be clear

## Preview

https://github.com/user-attachments/assets/13894ae9-26f5-4428-852e-e9b77f4bee48  

## Issue
1. Learning about the number 6 is a bit lacking. :worried:
2. This project is intended for personal study and may be of low quality. :worried::worried:




