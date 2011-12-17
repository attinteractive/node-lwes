var Parser = require('packet').Parser;

// Monkey patch node-packet until bug is resolve
// Parser mixes-in packet
Parser.prototype.pipeline = function(pattern, value) {
  var constant, parameters, transform, _i, _j, _len, _len2, _ref, _ref2;
  if (pattern.transforms) {
    _ref = pattern.transforms;
    for (_i = 0, _len = _ref.length; _i < _len; _i++) {
      transform = _ref[_i];
      parameters = []
      _ref2 = transform.parameters;
      for (_j = 0, _len2 = _ref2.length; _j < _len2; _j++) {
        constant = _ref2[_j];
        parameters.push(constant);
      } 
      parameters.push(!this.outgoing);
      parameters.push(pattern);
      parameters.push(value);
      if ( typeof this.transforms[transform.name] !== "undefined") {      // PATCH
        value = this.transforms[transform.name].apply(null, parameters);
      }                                                                   // PATC
    } 
  } 
  return value;
}
 
var Event = require('./event').Event;
var Type = require('./type');
var parser = new Parser();

parser.packet('BOOLEAN',        'b8|bool()'       );
parser.packet('BYTE',           'b8'              );
parser.packet('INT16',          '-b16'            );
parser.packet('UINT16',         'b16'             );
parser.packet('INT32',          '-b32'            );
parser.packet('UINT32',         'b32'             );
parser.packet('INT64',          '-b64'            );
parser.packet('UINT64',         'b64'             );
parser.packet('FLOAT',          'b32f'            );
parser.packet('DOUBLE',         'b64f'            );
parser.packet('STRING',         'b16/b8|utf8()'   );
parser.packet('IPADDR',         'l32|ipv4()'      );
parser.packet('IPV4',           'b32|ipv4()'      );
parser.packet('NAME',           'b8/b8|utf8()'    );

parser.packet('BOOLEAN_ARRAY',  'b16/b8|bool()'   );
parser.packet('BYTE_ARRAY',     'b16/b8'          );
parser.packet('INT16_ARRAY',    'b16/-b16'        );
parser.packet('UINT16_ARRAY',   'b16/b16'         );
parser.packet('INT32_ARRAY',    'b16/-b32'        );
parser.packet('UINT32_ARRAY',   'b16/b32'         );
parser.packet('INT64_ARRAY',    'b16/-b64'        );
parser.packet('UINT64_ARRAY',   'b16/b64'         );
parser.packet('FLOAT_ARRAY',    'b16/b32f'        );
parser.packet('DOUBLE_ARRAY',   'b16/b64f'        );
parser.packet('STRING_ARRAY',   'b8|strarry()'    );
parser.packet('IP_ADDR_ARRAY',  'b16/l32|ipv4()'  );
parser.packet('IPV4_ARRAY',     'b16/b32|ipv4()'  );

function parse(b) {
  // make sure we start from beginning of buffer
  parser.bytesRead = 0;

  var e = new Event();
  var type = parseEventName(b);
  var len = parseAttributeLength(b);
  e.type = type;

  if(len > 0) {
    for(var j=0; j < len; j++) {
      var attrName = parseAttributeName(b);
      var attrType = parseAttributeType(b);
      var attrValue = parseAttributeData(b, attrType);
      e.setAttr(attrName, attrType, attrValue);
    }
  }

  return e;
};

parseData = function(b, type) {
  var v = null;
  if(type == 'UNDEFINED'){
    console.log(b);
    throw "Unprocessible!";
  }

  parser.parse(type, function(val) {v = val;});
  parser.read(b, parser.bytesRead);
  return v;
};

parseEventName        = function(b) { return parseData(b,   'NAME'); };
parseAttributeLength  = function(b) { return parseData(b, 'UINT16'); };
parseEncodingName     = function(b) { return parseData(b,   'NAME'); };
parseEncodingType     = function(b) { return parseData(b,   'BYTE'); };
parseEncodingValue    = function(b) { return parseData(b, 'UINT16'); };
parseAttributeName    = function(b) { return parseData(b,   'NAME'); };
parseAttributeType    = function(b) { return Type.idToName(parseData(b,   'BYTE')); };
parseAttributeData    = parseData;

exports.parse = parse;

