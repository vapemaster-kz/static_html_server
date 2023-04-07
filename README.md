# static_html_server

A server was developed using C programming language.

To compile the code:
1. gcc -c -g server_function.c
2. gcc -c -g server.c
3. gcc -o server server.o server_function.o
4. sudo ./server

You might want to open another terminal to test the code.
In the new terminal, write curl http://127.0.0.1/your_html_file.html

What happens on the server side for different inputs:

<img width="361" alt="Снимок экрана 2023-04-07 в 21 29 53" src="https://user-images.githubusercontent.com/65128133/230637107-6904f563-7acd-4d0f-843b-8d621e753586.png">
