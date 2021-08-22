#include "syscall.h"

int
main()
{
    int pingID, pongID;
    PrintString("Ping-Pong test starting...\n");
    pingID = Exec("ping");
    pongID = Exec("pong");
    Join(pingID);
    Join(pongID);
}
