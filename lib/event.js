function ipdec_to_ipstr(num) {
  var d = num % 256;
  for (var i = 3; i > 0; i--) { 
    num = Math.floor(num/256);
    d = (num % 256) + '.' + d;
  }
  return d;
}

var event = function(type){
  this.type = type;
  this.attributes = {};
  this.encoding = {};
};

event.prototype = {
  getAttr : function(name) {
    return this.attributes[name];
  },

  setAttr : function(name, type, value) {
    this.attributes[name] = {}
    this.attributes[name]['type'] = type;
    if ( type === 'IPADDR' && typeof value === 'number' ) {
      this.attributes[name]['value'] = ipdec_to_ipstr(value);
    } else {
      this.attributes[name]['value'] = value;
    }
  }
}

exports.Event = event;
