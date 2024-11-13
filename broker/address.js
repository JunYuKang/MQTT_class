const os = require('os');

function getAddress(){
    const networkInterface = os.networkInterfaces();

    for (const name of Object.keys(networkInterface)) {
        for (const net of networkInterface[name]) {
            if (net.family === 'IPv4' && !net.internal) {
                return net.address;
            };
        };
    };
    return 'localhost';
};
module.exports = getAddress;