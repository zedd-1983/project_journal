I have started my project journal on GitHub today. The plan for this week in terms of 
the project is to finalize my project idea, discuss it with my supervisor and few other 
lecturers and write up a proposal to be submitted by Sunday 6th.


##07/10/2019  
During my last week I have setup the **MCUXpresso and FreeRTOS** and tested everything compiles
and works. I have created and submitted the project proposal.
Plan for this week is to get SystemView working with K64F to monitor the FreeRTOS and to
start working on the moisture detection. This will likely be an external interrupt.


##14/10/2019  
I have order two K64F development boards off Brian today along with a **J-Link EDU** debugging
probe that will help me to monitor the behaviour of the FreeRTOS using **SystemView**. Waiting
for confirmation from Brian that the order is OK and expected delivery times. I have also 
acquired 2 Bluetooth modules to get the wireless communication going. Didn't get to work on
the boards at all unfortunately but giving myself this week the get the external interrupt 
going and start working on the RTC for timekeeping. Will probably have to talk to Paul or 
Niall about the design of the FreeRTOS system and it's tasks

##25/10/2019   
Last week I have been working on the development of the software for K64F. I still haven't 
received any components but at the moment I am not dependent on them. So I got the external
interrupt working that is suppose to happen when moisture is detected and at the moment
this event is simulated by pushing a button on the development board. I have also worked on
development of the RTC to allow timekeeping which was a bit of a struggle as the example
provided by NXP in their software development kit wouldn't work properly for me and I was
running into all sorts of errors when I was trying to compile it or include snippets of it
in my code. Got it working eventually but thinking of re-designing the RTOS as at the moment
seems somewhat cumbersome.

##1/11/2019   
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
