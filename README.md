# ev1527Decoder
this project decodes ev1527 messages by using rxb13 receiverfor stm32f103 microcontroller.



for hardware specifications please review ev1527DecoderF103.ioc


project specifications :


use timer2 at 36mhz clock. if you want to use another timer please modify these two functions in ev1527 library:


uint16_t ev1527_GetTick(void) ; void ev1527_ResetTick(void);


use function ev1527_Init to init the library and call ev1527_Process() in your main loop. when a valid data received, your callback function


that you introduce it to the library through ev1527_Init(), would be called, so you can use the data. in the main.c there is an example.
