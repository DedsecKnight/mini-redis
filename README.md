# Mini Redis

## What is it?
This is my attempt to implementing a simplified version of the Redis Cache based on what I've learned [here](https://build-your-own.org/redis/#table-of-contents). 

## Supported commands
Currently, this implementation supports the following commands: 
- `del`: delete a key
- `expire`: set expiration timer for a key
- `get`: get the value associated with a key
- `set`: set the value associated with a key to some key
- `ttl`: get time-to-live value of a key (if exist)
- `zadd`: add a member to sorted set associated with some key
- `zrangebyscore`: get all members whose score falls within a specific range within the sorted set associated with key. 
- `zrem`: remove member from sorted set. 
- `zscore`: get score of member within sorted set. 

## How to run it?
To build this project, create a folder named `build` within the root directory and run the following command from the root folder: 
```
$ cmake --build ./build
```
Upon finish building, CMake will generate the 2 following binaries: 
- `./build/examples/server`: This is the server that will process the command received from client through socket connection. 
- `./build/examples/client`: This is a mock client that will send command to server through socket connection. 