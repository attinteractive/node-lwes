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
    this.attributes[name]['value'] = value;
  }
}

exports.Event = event;
