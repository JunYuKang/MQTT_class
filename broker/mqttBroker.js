const aedes = require('aedes');
const net = require('net');
const getAddress = require('./address');
const ip6To4 = require('./ipv6ToIpv4');

class mqttBroker {
    constructor(port) {
        this.port = port;
        this.aedes = aedes();
        this.broker = net.createServer(this.aedes.handle);
        this.connected = [];
    };
    setupBroker() {
        this.broker.listen(this.port, () => {
            console.log('Broker started and listening on ->', getAddress() + ':' + this.port);
            console.table(this.connected);
        });
        this.aedes.on ('client', (client) => {
            console.log(client.id, 'connected !');
            this.connected.push({
                ID:client.id,
                address:ip6To4(client.conn.remoteAddress),
                role:'client',
                topics:[],
                QoS:null
            });
            console.table(this.connected);
        });
        this.aedes.on('clientDisconnect', (client) => {
            console.log(client.id, 'got disconnected !');
            this.connected = this.connected.filter((conn) => conn.ID !== client.id);
            console.table(this.connected);
        });
    };
    setupPublish() {
        this.aedes.on('publish', (packet, client) => {
            if (client ==null) {
                return;
            };
            const index = this.connected.findIndex((conn) => conn.ID === client.id);
            if (index !== -1) {
                if (this.connected[index].role == 'client') {
                    this.connected[index].role = 'publisher';
                };
                if (!this.connected[index].topics.includes(packet.topic)) {
                    this.connected[index].topics.push(packet.topic);
                };
                if (this.connected[index].QoS == null || this.connected[index].QoS !== packet.qos) {
                    this.connected[index].QoS = packet.qos;
                };
            };
            console.table(this.connected);
        });
    };
    setupSubscribe() {
        this.aedes.on('subscribe', (subscriptions, client) => {
            if (client == null) {
                return;
            };
            const index = this.connected.findIndex((conn) => conn.ID === client.id);
            if (index !== -1) {
                this.connected[index].role = 'subscriber';
                if (!this.connected[index].topics.includes(subscriptions[0].topic)) {
                    this.connected[index].topics.push(subscriptions[0].topic);
                };
                this.connected[index].message = null;
                this.connected[index].QoS = subscriptions[0].qos;
            };
            console.table(this.connected);
        });
        this.aedes.on('unsubscribe', (subscriptions, client) => {
            if (client == null) {
                return;
            };
            const index = this.connected.findIndex((conn) => conn.ID === client.id);
            if (index !== -1) {
                this.connected[index].topics = this.connected[index].topics.filter(topic => topic !== subscriptions[0].topic);
                if (this.connected[index].topics.length === 0) {
                    this.connected[index].role = 'client';
                };
            };
            console.table(this.connected);
        });
    };
};

const broker = new mqttBroker(1883);
broker.setupBroker();
broker.setupPublish();
broker.setupSubscribe();