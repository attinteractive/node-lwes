var vows = require('vows'),
  assert = require('assert');
  LWES  = require('../lib/lwes').LWES;


vows.describe('LWES').addBatch({
  'class generates instances': { //context
    'when called with new': function (lwesClass) { //vow
      var lwesObj = new LWES();
      assert.isObject(lwesObj);
    },  
    'when called without new': function (logos) { //vow
      var lwesObj = LWES();
      assert.isObject(lwesObj);
    }   
  },
  'instances have a specific set of methods and objects': { // context
    topic: new LWES(),
      'that include an Event object': function (lwes) { // vow
        assert.isFunction(lwes.Event);
      },  
      'that include a createListener method ': function (lwes) { // vow
        assert.isFunction(lwes.createListener);
      }  
  }  
}).export(module); // Export the Suite

