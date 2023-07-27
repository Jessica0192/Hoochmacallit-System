<h1 align="center">The Hoochmacallit System</h1>
The project is about developing an application suite (a system) consisting of three distinct processing components: the "data creator" (DC) application, the "data reader" (DR) application, and the "data corruptor" (DX) application. The purpose of the project is to understand and implement Inter-Process Communication (IPC) mechanisms within UNIX/Linux System Application programming. The project aims to determine which IPC mechanism is better suited for specific communication tasks.


## Important Note
The content of this repository is an academic project and should not be made public as it is related to academic coursework. Any misuse of the project may result in serious consequences, including academic penalties and disciplinary action. Access to the code base and additional materials related to this project may be granted only for legitimate educational or research purposes. 


## Technology Stack
Linux
C

## Programs
### Data Creator (DC) Application
* The DC application's role is to artificially generate a status condition representing the state of the Hoochamacallit machine.
* The application sends messages via a message queue to the Data Reader application (DR).
* The messages include the machine's ID (using the PID of the process) and a randomly selected status value.
* The status values represent different conditions of the machine, such as OKAY, Hydraulic Pressure Failure, Safety Button Failure, etc.
* The DC application sends messages on a random time basis between 10 and 30 seconds apart.
* The application continues to send messages until it generates a "Machine is Off-line" message, at which point it may exit.
* The DC application should have proper error handling and checks for the existence of the message queue before sending messages.
* Debug output can be used during development but should be removed in the final version.

### Data Reader (DR) Application
* The DR application's purpose is to monitor the incoming message queue for different Hoochamacallit machines.
* It keeps track of the number of different machines present and records each new message it receives in a data monitoring log file.
* The DR application creates and maintains a master list of all clients it communicates with, implemented in shared memory.
* The master list contains the message queue ID, the number of DC processes in communication, and information about each DC process.
* The DR application logs events such as adding new machines, updating messages, and detecting offline machines.
* If all Hoochamacallit machines go offline, the DR application logs the event, frees up shared memory, and terminates.

### Data Corruptor (DX) Application
* The DX application's purpose is to gain knowledge of the resources and processes in the application suite and randomly perform allowable corruptions.
* Corruptions include killing a DC process or deleting the message queue used between DCs and the DR application.
* The DX application tries to attach to shared memory created by the DR process and performs its actions in a loop with random time intervals.
* Each action taken by DX is logged in a dataCorruptor.log file with timestamps and descriptions of the actions.
* Proper error handling is essential to handle failures when trying to kill processes, delete files, or remove message queues.
