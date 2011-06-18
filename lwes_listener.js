#!/usr/bin/env node

var LWES = require('./lib/lwes').LWES;
var lwesObj = new LWES();
var filteredEvents = ['EventTypeA', 'EventTypeB'];

lwesObj.createListener(filteredEvents, function(lwesEvent) {
  console.log("I Found a filtered event: " + lwesEvent.type);
});
