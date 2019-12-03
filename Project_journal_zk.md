# Project Journal  
  

I have started my project journal on GitHub today. The plan for this week in terms of 
the project is to finalize my project idea, discuss it with my supervisor and few other 
lecturers and write up a proposal to be submitted by Sunday 6th.


##### 07/10/2019  
During my last week I have setup the **MCUXpresso and FreeRTOS** and tested everything compiles
and works. I have created and submitted the project proposal.
//Plan for this week is to get SystemView working with K64F to monitor the FreeRTOS and to/
start working on the moisture detection. This will likely be an external interrupt.


##### 14/10/2019  
I have order two K64F development boards off Brian today along with a **J-Link EDU** debugging
probe that will help me to monitor the behaviour of the FreeRTOS using **SystemView**. Waiting
for confirmation from Brian that the order is OK and expected delivery times. I have also 
acquired 2 Bluetooth modules to get the wireless communication going. Didn't get to work on
the boards at all unfortunately but giving myself this week the get the external interrupt 
going and start working on the RTC for timekeeping. Will probably have to talk to Paul or 
Niall about the design of the FreeRTOS system and it's tasks

##### 25/10/2019   
Last week I have been working on the development of the software for K64F. I still haven't 
received any components but at the moment I am not dependent on them. So I got the external
interrupt working that is suppose to happen when moisture is detected and at the moment
this event is simulated by pushing a button on the development board. I have also worked on
development of the RTC to allow timekeeping which was a bit of a struggle as the example
provided by NXP in their software development kit wouldn't work properly for me and I was
running into all sorts of errors when I was trying to compile it or include snippets of it
in my code. Got it working eventually but thinking of re-designing the RTOS as at the moment
seems somewhat cumbersome.

##### 1/11/2019   
Since my last contribution to this journal a number of things have happened. I have managed to
get the **RTC** working using the **Peripheral config tool** of MCUXpresso IDE. This worked fine
and I did commit my changes to my branch back then. However, upon merging my branch to 
master it stopped working and I wasn't at that time able to fix it. So in my desperation
I have reverted the merge thus creating a crazy mess in my repository. While thinking of
how to fix this I kept working on my project and got the RTC up and running again. In order
to clean the mess up, I ended up copying all my code from the newest commit and applying it
directly to master branch. This was followed by removal of the used branch. I think thanks to
this, there might be some history issues. But the repository is now working again and I am 
continuing to develop more code.
I have also received my two K64Fs along with J-Link EDU programmer so I have started working
with those. I have successfully integrated **SystemView** into my code and am able to continuously
monitor the behaviour of the FreeRTOS running on my MCU.

#### 13/11/2019
I have been stuck on running the **J-Link** along with the **UART0** that is connected to **OpenSDA USB**
port on K64F. I have been playing with the code, trying various things or workarounds such as using the 
**JLinkRTTViewer** to substitute the UART functionality. Got most of it working however was unable to
obtain user input correctly. After some investigation I opted for a solution that would allow the
JLink to work with the OpenSDA port. This solution involved **cutting some traces on the board** and 
I was quite nervous about doing so but it seems to have worked so I started to revert to the UART
code I have previously implemented. I intend to get this to work quite quickly so I can close and
merge the userTimeConfig branch and move onto the **Bluetooth** communication. For this I have installed
the necessary headers onto the board but have yet to test it's functionality.

#### 23/11/2019
I have finaly managed to implement user time configuration task. One of the issues I haven't considered was
the lack of memory assigned to the freertos task. Increasing this, helped and I can now successfully 
adjust the time and date, including simple error checking for those. I was able to reuse some code from
my other embedded systems project that I have written. During my attempts I have disabled the UART0 interrupt
so my menu doesn't work at the moment, but will get it working quickly. I have started playing with the 
Bluetooth module and will be trying to get it working also. I will have to enable UART4 for this and 
took me a while to realize that I need to somehow communicate with the module through it. Wasn't sure
how to do this at first but luckily I have obtained a USBtoTTL module a while back for my own projects so I
think that this will be perfect for the communication. Once I have established the connection between the board
and the BT module I will start working on the second device and try to receive communication wirelessly.

#### 29/11/2019  
I have been working on getting the Bluetooth module to work with K64F. But wasn't successful so far. I can connect
to the module from my phone and it even generates an interrupt but that is as far as it gets. On top of it
nothing gets printed into the terminal anymore for an unknow reason. Worst case scenario, I can revert to master
just before the bluetooth module started to be implemented. I have also realized
that I will need a different module for the main device (the one I am currently working on) as the one 
I am currently using cannot be configured in **master mode** and communication between two slaves isn't possible.
Should however receive this module soon so will start implementing it's functionality then. I can still however
keep trying to get communication going between my phone and the current bluetooth device as reconfiguration of
the new device shouldn't be too complicated.

#### 02/12/2019  
Finally got the BT working. It can receive characters from my phone. I have replaced the HC-06 with HC-05  
and this should allow me to configure it as master so it can communicate with the second device. It should  
also allow me to restrict access to the connection by specifying exact device the master can connect to. I think  
that this could reduce the interference from other devices around that might be trying to connect.  
For some reason I am having problems with PRINTF now working at all. I have tried to use semihosting as well as  
UART to no avail. Might need to talk to somebody about this as apart from adding the BT I am not aware of any
other change that could have caused this.  

#### 03/12/2019  
I have been playing with the BT modules and configured one of them as master and another one as slave with matching  
UART settings. As BT still doesn't work with my boards I cannot test their connectivity. The only other option is  
to use two USB-to-TTL devices and connect the BTs to them. This might actually be an good option that could help  
me to setup these correctly as prior to plugging them to K64Fs. This would ensure that the devices are correctly  
configured and if there are any errors, they would be most likely in my source code. I also want to restrict   
the communication to just these two devices by binding their addresses.
