# IRC
- What is IRC?
  - An IRC server (Internet Relay Chat server) is a type of server that facilitates real-time communication between users on a network, enabling them to send and receive text-based messages in real-time. It operates as part of the larger IRC system, which is a protocol used for online messaging, especially for group chats and private messages.

- This project implements an IRC (Internet Relay Chat) server, enabling real-time communication among users.
- The server supports multiple channels (chat rooms) where users can join, interact, and manage their participation.
# Key Commands

  - JOIN
    - Allows a user to join a specific channel or create a new one if it doesn't exist.
    - Usage: ``` /JOIN #channelname ```
    - A user sends the ```JOIN``` command to connect to a channel and begin interacting with other users within it.
  
  - KICK
    - Removes a user from a channel.
    - Usage: ``` /KICK #channelname username ```
    -  A channel operator or admin can use the ```KICK``` command to eject a user from a channel, typically for violating rules or disruptive behavior.
  
  - PART
    - Allows a user to leave a channel.
    - Usage: ```/PART #channelname```
    - When a user no longer wants to participate in a channel, they use the ```PART``` command to exit the channel.
  
  - INVITE
    - Invites a user to join a specific channel.
    - Usage: ```/INVITE username #channelname```
    - A user can invite another user to join a private channel. Typically, this is used for private or invite-only channels.
  
  - MODE
    -  Sets or changes the mode of a channel or user.
    -  Usage: ```/MODE #channelname +i``` (e.g., enabling invite only mode for a channel)
    -  The ```MODE``` command is used for channel and user management, allowing the setting of various modes like making a channel moderated (only operators can speak), or changing user modes (like banning or giving them operator status).

# File Transfering
  - In this IRC server it's possible to send files trough TCP.
  - It works with the following commands :
    - ```SF``` command.
      - The user that wants to send the file uses ```SF``` command. This command initiates the transfering protocol. 
      - Usage: ``` /SF username filename or file path ```
      - Once the user sends this command, the user that is going to receive the file will receive a notification informing that there is a pending file transfer request.
    - ```SFA``` command.
    -  To accept the file trasnfer request the user that is receiving it must use the ```SFA``` command.
    -  Usage: ```/SFA username ```
    -  When the command is sent to the server the transfering protocol will establish connection between the two clients and accept the connection.
