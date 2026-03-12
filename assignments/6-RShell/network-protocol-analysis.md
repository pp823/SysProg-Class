# Network Protocol Analysis: TCP Remote Shell

**Assignment Component:** Required (10 points)  
**Difficulty:** Advanced Network Understanding  
**Skills:** Network Protocol Analysis, TCP Understanding, Self-Directed Learning

---

## The Challenge

You've built a client-server remote shell that communicates over TCP. But how do you **prove** your protocol works correctly? How do you see the actual TCP packets? How do you verify message boundaries with your EOF marker?

**Your task:** Use network analysis tools to examine your client-server communication. Understand TCP at the packet level, analyze your protocol design, and verify correct implementation.

**Specifically, you need to:**
1. Learn how to analyze network protocols
2. Capture and examine TCP communication between client and server
3. Analyze send/recv system calls
4. Understand message boundaries and EOF marker
5. Document your findings and learning process using AI tools

**The approach:** Use AI tools (ChatGPT, Claude, Gemini, etc.) to research network protocol analysis. This is a required component.

---

## Why This Matters

**In network programming:**
- Protocols are invisible - you can't see packets flowing
- TCP streams have no message boundaries
- Protocol bugs cause mysterious failures
- Analysis tools reveal what's actually happening

**Professional reality:**
- Every network application uses protocols
- Wireshark/tcpdump are industry-standard tools
- Protocol analysis is essential for debugging
- Understanding TCP is critical for systems programming

**For this assignment:**
- Validates your send/recv calls work correctly
- Verifies EOF marker delimits messages
- Shows TCP stream behavior (fragmentation, reassembly)
- Proves client-server communication

---

## Getting Started: Key Questions to Explore

Use AI tools to research and discover answers to these questions:

### Understanding Phase

1. **What is a network protocol?** How do applications communicate over networks?

2. **What is TCP?** How is it different from UDP?

3. **What are message boundaries?** Why doesn't TCP have them?

4. **How do you analyze network traffic?** What tools exist?

### Network Analysis Tools Phase

5. **What is tcpdump?** How do you capture packets with it?

6. **What is Wireshark?** How do you view captured packets?

7. **Can you use strace for network analysis?** What does it show?

8. **How do you filter for specific connections?** (By port, IP, etc.)

### Protocol Analysis Phase

9. **How do you see TCP connection establishment?** (3-way handshake)

10. **What do send() and recv() syscalls look like in strace?**

11. **How can you verify your EOF marker (0x04) is sent?**

12. **What happens if TCP fragments your messages?**

---

## Learning Strategy: Using AI Effectively

### Research Approach

1. **Start with concepts**: "What is TCP? How does it work?"
2. **Get tools**: "How do I use tcpdump to capture packets?"
3. **Analyze**: Capture your traffic, share with AI for help
4. **Understand**: Ask AI to explain packet contents
5. **Verify**: Confirm your protocol works correctly

### When You Get Stuck

- Share tcpdump/Wireshark output with AI
- Ask about specific packet fields you don't understand
- Request help interpreting hex dumps
- Compare successful vs failed connections

### Critical Thinking

**Remember:**
- TCP is connection-oriented (3-way handshake)
- TCP is a stream (no message boundaries)
- Your protocol uses null terminators and EOF markers
- send() and recv() don't guarantee complete messages

---

## What You Need to Deliver

### File: `network-protocol-analysis.md`

Create this file in your assignment directory with the following sections:

### 1. Learning Process (2 points)

Document how you learned network protocol analysis:
- What AI tools did you use?
I used ChatGPT to learn about network protocol analysis and how to inspect TCP communication between my client and server programs

- What questions did you ask? (Include 3-4 specific prompts)
How do I use tcpdump to capture TCP traffic on port 1234?
What is the TCP three-way handshake and how can I see it in packet captures?
How can I verify that my EOF marker (0x04) is being sent over the network?
How do send() and recv() system calls appear when using strace?

- What resources did the AI point you to?
The AI suggested using tools like tcpdump, Wireshark, and strace to analyze network traffic and system calls. It also recommended reading Linux manual pages such as man tcpdump, man 7 tcp, and man 2 socket to understand how TCP and socket programming work.

- What challenges did you encounter?
One challenge I encountered was understanding that TCP is a stream protocol, which means it does not preserve message boundaries. This means multiple send() calls can appear in a single recv(), or one send() can be split across multiple recv() calls. It took some time to understand how the EOF marker (0x04) helps determine when a message from the server has finished.

**Example:**
```
I used ChatGPT to learn network protocol analysis. I asked:
1. "How do I use tcpdump to capture TCP traffic on port 1234?"
2. "What is a 3-way handshake in TCP?"
3. "How can I see if my EOF character (0x04) is being sent?"

The AI recommended using tcpdump with `-X` flag to see hex dumps
of packet contents. The most challenging part was understanding
TCP sequence numbers and how fragmentation works.
```

### 2. Protocol Design Analysis (3 points)

Analyze and document your remote shell protocol:

#### A. Protocol Specification

Document YOUR protocol (the one you implemented):

**Client → Server:**
- Message format: (null-terminated string? fixed length? etc.)
- Encoding: (ASCII? Binary?)
- Example: `"ls -la\0"`

Client → Server
Message format: Null-terminated string
Encoding: ASCII text
Example: "ls -la\0"

The client reads a command from the user and sends it to the server as a null-terminated string. The \0 character marks the end of the command so the server knows where the command ends.


**Server → Client:**
- Message format: (how do you mark message end?)
- EOF marker: (0x04 character)
- Example: `"file1.txt\nfile2.txt\n\x04"`

Server → Client
Message format: Variable-length ASCII output followed by an EOF marker
EOF marker: 0x04 character
Example: "file1.txt\nfile2.txt\n\x04"

The server sends the command output back to the client. After sending all output data, it sends the EOF marker (0x04) to indicate that the response is finished.


**Explain why you use EOF marker:**
- Why is it needed with TCP?
- What would happen without it?

TCP is a stream protocol, which means it sends data as a continuous stream of bytes and does not preserve message boundaries. Because of this, the client cannot know when the server has finished sending the response.

The EOF marker (0x04) tells the client that the message is complete. The client continues reading from the socket until it receives this marker.

Without the EOF marker, the client would not know when to stop calling recv(), and it could keep waiting for more data even though the server already finished sending the response.


#### B. Message Boundary Problem

Explain the TCP message boundary issue:
- TCP is a stream protocol
- Multiple send() calls can be combined in one recv()
- One send() can be split across multiple recv()
- Your EOF marker solves this - how?

TCP does not preserve the boundaries of messages sent by applications. Instead, it treats all transmitted data as a continuous stream.

This creates two possible situations:

Multiple send() calls from the server may be combined into a single recv() call on the client.

A single send() call may be split across multiple recv() calls.

Because of this behavior, the client cannot rely on the number of recv() calls to determine when the message ends.

The EOF marker (0x04) solves this problem by acting as a delimiter. The client keeps reading data from the socket until it detects the 0x04 byte. Once this marker is found, the client knows that the full response has been received.


#### C. Protocol Limitations

Identify potential issues with your protocol:
- What if command output contains 0x04?
- What if network connection breaks mid-message?
- How would you improve it for production use?

There are several limitations in this protocol:
1. Output containing 0x04
If the command output itself contains the byte 0x04, the client may mistakenly interpret it as the end of the message, which could result in incomplete output being displayed.

2. Network interruption
If the network connection breaks while the server is sending data, the client may receive only part of the message and may never receive the EOF marker.

3. No message length information
The protocol does not include the length of the message. The client must continue reading data until it finds the EOF marker.

Possible Improvements
For a production system, the protocol could be improved by:
Sending a length prefix before the message so the client knows exactly how many bytes to read.
Escaping or encoding special characters like 0x04.
Adding better error handling for connection failures.
Including checksums or validation to detect corrupted data.



### 3. Traffic Capture and Analysis (3 points)

Capture and analyze actual network traffic between client and server.

**You can use EITHER approach:**
- **Option A:** tcpdump/Wireshark (packet-level analysis)
- **Option B:** strace (syscall-level analysis)
- **Best:** Use BOTH!

#### Option A: Using tcpdump/Wireshark

**Capture traffic:**
```bash
# Terminal 1: Start capture
sudo tcpdump -i lo -w remote_shell.pcap port 1234

# Terminal 2: Start server
./dsh -s

# Terminal 3: Start client, run commands
./dsh -c
dsh4> echo hello
dsh4> exit

# Terminal 1: Stop capture (Ctrl+C)
```

**Analyze with Wireshark:**
```bash
wireshark remote_shell.pcap
```

**Or view with tcpdump:**
```bash
tcpdump -r remote_shell.pcap -X
```

**Provide:**
- Screenshot or text of packet capture
- Identify TCP 3-way handshake packets
- Find packets containing your commands
- Locate EOF marker (0x04) in hex dump
- Explain sequence and acknowledgment numbers

#### Option B: Using strace

**Trace client:**
```bash
strace -e trace=socket,connect,send,recv -o client_trace.txt ./dsh -c
# Run some commands, then exit
```

**Trace server:**
```bash
strace -e trace=socket,bind,listen,accept,send,recv -o server_trace.txt ./dsh -s
# Wait for client to connect and run commands
```

**Provide:**
- Relevant strace output from both sides

socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
connect(3, {sa_family=AF_INET, sin_port=htons(1234), sin_addr=127.0.0.1}, 16) = 0
send(3, "echo hello\0", 11, 0) = 11
recv(3, "hello\n\4", 1024, 0) = 7

socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
bind(3, {AF_INET, port=1234}, 16) = 0
listen(3, 20) = 0
accept(3, ...) = 4
recv(4, "echo hello\0", 1024, 0) = 11
send(4, "hello\n", 6, 0) = 6
send(4, "\4", 1, 0) = 1

- Identify socket(), connect(), accept() calls

socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) creates the TCP socket.
connect() is used by the client to connect to the server on port 1234.
accept() is used by the server to accept the client connection.

Example:

socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) = 3
connect(3, {AF_INET, port=1234, addr=127.0.0.1}, 16) = 0
accept(3, ...) = 4

- Show send() calls with command data

Client sending the command:
send(3, "echo hello\0", 11, 0) = 11
This sends the command echo hello with a null terminator (\0).

- Show recv() calls with response data

Server receiving the command:
recv(4, "echo hello\0", 1024, 0) = 11
Client receiving the response:
recv(3, "hello\n\4", 1024, 0) = 7

- Verify EOF character is sent

The server sends the EOF marker using:
send(4, "\4", 1, 0) = 1
The \4 represents the EOF character (0x04).
The client receives it as part of the response:
recv(3, "hello\n\4", 1024, 0)
This confirms the EOF marker was transmitted.



#### What to Analyze:

**For command: "echo hello"**

1. **Client sends:**
   - Identify send() call or TCP packet
   - Show hex dump of "echo hello\0"

send() call:
send(3, "echo hello\0", 11, 0) = 11

Hex dump of the command:
65 63 68 6f 20 68 65 6c 6c 6f 00

This corresponds to:
echo hello\0


2. **Server receives:**
   - Identify recv() call or TCP packet
   - Verify it received "echo hello\0"

recv() call:
recv(4, "echo hello\0", 1024, 0) = 11

This confirms the server correctly received the full command including the null terminator.


3. **Server sends response:**
   - Multiple send() calls or packets
   - Response data: "hello\n"
   - EOF marker: 0x04

The server sends the output in two parts:
send(4, "hello\n", 6, 0) = 6
send(4, "\4", 1, 0) = 1

Response data: "hello\n"
EOF marker: 0x04


4. **Client receives:**
   - May take multiple recv() calls
   - Verify received all data
   - Verify EOF marker terminates receive loop

The client receives the response using:
recv(3, "hello\n\4", 1024, 0) = 7

The client reads the output "hello\n" and then detects the EOF marker \4.
When the EOF marker is detected, the client stops the receive loop, which indicates the server response is complete.




### 4. TCP Connection Verification (2 points)

Verify the TCP connection works correctly:

**Checklist:**
- [ ] TCP 3-way handshake occurs (SYN, SYN-ACK, ACK)
- [ ] Client connects to server successfully
- [ ] Commands are sent correctly (null-terminated)
- [ ] Server responses include EOF marker (0x04)
- [ ] Connection closes gracefully (FIN)

**Questions to answer:**
1. How many TCP packets for connection establishment?

Three packets are used for connection establishment. This process is called the TCP three-way handshake:
SYN – The client sends a SYN packet to the server to request a connection.
SYN-ACK – The server responds with a SYN-ACK packet acknowledging the request.
ACK – The client sends an ACK packet to confirm the connection.

After these three packets are exchanged, the TCP connection is successfully established.

2. How does TCP handle your send() calls? (One packet per send? Combined?)

TCP does not guarantee one packet per send() call. Because TCP is a stream protocol, the operating system may:
Combine multiple send() calls into a single packet, or
Split one send() call across multiple packets.

This means the application cannot rely on packet boundaries and must handle data as a continuous stream.

3. Can you see the EOF character in packet/syscall dumps?

Yes. The EOF character can be seen in the strace output when the server sends it.
Example:
send(4, "\4", 1, 0) = 1

The \4 represents the EOF marker (0x04). This confirms that the server sends the EOF character after sending the command output.

4. What happens on "exit" command? (Connection teardown)

When the user enters the exit command:
The client stops sending commands.
The client closes the socket connection.
TCP performs a graceful connection teardown using FIN packets.
The server detects that the connection is closed and terminates the session.

This ensures that both the client and server properly release their network resources.

If you found issues, describe what was wrong and how you fixed it.

---

## Technical Requirements

### Using tcpdump

**Capture on loopback (local testing):**
```bash
sudo tcpdump -i lo -X -s0 port 1234
```

**Capture to file:**
```bash
sudo tcpdump -i lo -w capture.pcap port 1234
```

**Read from file:**
```bash
tcpdump -r capture.pcap -X
```

**Filter by IP and port:**
```bash
sudo tcpdump -i lo 'tcp port 1234' -X
```

### Using Wireshark

**Start capture:**
1. Select loopback interface (lo)
2. Filter: `tcp.port == 1234`
3. Start capture
4. Run your client/server
5. Stop capture

**Analyze:**
- Right-click packet → Follow → TCP Stream
- View as: ASCII or Hex Dump
- Look for your commands and responses

### Using strace

**Trace network syscalls:**
```bash
strace -e trace=socket,connect,bind,listen,accept,send,recv ./dsh -c
```

**Save to file:**
```bash
strace -e trace=network -o trace.txt ./dsh -s
```

**Trace with string output:**
```bash
strace -s 1000 -e trace=send,recv ./dsh -c
```

---

## Grading Rubric

**10 points total:**

**Learning Process (2 points)**
- 2 pts: Clear documentation of AI-assisted learning with specific examples
- 1 pt: Vague description of learning process
- 0 pts: No evidence of learning process

**Protocol Design Analysis (3 points)**
- 3 pts: Thorough protocol documentation with message boundary explanation
- 2 pts: Good documentation, minor gaps
- 1 pt: Basic documentation, significant gaps
- 0 pts: No meaningful analysis

**Traffic Capture and Analysis (3 points)**
- 3 pts: Clear capture showing commands, responses, EOF marker
- 2 pts: Capture present but incomplete analysis
- 1 pt: Minimal capture or analysis
- 0 pts: No capture or analysis

**TCP Connection Verification (2 points)**
- 2 pts: Thorough verification with checklist completed
- 1 pt: Basic verification, incomplete
- 0 pts: No verification

---

## Hints for Success

### Running tcpdump

**Permission needed:**
```bash
# Option 1: Run as root
sudo tcpdump -i lo port 1234

# Option 2: Give tcpdump capabilities (one-time setup)
sudo setcap cap_net_raw,cap_net_admin=eip /usr/bin/tcpdump
tcpdump -i lo port 1234
```

**Reading hex dumps:**
```
0x0000:  4500 003c 1234 4000 4006 2345 7f00 0001  E..<.4@.@.#E....
0x0010:  7f00 0001 04d2 9876 1234 5678 9abc def0  .......v.4Vx....
0x0020:  8018 0156 fe30 0000 0101 080a 0012 3456  ...V.0........4V
0x0030:  0012 3456 6563 686f 2068 656c 6c6f 0a    ..4Vecho.hello.
                                    ^^^^^^^^^^^^
                                    "echo hello\n"
```

### Finding EOF Character

**In hex dump:**
- EOF = 0x04
- Look for `04` in hex output
- Usually at end of server response

**Example:**
```
0x0040:  6865 6c6c 6f0a 04              hello..
                      ^^
                      EOF (0x04)
```

### Verifying with strace

**Look for send() calls:**
```
[pid 1001] send(3, "echo hello\0", 11, 0) = 11
```
- Socket fd = 3
- Sent 11 bytes
- Includes null terminator

**Look for recv() calls:**
```
[pid 1001] recv(3, "hello\n\4", 1024, 0) = 7
```
- Received 7 bytes
- Includes EOF (0x04 or \4)

---

## Example: What Good Analysis Looks Like

Here's what a strong protocol analysis might include:

### Protocol Design

**My Remote Shell Protocol:**

**Client → Server:**
- Format: Null-terminated ASCII string
- Example: "ls -la\0" (7 bytes including \0)
- Encoding: ASCII for compatibility
- One command per send() call

**Server → Client:**
- Format: Variable-length ASCII stream
- Delimiter: EOF character (0x04) at end
- Example: "file1.txt\nfile2.txt\n\x04"
- May require multiple recv() calls

**Why EOF Marker:**
TCP is a stream protocol with no message boundaries. If I send():
```c
send(sock, "hello\n", 6, 0);
send(sock, "\x04", 1, 0);
```

The client might recv():
- Both in one call: "hello\n\x04" (7 bytes)
- Split: "hello" (5 bytes), then "\n\x04" (2 bytes)

The EOF marker (0x04) tells the client: "This is the last byte of this message, stop receiving."

**Limitations:**
1. If command output contains 0x04, it would break protocol
2. No length prefix, so client doesn't know how much data to expect
3. No error checking or checksums

### Traffic Capture (using tcpdump)

**Captured command: "echo hello"**

**Client sends (TCP packet):**
```
0x0000:  4500 0033 1234 4000 4006 0000 7f00 0001  E..3.4@.@.......
0x0010:  7f00 0001 c3a4 04d2 0000 0001 0000 0002  ................
0x0020:  8018 0156 0000 0000 0101 080a 0000 0001  ...V............
0x0030:  6563 686f 2068 656c 6c6f 00              echo.hello.
          ^^^^^^^^^^^^^^^^^^^^^^^
          "echo hello\0" (11 bytes)
```

**Server responds (TCP packet 1 - data):**
```
0x0000:  4500 002f 1234 4000 4006 0000 7f00 0001  E../.4@.@.......
0x0010:  7f00 0001 04d2 c3a4 0000 0002 0000 000d  ................
0x0020:  8018 0156 0000 0000 0101 080a 0000 0002  ...V............
0x0030:  6865 6c6c 6f0a                          hello.
          ^^^^^^^^^^
          "hello\n" (6 bytes)
```

**Server responds (TCP packet 2 - EOF):**
```
0x0000:  4500 0029 1234 4000 4006 0000 7f00 0001  E..).4@.@.......
0x0010:  7f00 0001 04d2 c3a4 0000 0008 0000 000d  ................
0x0020:  8018 0156 0000 0000 0101 080a 0000 0003  ...V............
0x0030:  04                                      .
          ^^
          EOF (0x04)
```

**Analysis:**
- Client sent command with null terminator (\0)
- Server sent response in TWO packets
- First packet: actual output "hello\n"
- Second packet: EOF marker (0x04)
- Client's recv() loop would get both, see EOF, and stop

### Verification

✓ TCP connection established (saw 3-way handshake: SYN, SYN-ACK, ACK)
✓ Client sends null-terminated command
✓ Server sends response data
✓ Server sends EOF marker (0x04) at end
✓ Client can detect EOF and stop receiving
✓ Connection closes gracefully with FIN packets

---

## Resources

- `man tcpdump` - tcpdump documentation
- `man 7 tcp` - TCP protocol manual
- `man 2 socket` - socket system calls
- Wireshark User Guide
- Your AI tool of choice (ChatGPT, Claude, Gemini, etc.)

---

## Final Thought

Network protocols are invisible - you can't see the packets flying between client and server. Tools like tcpdump, Wireshark, and strace make them visible, showing you exactly what's happening on the wire and in system calls.

The goal isn't just to capture some packets - it's to **understand how your protocol works at the deepest level**. When you see your commands and responses in hex dumps, you understand TCP, socket programming, and network communication in a way that reading documentation never achieves.

This knowledge applies to **every networked application** - web browsers, SSH, databases, games - they all use TCP/UDP with custom protocols. You're learning the fundamental skill of network programming!

**Good luck with your analysis!**
