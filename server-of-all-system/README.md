# Virtual Strike Game Server

This repository contains the WebSocket server implementation for the Virtual Strike game, developed as part of the CSE 396 Computer Engineering Project at Gebze Technical University in Spring 2024. This server facilitates real-time communication between the game's client-side components and the backend game logic.

## Project Overview

Virtual Strike is an innovative shooting game designed to provide players with an immersive gaming experience using Virtual Reality (VR) technology. The game integrates players' real-life movements into the virtual world, allowing them to control their in-game character and aim at targets through physical actions.

## Server Functionality

The server uses WebSocket technology to handle real-time data transmission between the client's input devices (such as VR headsets and custom controllers) and the game's logic running either on a local machine or a remote server. It is responsible for:
- Receiving joystick and sensor data from clients.
- Broadcasting these inputs to the game logic for processing.
- Sending game state updates and visuals back to clients.

## Technologies

- Node.js
- WebSocket (using the `ws` library)
- Crypto (for UUID generation of client sessions)

## Setup and Installation

1. **Clone the repository:**
   ```bash
   git clone https://github.com/yourusername/virtual-strike-server.git
   cd virtual-strike-server
   ```
2. **Install dependencies:**
   ```bash
   npm install
   npm start
   ```

## Contributing
Contributions to the Virtual Strike server are welcome. Please ensure to follow the existing code style and add unit tests for any new or changed functionality.

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.

## Acknowledgments
Gebze Technical University
CSE 396 Course Staff
All team members who contributed to the development of Virtual Strike

## Contact
For any inquiries, you can reach out to the project team through the issues section of this GitHub repository.