var Buffer = require('buffer').Buffer;
var parse = require('./parser/build/Release/parser').parse;
var dgram = require('dgram');


LWES = function(options) {
  // Make sure that the new operator is always used (http://ejohn.org/apps/learn/#36)
  if ( !(this instanceof arguments.callee) ) {
    return new arguments.callee(arguments);
  }
  this.defaultAddress = '0.0.0.0';
};

LWES.prototype.createListener = function(filteredEvents, callback, port, address) {
  if ( typeof callback !== 'function') { 
    throw { name:     'TypeError'
          , message:  'callback must be a function'
    };
  }

  // Use defaults if necessary
  port = port || 1111
  address = address || this.defaultAddress; 


  var udpSocket = dgram.createSocket('udp4');
  // If we're not listening locally, join a multicast group
  if ( address !== this.defaultAddress ) {
    udpSocket.addMembership(address);
  }

  var filter = (typeof filteredEvents !== 'undefined' && filteredEvents.length > 0);
  var lwesEvent = null;
  var buffer = null;

  udpSocket.on('message', function(message, info) {
    buffer = new Buffer(message);
    lwesEvent = parse(buffer);

    if ( filter ) { 
      if ( filteredEvents.indexOf(lwesEvent.type) >= 0) {
        callback(lwesEvent);
      }
    } else {
      callback(lwesEvent);
    }
  }).on('listening', function(){
    console.log('listening for LWES events on ' + address + ':' + port);
  }).bind(port, address);

};

LWES.prototype.Event = require('./event').Event;

exports.LWES = LWES;
