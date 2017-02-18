var net = require('net');

/* Create Server */
var server = net.createServer(function(socket) {
        /* If the socket receives data */
        socket.on('data', function(data){
                /* Prints data */
                console.log(data.toString());

                /* Echoes data */
                socket.write('Echo: ' + data.toString());
                socket.pipe(socket);
        });

        /* If the socket closes */
        socket.write('Echo server\r\n');
        socket.pipe(socket);
});

/* Starts listening */
server.listen(1337, '127.0.0.1');

