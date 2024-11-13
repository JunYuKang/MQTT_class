const net = require('net');

function ip6To4(ip){
    if (net.isIPv6(ip)){
        const ipv4 = /(^|\d+\.){3}\d+/;
        const match = ip.match(ipv4);
        return match ? match[0] : ip;
    }
    return ip;
};
module.exports = ip6To4;