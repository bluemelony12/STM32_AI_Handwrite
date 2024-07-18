# Handwritten digit recognition using STM32 X-CUBE-AI

## Project Outline
This project adds a CNN deep learning model to the firmware based on STM32 X-CUBE-AI,  
recognizes handwritten numbers drawn on the screen, and outputs the results.  
(Displays up to three predicted values, numbered 0 to 9.)  
The development target is stm32F769i disco board and built based on STM32CubeMX and STM32CubeIDE.  

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
├─ BSP
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
├─ Core
│  ├─ Inc
│  ├─ Src
│  └─ Startup
├─ Drivers
│  ├─ CMSIS
│  └─ STM32F7xx_HAL_Driver
├─ HandWriteNumber.tflite
├─ Hand_Write_Number.ipynb
├─ Middlewares
│  └─ ST
│     └─ AI
│        ├─ Inc
├─ Number_Check.ioc
├─ Number_Check.launch
├─ README.md
├─ STM32F769NIHX_FLASH.ld
├─ STM32F769NIHX_RAM.ld
├─ X-CUBE-AI
└─ best-HandWriteNumber.h5
```

## AI Model Information  

(It is my first deep learning model. :sweat_smile:)  

**1. Training Model**  
![image](https://github.com/user-attachments/assets/3fb5eea6-7cc7-44c6-bf0c-28323c572415)

**2. Model Layer** 
```
_________________________________________________________________
 Layer (type)                Output Shape              Param #   
=================================================================
 dense_16 (Dense)            (None, 128)               256       
                                                                 
 dense_17 (Dense)            (None, 128)               16512     
                                                                 
 dense_18 (Dense)            (None, 1)                 129       
                                                                 
=================================================================
Total params: 16,897
Trainable params: 16,897
Non-trainable params: 0
_________________________________________________________________
```

**3. Loss Graph**  
![image](https://github.com/user-attachments/assets/13afed73-c609-4d3f-b510-901820a6da7b)


## How to run the project
1. Execute the .project file for add the project to the CubeIDE
2. Build project. (Target: stm32F769i disco board)
3. Connect the target board and run it.
4. When the program starts, the input value (x_val) is increased by 0.1 from 0 and the predicted output value Sin(y_val) is output to VCP.

## Problem
1. AI modeling input ranges from 0 to 2pi, so any value higher than that will result in an error.




