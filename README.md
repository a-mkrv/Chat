# WaveTalk (~Whisper)
<center>
<img src="https://cloud.githubusercontent.com/assets/12527666/22626570/60ae9e12-ebc1-11e6-9089-5d86ced0a4dc.png" width="250">
</center>

### Client - Server application, Messenger

The сrossplatform client is the messenger, which interacts with the [mobile client](https://github.com/MarioCode/WaveTalk)  and [desktop applications](https://github.com/MarioCode/Chat/tree/master/Client). Data transmission between devices is organized with using TCP sockets.

#### Client: 
Cross-platform program written in C ++ (Qt). Has a complete user-friendly interface, a lot of settings, support for encryption.

#### Server:
It is a centralized place for connecting clients, both desktop and mobile.
This server can be deployed not only for the Internet, but also for local work. To store user information used the SQLite database. 
Used partial multithreading and support at least 100 connections.

The development is aimed at creating a secure multi-user system for instant information exchange between users.

<img src="https://user-images.githubusercontent.com/12527666/29724493-5a5a6e2a-89d1-11e7-8aba-30f7814a9bfa.png" width="30"> Mobile iOS client (Swift): https://github.com/MarioCode/WaveTalk

<img src="https://user-images.githubusercontent.com/12527666/29724734-4b37921e-89d2-11e7-818b-ddf0a550e5af.png" width="30"> This project is developed as a diploma thesis of bachelor for the University. 

---

#### Already implemented:
+ Finished UI parts, user-friendly interface
+ Registration, authorization of users
+ Group Dialogues
+ Full translation of 4 languages (Russian, English, French, German) 
+ Notifications and emoji
+ Encryption:
  - RSA (256 bit) - for messages
  - MD5 (+salt) - for user passwords
+ Database: 
  - SQLite - storage for text information (user's data, messages, etc)

#### In the plans:
+ _Storage of media files_
+ _Improve server stability_
+ _Using multithreading in the client_
+ _Use real user avatars (as in the mobile client)_
+ _Choice of encryption algorithm_

---

#### Instruments:
+ Qt Creator 4+
+ C++
+ SourceTree / Git

---

### Build the project
```
qmake Chat.pro -spec macx-clang CONFIG+=debug CONFIG+=x86_64 CONFIG+=qml_debug && /usr/bin/make qmake_all
```

### The current developments

#### Working area of the chat. Contact list and selected dialog
<img src="https://user-images.githubusercontent.com/12527666/29733619-b82c1786-89f6-11e7-9f4b-bb274e5199ed.jpg" width="700">

#### Authorization, Registration
<img src="https://user-images.githubusercontent.com/12527666/29733670-f8e82b20-89f6-11e7-9c65-a5379cff5a89.png" width="700">

#### Setting (profile), Chat space
<img src="https://user-images.githubusercontent.com/12527666/29733673-0001db2c-89f7-11e7-90ca-d8106954f39d.png" width="700">

#### Create group chat, Server log
<img src="https://user-images.githubusercontent.com/12527666/29733674-030e54b2-89f7-11e7-840c-fbd8f7dc7045.png" width="700">
