#!/usr/bin/env node

var Buffer = require('buffer').Buffer;
var parse = require('lwes').parse;
var fs = require('fs');
var dgram = require('dgram');
var logfile = fs.createWriteStream('lwes.bin');

dgram.createSocket("udp4").on('message', function(message, info){
  var buffer = new Buffer(message);
  var lwesEvent = parse(buffer);
  console.log(buffer);
  console.log(lwesEvent);
}).on('listening', function(){
  console.log("start listening on 0.0.0.0:12345.");
}).bind(12345);

