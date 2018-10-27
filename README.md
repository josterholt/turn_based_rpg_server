## JSON Structure
See Client documentation for JSON structures

## Libwebsockets
The server uses libwebsockets to handle the initial websocket connection and handshake.

game_server_protocol handles the raw data. See website for more information: https://libwebsockets.org/

### Types of connections
There are different endpoints for different types of communication:

/connect - New player creating or joining a game
/playerUpdate - Player info update for an existing game

@todo This can probably be consolidated into one endpoint by adding a "type" buffer

#### Creating / Joining a Game
Endpoint: /connect

When the client sends a connection request, a new game will be created unless a `gameId` is specified in the payload.


#### Position Update
Endpoint: /playerUpdate

A connected client updates player state. Payload includes position (x,y), velocity (x,y), facing (0,1,2,3 *needs confirmation)


## Game Server Engine

### Connect Event
When a client requests a connection, a `GameClient` is created.

### Game Client
The game client represents the interface for clients to change game state. This class routes requests to the appropriate `GameInstance` method.

### Game Manager

### Game State
Represents a game instance including identifiers, players, NPCs, events, validation and methods to manipulate state.

Requests and server generated events will use Game State methods. Game State is responsible for validating the requests and then making the change to state.

### Game Unit
Base class for NPC and Player. Holds basic information shared by player and NPC.

#### Game Player
Playable character/unit

#### Game Mob
NPC character/unit managed by server.