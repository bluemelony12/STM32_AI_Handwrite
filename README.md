# Handwritten digit recognition using STM32 X-CUBE-AI

## Description
This project adds a CNN deep learning model to the firmware based on STM32 X-CUBE-AI,  
recognizes handwritten numbers drawn on the screen, and outputs the results.  
(Displays up to three predicted values, numbered 0 to 9.)  
development target is stm32F769i disco board and built based on STM32CubeMX and STM32CubeIDE.  

![NumCheck_1](https://github.com/user-attachments/assets/4f72338a-f010-4cee-9248-c8e51eb2fb4e)  

**--Version--**  
STM32CUBE F7 v1.17.1  
STM32CUBE IDE 1.15.1  
X-CUBE-AI 9.0.0

## Project Structure
```
STM32_AI_Handwrite
├─ .ai
├─ .cproject
├─ .gitignore
├─ .mxproject
├─ .project
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
├─ Hand_Write_Number.ipynb -------------> Custom CNN model
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
training dataset was TensorFlow's MNIST handwritten digit data set,28x28.  
(ref. https://www.tensorflow.org/datasets/catalog/mnist?hl=ko)  

  
**2. Model Layer** 
```
intput shape = 28 x 28 x 1
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

## How to run the project
1. Execute the .project file for add the project to the CubeIDE
2. Build project. (Target: stm32F769i disco board)
3. Connect the target board and run it.
4. When the program starts, the input value (x_val) is increased by 0.1 from 0 and the predicted output value Sin(y_val) is output to VCP.

## Running


## Problem
1. AI modeling input ranges from 0 to 2pi, so any value higher than that will result in an error.




