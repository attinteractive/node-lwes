var Buffer = require('buffer').Buffer;
var parse = require('./parser').parse;
var dgram = require('dgram');


LWES = function(options) {
  // Make sure that the new operator is always used (http://ejohn.org/apps/learn/#36)
  if ( !(this instanceof arguments.callee) ) {
    return new arguments.callee(arguments);
  }
};

LWES.prototype.createListener = function(filteredEvents, callback, port, address) {
  if ( typeof callback !== 'function') { 
    throw { name:     'TypeError'
          , message:  'callback must be a function'
    };
  }

  // Use defaults if necessary
  port = port || 1111
  address = address || '0.0.0.0' 

  var filter = (typeof filteredEvents !== 'undefined' && filteredEvents.length > 0);

  dgram.createSocket('udp4').on('message', function(message, info) {
    var buffer = new Buffer(message);
    var lwesEvent = parse(buffer);

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
